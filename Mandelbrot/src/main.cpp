#include <iostream>
#include <string>
#include <fstream>
#include <cmath>
#include <chrono>

#include "stb/stb_image_write.h"

constexpr float default_scale = 1.5f;

int mandelbrot(const float& x, const float& y, const int& max_iteration,
               const float& scale_x = default_scale,
               const float& scale_y = default_scale,
               const float& offset_x = 0.f, const float& offset_y = 0.f,
               const float& radius = 2.f) {
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

int main(int argc, char const* argv[]) {
    const int width = 4096, height = 4096, channels = 4;
    const int max_iter = 256;

    uint8_t* pixels = new uint8_t[width * height * channels];

    std::cout << "Generating Mandelbrot set..." << std::endl;
    auto start = std::chrono::high_resolution_clock::now();

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

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    std::cout << "Elapsed time: " << elapsed.count() << " s\n";

    start = std::chrono::high_resolution_clock::now();

    stbi_write_png("mandelbrot.png", width, height, channels, pixels,
                   width * channels);

    end = std::chrono::high_resolution_clock::now();
    elapsed = end - start;
    std::cout << "Writing took: " << elapsed.count() << " s\n";

    delete[] pixels;

    return 0;
}
