C++ Ray Tracer
A physically-based path tracing renderer built in C++ following Ray Tracing in One Weekend by Peter Shirley, with additional features including triangle primitives, direct lighting, and real-time preview capabilities.
Features

Path tracing with global illumination
Multiple materials: Lambertian diffuse, metal with adjustable roughness, and dielectric (glass)
Geometric primitives: spheres and triangles
Camera controls: adjustable field of view, look-at positioning, and depth of field
Direct lighting: optional explicit light sampling with soft shadows
Multi-threaded rendering using OpenMP
Real-time preview: SFML-based PPM image viewer
Gamma correction for accurate color display

Sample Output
The default scene renders a collection of randomly placed spheres with varying materials (diffuse, metal, glass) on a ground plane, plus three hero spheres and a triangle, demonstrating the full range of the renderer's capabilities.
Requirements

C++17 compatible compiler (GCC, Clang, or MSVC)
OpenMP (optional, for parallel rendering)
SFML 3.x (optional, for the image viewer)

Building
Main Ray Tracer
bashg++ -std=c++17 -O3 -fopenmp main.cpp -o raytracer
PPM Viewer (optional)
bashg++ -std=c++17 -O3 viewer.cpp -o viewer -lsfml-graphics -lsfml-window -lsfml-system
Usage
Rendering an Image
bash./raytracer > image.ppm
The renderer outputs progress to stderr and the PPM image data to stdout.
Viewing the Result
bash./viewer
The viewer expects an image.ppm file in the current directory.
Configuration
Camera Settings
Edit main.cpp to adjust camera parameters:
cppcam.aspect_ratio = 16.0 / 9.0;      // Image aspect ratio
cam.image_width = 1200;              // Width in pixels
cam.samples_per_pixel = 10;          // Samples per pixel (quality)
cam.max_depth = 50;                  // Maximum ray bounce depth
cam.vfov = 20;                       // Vertical field of view (degrees)
cam.lookfrom = point3(13, 2, 3);    // Camera position
cam.lookat = point3(0, 0, 0);       // Look-at point
cam.vup = vec3(0, 1, 0);            // Up vector
cam.defocus_angle = 0.6;             // Depth of field blur
cam.focus_dist = 10.0;               // Focus distance
Direct Lighting (optional)
Enable explicit light sampling for faster convergence:
cppcam.enable_direct_lighting = true;
cam.light_pos = point3(5, 5, -2);
cam.light_intensity = color(7, 7, 7);
cam.soft_shadow_samples = 4;         // Samples for soft shadows
cam.light_radius = 0.5;              // Light source size
Materials
Create materials and add objects to the scene:
cpp// Lambertian (diffuse)
auto diffuse = make_shared<lambertian>(color(0.7, 0.3, 0.3));
world.add(make_shared<sphere>(point3(0, 0, -1), 0.5, diffuse));

// Metal
auto metal = make_shared<metal>(color(0.8, 0.8, 0.8), 0.3);  // color, fuzz
world.add(make_shared<sphere>(point3(1, 0, -1), 0.5, metal));

// Dielectric (glass)
auto glass = make_shared<dielectric>(1.5);  // refractive index
world.add(make_shared<sphere>(point3(-1, 0, -1), 0.5, glass));

// Triangle
auto triangle_mat = make_shared<lambertian>(color(0.6, 0.6, 0.6));
world.add(make_shared<triangle>(
    point3(-1, 0, -1), 
    point3(1, 0, -1), 
    point3(0, 1, -1), 
    triangle_mat
));
Project Structure
├── main.cpp              # Scene setup and entry point
├── viewer.cpp            # SFML-based PPM image viewer
├── camera.h              # Camera and rendering logic
├── material.h            # Material definitions (lambertian, metal, dielectric)
├── hittable.h            # Abstract hittable interface
├── hittable_list.h       # Scene object container
├── sphere.h              # Sphere primitive
├── triangle.h            # Triangle primitive
├── ray.h                 # Ray class
├── vec_3.h               # 3D vector math
├── color.h               # Color utilities and gamma correction
├── interval.h            # Interval class for ray intersections
└── rt.h                  # Common utilities and constants
Performance Tips

Increase samples_per_pixel for better quality (10-500 typical)
Enable OpenMP with -fopenmp flag for parallel rendering
Reduce image_width for faster preview renders
Use max_depth of 50 for typical scenes
Direct lighting can significantly speed up convergence for scenes with point lights

Technical Details

Rendering algorithm: Unidirectional path tracing with importance sampling
BxDF models: Lambertian diffuse, Cook-Torrance microfacet metal (simplified), and Fresnel dielectric
Acceleration: Per-thread RNG for thread-safe parallel rendering
Output format: PPM P3 (ASCII)
Color space: Linear RGB with gamma 2.0 correction on output

License
Based on Ray Tracing in One Weekend by Peter Shirley. Extended implementation with additional features.
Acknowledgments

Peter Shirley's Ray Tracing in One Weekend book series
SFML library for cross-platform graphics
