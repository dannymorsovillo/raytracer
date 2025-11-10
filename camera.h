#ifndef CAMERA_H
#define CAMERA_H

#include "rt.h"        
#include "hittable.h"
#include "material.h"
#include "color.h"      
#include <vector>
#include <algorithm>

#ifdef _OPENMP
#include <omp.h>
#endif

class camera {
  public:
    double aspect_ratio = 1.0;
    int    image_width = 100;
    int    samples_per_pixel = 10;
    int    max_depth = 10;

    double vfov = 90;                         
    point3 lookfrom = point3(0,0,0);
    point3 lookat   = point3(0,0,-1);
    vec3   vup      = vec3(0,1,0);

    double defocus_angle = 0;                  
    double focus_dist = 10;

    bool   enable_direct_lighting = false;    
    point3 light_pos = point3(5,5,-2);         
    color  light_intensity = color(7,7,7);    
    int    soft_shadow_samples = 1;            
    double light_radius = 0.0;                

    void render(const hittable& world) {
        initialize();

        std::vector<color> framebuffer(image_width * image_height);

        std::clog << "Rendering " << image_width << "x" << image_height
                  << " spp=" << samples_per_pixel
                  << (enable_direct_lighting ? " + direct light\n" : "\n");

        #pragma omp parallel for schedule(static)
        for (int j = 0; j < image_height; ++j) {
            for (int i = 0; i < image_width; ++i) {
                color pixel_color(0,0,0);
                for (int s = 0; s < samples_per_pixel; ++s) {
                    ray r = get_ray(i, j);
                    pixel_color += ray_color(r, max_depth, world);
                }
                framebuffer[j*image_width + i] = pixel_samples_scale * pixel_color;
            }
            #pragma omp critical
            std::clog << "\rScanlines remaining: " << (image_height - j - 1) << ' ';
        }

        std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";
        for (int j = 0; j < image_height; ++j)
            for (int i = 0; i < image_width; ++i)
                write_color(std::cout, framebuffer[j*image_width + i]);

        std::clog << "\rDone.                             \n";
    }

    inline bool visible_to_light(const point3& p, const point3& L, const hittable& world) const {
        vec3 toL = L - p;
        double dist = toL.length();
        ray shadow_ray(p + 1e-4 * unit_vector(toL), toL);
        hit_record occl;
        return !world.hit(shadow_ray, interval(0.0, dist - 1e-4), occl);
    }

  private:
    int    image_height = 1;
    double pixel_samples_scale = 1.0;
    point3 center;                 
    point3 pixel00_loc;            
    vec3   pixel_delta_u;         
    vec3   pixel_delta_v;          
    vec3   u, v, w;                
    vec3   defocus_disk_u;        
    vec3   defocus_disk_v;        

    void initialize() {
        image_height = int(image_width / aspect_ratio);
        if (image_height < 1) image_height = 1;
        pixel_samples_scale = 1.0 / samples_per_pixel;

        center = lookfrom;

        w = unit_vector(lookfrom - lookat);
        u = unit_vector(cross(vup, w));
        v = cross(w, u);

        auto theta = degrees_to_radians(vfov);
        auto h = std::tan(theta/2);
        auto viewport_height = 2 * h * focus_dist;
        auto viewport_width  = viewport_height * (double(image_width)/image_height);

        vec3 viewport_u = viewport_width * u;
        vec3 viewport_v = viewport_height * -v;

        pixel_delta_u = viewport_u / image_width;
        pixel_delta_v = viewport_v / image_height;

        auto viewport_upper_left = center - (focus_dist * w) - viewport_u/2 - viewport_v/2;
        pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);

        auto defocus_radius = focus_dist * std::tan(degrees_to_radians(defocus_angle / 2));
        defocus_disk_u = u * defocus_radius;
        defocus_disk_v = v * defocus_radius;
    }

    ray get_ray(int i, int j) const {
        vec3 offset(random_double() - 0.5, random_double() - 0.5, 0);
        point3 pixel_sample = pixel00_loc
                            + ((i + offset.x()) * pixel_delta_u)
                            + ((j + offset.y()) * pixel_delta_v);

        point3 origin = (defocus_angle <= 0) ? center : defocus_disk_sample();
        vec3   direction = pixel_sample - origin;
        return ray(origin, direction);
    }

    point3 defocus_disk_sample() const {
        vec3 p = random_in_unit_disk();
        return center + (p.x() * defocus_disk_u) + (p.y() * defocus_disk_v);
    }

    point3 sample_on_light(const point3& C, double R) const {
        if (R <= 0.0) return C;                         
        vec3 d = random_in_unit_disk();
        return point3(C.x() + R * d.x(), C.y(), C.z() + R * d.y());
    }

    color direct_light_sampled(const hit_record& rec,
                               const hittable& world,
                               const point3& C,
                               double R,
                               const color& Li,
                               int shadow_samples) const
    {
        if (!enable_direct_lighting) return color(0,0,0);

        int n = std::max(1, shadow_samples);
        color sum(0,0,0);

        for (int s = 0; s < n; ++s) {
            point3 Ls = sample_on_light(C, R);
            vec3   Ldir = unit_vector(Ls - rec.p);
            double ndotl = std::max(0.0, dot(rec.normal, Ldir));
            if (ndotl <= 0.0) continue;

            if (visible_to_light(rec.p, Ls, world)) {
                double dist2 = (Ls - rec.p).length_squared();
                sum += Li * (ndotl / dist2);
            }
        }
        return sum / double(n);
    }

    color ray_color(const ray& r, int depth, const hittable& world) const {
        if (depth <= 0) return color(0,0,0);

        hit_record rec;
        if (world.hit(r, interval(0.001, infinity), rec)) {
          
            color out = rec.mat->emitted();

            color Ld = enable_direct_lighting
                ? direct_light_sampled(rec, world, light_pos, light_radius, light_intensity, soft_shadow_samples)
                : color(0,0,0);

            ray   scattered;
            color attenuation;
            if (rec.mat->scatter(r, rec, attenuation, scattered)) {
                out += attenuation * ray_color(scattered, depth - 1, world);
                out += attenuation * Ld;   
            } else {
                out += Ld;                 
            }
            return out;
        }

        vec3 unit_direction = unit_vector(r.direction());
        auto a = 0.5 * (unit_direction.y() + 1.0);
        return (1.0 - a) * color(1.0, 1.0, 1.0) + a * color(0.5, 0.7, 1.0);
    }
};

#endif
