#define _USE_MATH_DEFINES

#include <iostream>
#include <chrono>
#include <vector>
#include <string.h>
#include <cmath>

#include "stb/stb_image_write.h"

constexpr int WIDTH = 512;
constexpr int HEIGHT = 512;
constexpr int MAX_ITERATIONS = 512;

template <typename T, typename = typename std::enable_if<
                          std::is_floating_point<T>::value, T>::type>
int mandelbrot(T x, T y, int max, T scale = 1.0, T offset_x = 0.0,
               T offset_y = 0) {
    T r2 = 2 * 2;

    T zr = 0;
    T zr_tmp = 0;
    T zi = 0;

    scale = std::pow<T>(2.0, scale);
    T cr = x * scale + offset_x;
    T ci = y * scale + offset_y;

    int iterations = 0;

    for (int i = 0; i < max; i++) {
        zr_tmp = zr * zr - zi * zi + cr;
        zi = 2.0 * zr * zi + ci;
        zr = zr_tmp;

        if (zr * zr + zi * zi > r2)
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

int main(int argc, char const* argv[]) {
    const int CHANNELS = 4;
    const int max_iterations = MAX_ITERATIONS;

    std::cout << "Hello Mandelbrot set!\n";

    uint8_t* pixels = new uint8_t[WIDTH * HEIGHT * CHANNELS];
    int* fractals = new int[WIDTH * HEIGHT];

    for (int i = 0; i < HEIGHT; i++) {
        double y = map<double>(i + 1, 1.0, HEIGHT, 1.0, -1.0);
        for (int j = 0; j < WIDTH; j++) {
            double x = map<double>(j + 1, 1.0, HEIGHT, -1.0, 1.0);

            fractals[i * WIDTH + j] =
                mandelbrot<double>(x, y, max_iterations, 1.2);
        }
    }

    int index = 0;
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            int32_t n = fractals[i * WIDTH + j];

            if (n < max_iterations) {
                float c = std::sqrtf((float)n / (float)max_iterations);
                // c = clamp<float>(c, 0, 1);

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

    stbi_write_png("mandelbrot.png", WIDTH, HEIGHT, CHANNELS, pixels,
                   WIDTH * CHANNELS);
    delete[] pixels;
    delete[] fractals;

    return 0;
}
