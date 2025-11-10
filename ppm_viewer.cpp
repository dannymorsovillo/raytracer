#include "rt.h"
#include <SFML/Graphics.hpp>
#include <fstream>
#include <vector>
#include <string>
#include <cstdint>
#include <optional>

struct ImageData {
    unsigned w;
    unsigned h;
    std::vector<std::uint8_t> pixels; 
};

ImageData load_ppm(const std::string& filename) {
    std::ifstream in(filename);
    std::string magic;
    in >> magic;                 
    unsigned w, h;
    int maxv;
    in >> w >> h >> maxv;

    std::vector<std::uint8_t> pixels;
    pixels.reserve(w * h * 4);

    for (unsigned i = 0; i < w * h; ++i) {
        int r, g, b;
        in >> r >> g >> b;
        pixels.push_back(static_cast<std::uint8_t>(r));
        pixels.push_back(static_cast<std::uint8_t>(g));
        pixels.push_back(static_cast<std::uint8_t>(b));
        pixels.push_back(255); 
    }

    return { w, h, std::move(pixels) };
}

int main() {
    auto img = load_ppm("image.ppm");

    sf::RenderWindow window(sf::VideoMode({img.w, img.h}), "PPM Viewer");

    sf::Texture texture({img.w, img.h});
    texture.update(img.pixels.data());

    sf::Sprite sprite(texture);

    while (window.isOpen()) {
        while (auto evt = window.pollEvent()) {
            if (evt->is<sf::Event::Closed>()) {
                window.close();
            }
        }

        window.clear();
        window.draw(sprite);
        window.display();
    }

    return 0;
}
