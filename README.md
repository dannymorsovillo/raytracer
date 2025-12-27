# C++ Path Tracer

A compact, physically-based path tracing renderer implemented in modern C++.  
This project follows the techniques from Peter Shirley's *Ray Tracing in One Weekend* and extends them with triangle primitives, direct lighting, and a lightweight SFML-based PPM previewer.

---

## Features

- Unidirectional path tracing with importance sampling (global illumination)
- Materials:
  - Lambertian diffuse
  - Metal with adjustable roughness (simple microfacet-like model)
  - Dielectric (glass) with Fresnel handling
- Geometric primitives:
  - Spheres
  - Triangles
- Optional explicit direct-light sampling with soft shadows
- Camera controls: aspect ratio, field of view, look-at positioning, depth of field
- Multi-threaded rendering using OpenMP
- Real-time preview tool using SFML (optional)
- Gamma correction (gamma = 2.0) on output

---

## Sample Output

The default scene includes a ground plane, a collection of randomly distributed spheres with different materials (diffuse, metal, glass), three prominent ("hero") spheres, and a triangle to showcase triangle intersection correctness and shading behavior.

---

## Requirements

- C++17 compatible compiler (GCC / Clang / MSVC)
- OpenMP (optional, recommended for parallel rendering)
- SFML 3.x (optional, for the PPM viewer)

---

## Building

Open a terminal in the repository root.

- Build the renderer:
```bash
g++ -std=c++17 -O3 -fopenmp main.cpp -o raytracer
```

- Build the optional PPM viewer:
```bash
g++ -std=c++17 -O3 viewer.cpp -o viewer -lsfml-graphics -lsfml-window -lsfml-system
```

Notes:
- On systems without OpenMP, remove `-fopenmp`. Performance will be single-threaded.
- Linker flags for SFML may differ by platform/package manager. Adjust as necessary.

---

## Usage

- Render an image (PPM ASCII format to stdout):
```bash
./raytracer > image.ppm
```
The renderer writes progress information to stderr and the PPM image to stdout.

- View the generated image (optional viewer):
```bash
./viewer
```
The viewer expects `image.ppm` in the current directory.

---

## Configuration

Most scene and rendering settings are exposed in `main.cpp` and the camera utility. Example camera / rendering parameters that can be edited in `main.cpp`:

```cpp
// Image
cam.aspect_ratio = 16.0 / 9.0;
cam.image_width = 1200;
cam.samples_per_pixel = 10;
cam.max_depth = 50;

// Camera (positioning, DOF)
cam.vfov = 20.0;                        // vertical field of view in degrees
cam.lookfrom = point3(13, 2, 3);
cam.lookat = point3(0, 0, 0);
cam.vup = vec3(0, 1, 0);
cam.focus_dist = 10.0;
cam.defocus_angle = 0.6;                // depth of field blur

// Direct lighting (optional)
cam.enable_direct_lighting = true;
cam.light_pos = point3(5, 5, -2);
cam.light_intensity = color(7, 7, 7);
cam.soft_shadow_samples = 4;
cam.light_radius = 0.5;
```

Enable or tune these values to trade off between quality, noise, and render time.

---

## Scene & Materials Example

Objects and materials are created and added to the scene in `main.cpp`. Example snippets:

```cpp
// Lambertian (diffuse)
auto diffuse = make_shared<lambertian>(color(0.7, 0.3, 0.3));
world.add(make_shared<sphere>(point3(0, 0, -1), 0.5, diffuse));

// Metal
auto metal = make_shared<metal>(color(0.8, 0.8, 0.8), 0.3);
world.add(make_shared<sphere>(point3(1, 0, -1), 0.5, metal));

// Dielectric (glass)
auto glass = make_shared<dielectric>(1.5);
world.add(make_shared<sphere>(point3(-1, 0, -1), 0.5, glass));

// Triangle
auto triangle_mat = make_shared<lambertian>(color(0.6, 0.6, 0.6));
world.add(make_shared<triangle>(
    point3(-1, 0, -1),
    point3(1, 0, -1),
    point3(0, 1, -1),
    triangle_mat
));
```

---

## Project Structure

- main.cpp              — Scene setup and renderer entry point
- viewer.cpp            — SFML-based PPM image viewer (optional)
- camera.h              — Camera, projection, and rendering parameters
- material.h            — Material models (Lambertian, Metal, Dielectric)
- hittable.h            — Abstract hittable interface
- hittable_list.h       — Scene object container
- sphere.h              — Sphere primitive
- triangle.h            — Triangle primitive and intersection code
- ray.h                 — Ray class and utilities
- vec_3.h               — 3D vector math helpers
- color.h               — Color utilities and gamma correction
- interval.h            — Intersection interval helpers
- rt.h                  — Common utilities and constants

---

## Performance Tips

- Increase `samples_per_pixel` for higher quality (typical range 10–500; higher = less noise).
- Compile with OpenMP enabled (`-fopenmp`) to use multiple CPU cores.
- Lower `image_width` for faster test renders.
- Use `max_depth` ≈ 50 for typical scenes; reduce for faster, less accurate renders.
- Enable explicit direct lighting to improve convergence for scenes with strong local light sources.

---

## Technical Details

- Rendering algorithm: Unidirectional path tracing with importance sampling
- Material models: Lambertian diffuse, simplified microfacet metal-like model, Fresnel dielectric
- Threading: Per-thread RNG for safe parallel rendering
- Output: PPM P3 (ASCII)
- Color space: Linear RGB with gamma 2.0 correction on write

---

## License

This repository is based on the ideas and examples from Peter Shirley's *Ray Tracing in One Weekend*. See the repository's LICENSE file for full license details (if present). If you plan to reuse code or redistribute, please follow the license terms and acknowledge the original source.

---

## Acknowledgements

- Peter Shirley — Ray Tracing in One Weekend
- SFML — Simple and Fast Multimedia Library (viewer)

---

