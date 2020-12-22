#define _USE_MATH_DEFINES

#include <iostream>
#include <chrono>
#include <vector>
#include <string.h>
#include <cmath>

#include "stb/stb_image_write.h"

constexpr int default_width = 4096;
constexpr int default_height = 4096;

constexpr float default_scale = 1.5f;
constexpr float default_radius = 2.f;
constexpr float default_offset_x = -0.25f;
constexpr float default_offset_y = 0.f;
constexpr int default_max_iterations = 512;

int mandelbrot(const float& x, const float& y, const int& max_iteration,
               const float& scale_x = default_scale,
               const float& scale_y = default_scale,
               const float& offset_x = default_offset_x,
               const float& offset_y = default_offset_y,
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

template <typename T, typename = typename std::enable_if<
                          std::is_arithmetic<T>::value, T>::type>
T clamp(T x, T min, T max) {
    return x < max ? (x > min ? x : min) : max;
}

template <typename T, typename = typename std::enable_if<
                          std::is_arithmetic<T>::value, T>::type>
T map(T x, T in_min, T in_max, T out_min, T out_max) {
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
    const int width = default_width, height = default_height, channels = 4;
    const int max_iter = default_max_iterations;

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

    std::cout << "Allocating " << width * height * channels * sizeof(uint8_t)
              << " bytes of memory for image..." << std::endl;
    start = std::chrono::high_resolution_clock::now();
    uint8_t* pixels = new uint8_t[width * height * channels];
    elapsed = std::chrono::high_resolution_clock::now() - start;
    std::cout << "Time took allocating memory for image: " << elapsed.count()
              << "s\n";

    std::cout << "Allocating " << width * height * sizeof(int32_t)
              << " bytes of memory for Mandelbrot...\n";
    start = std::chrono::high_resolution_clock::now();
    int32_t* fractals = new int32_t[width * height];
    elapsed = std::chrono::high_resolution_clock::now() - start;
    std::cout << "Time took: " << elapsed.count() << "s\n";

    std::cout << "Generating Mandelbrot set..." << std::endl;
    start = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < height; i++) {
        float map_y = map<float>((float)(i + 1), 1.0, (float)height, 1.0, -1.0);

        for (int j = 0; j < width; j++) {
            float map_x =
                map<float>((float)(j + 1), 1.0f, (float)width, -1.0f, 1.0f);

            fractals[i * width + j] = mandelbrot(map_x, map_y, max_iter);
        }
    }

    end = std::chrono::high_resolution_clock::now();
    elapsed = end - start;
    std::cout << "Elapsed time: " << elapsed.count() << " s\n";

    std::cout << "Generating image...\n";
    start = std::chrono::high_resolution_clock::now();

    int index = 0;
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            int32_t n = fractals[i * width + j];

            if (n < max_iter) {
                float c = map<float>(n, 0, max_iter, 0, 1);
                c = clamp<float>(c, 0, 1);

                int r = (int)((sin(.3f * 12.f * c - M_PI / 2.f) + 1.f) * 0.5 *
                              255.f);
                int g = (int)((sin(.1f * 12.f * c - M_PI / 2.f) + 1.f) * 0.5 *
                              255.f);
                int b = (int)((sin(.5f * 12.f * c - M_PI / 2.f) + 1.f) * 0.5 *
                              255.f);

                pixels[index++] = r;
                pixels[index++] = g;
                pixels[index++] = b;
                pixels[index++] = 255;
            } else {
                pixels[index++] = 5;
                pixels[index++] = 5;
                pixels[index++] = 5;
                pixels[index++] = 255;
            }
        }
    }

    elapsed = std::chrono::high_resolution_clock::now() - start;
    std::cout << "Time took generating image: " << elapsed.count() << "s\n";

    start = std::chrono::high_resolution_clock::now();
    std::cout << "Writing to disk..." << std::endl;
    stbi_write_png("mandelbrot.png", width, height, channels, pixels,
                   width * channels);

    end = std::chrono::high_resolution_clock::now();
    elapsed = end - start;
    std::cout << "Time took to write: " << elapsed.count() << " s\n";

    std::cout << "Freeing memory...\n";
    free(pixels);
    free(fractals);

    return 0;
}
