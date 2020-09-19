#include <iostream>
#include <cmath>
#include <chrono>
#include <vector>
#include <string.h>

#include "stb/stb_image_write.h"

constexpr float default_scale = 1.5f;
constexpr float default_radius = 2.f;

int mandelbrot(const float& x, const float& y, const int& max_iteration,
               const float& scale_x = default_scale,
               const float& scale_y = default_scale,
               const float& offset_x = 0.f, const float& offset_y = 0.f,
               const float& radius = default_radius) {
    float r2 = radius * radius;

    float z_real = 0;
    float z_real_tmp = 0;

    float z_imaginary = 0;

    float c_real = x * scale_x + offset_x;
    float c_imaginary = y * scale_y + offset_y;

    int iterations = 0;

    for (int i = 0; i < max_iteration; i++) {
        z_real_tmp = z_real * z_real - z_imaginary * z_imaginary + c_real;
        z_imaginary = 2.f * z_real * z_imaginary + c_imaginary;
        z_real = z_real_tmp;

        // dot product instead of sqrt
        if (z_real * z_real + z_imaginary * z_imaginary > r2)
            break;

        iterations++;
    }

    return iterations;
}

float map(float x, float in_min, float in_max, float out_min, float out_max) {
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void print_console(const int& max_iter) {
    float step = 0.04;
    for (float i = -1; i <= 1; i += step) {
        for (float j = -3.5; j <= 1; j += step) {
            if (mandelbrot(j, i, max_iter, 0.6f, 1.1f) >= max_iter - 200) {
                std::cout << "*";
            } else {
                std::cout << " ";
            }
        }

        std::cout << std::endl;
    }
}

int main(int argc, char const* argv[]) {
    const int width = 8192, height = 8192, channels = 4;
    const int max_iter = 256;

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed;
    auto start = std::chrono::high_resolution_clock::now();

    if (argc > 1 && strcmp(argv[1], "-c") == 0) {
        std::cout << "Generating Mandelbrot set..." << std::endl;
        print_console(max_iter);
        end = std::chrono::high_resolution_clock::now();
        elapsed = end - start;
        std::cout << "Elapsed time: " << elapsed.count() << " s\n";

        return 0;
    }

    std::cout << "Allocating memory..." << std::endl;
    start = std::chrono::high_resolution_clock::now();
    std::vector<uint8_t> pixels(width * height * channels);
    elapsed = std::chrono::high_resolution_clock::now() - start;
    std::cout << "Time took allocating memory: " << elapsed.count() << " s\n";

    std::cout << "Generating Mandelbrot set..." << std::endl;
    start = std::chrono::high_resolution_clock::now();

    int index = 0;
    for (int i = 1; i < height + 1; i++) {
        float map_y = map((float)i, 1.0, (float)height, 1.0, -1.0);

        for (int j = 1; j < width + 1; j++) {
            float map_x = map((float)j, 1.0, (float)width, -1.0, 1.0);
            int n = mandelbrot(map_x, map_y, max_iter);

            if (n == max_iter) {
                pixels[index++] = 0;
                pixels[index++] = 0;
                pixels[index++] = 0;
                pixels[index++] = 255;
                continue;
            }

            pixels[index++] = 255;
            pixels[index++] = 255;
            pixels[index++] = 255;
            pixels[index++] = 255;
        }
    }

    end = std::chrono::high_resolution_clock::now();
    elapsed = end - start;
    std::cout << "Elapsed time: " << elapsed.count() << " s\n";

    start = std::chrono::high_resolution_clock::now();

    std::cout << "Writing to disk..." << std::endl;
    stbi_write_png("mandelbrot.png", width, height, channels, &pixels[0],
                   width * channels); // &pixels[0] kind of hacky

    end = std::chrono::high_resolution_clock::now();
    elapsed = end - start;
    std::cout << "Time took to write: " << elapsed.count() << " s\n";

    return 0;
}
