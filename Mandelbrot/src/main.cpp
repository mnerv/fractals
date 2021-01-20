#define _USE_MATH_DEFINES

#include <iostream>
#include <chrono>
#include <vector>
#include <string.h>
#include <cmath>

#include <thread>
#include <mutex>

#include "stb/stb_image_write.h"

constexpr int WIDTH = 2048;
constexpr int HEIGHT = 2048;
constexpr int MAX_ITERATIONS = 256;

template <typename T, typename = typename std::enable_if<
                          std::is_floating_point<T>::value, T>::type>
int mandelbrot(T x, T y, int max, T scale = 1.0, T offset_x = 0.0,
               T offset_y = 0, T radius = 2.0) {
    T r2 = radius * radius;

    T zr = 0;
    T zr_tmp = 0;
    T zi = 0;

    scale = std::pow(2.0, scale);
    T cr = x * scale + offset_x;
    T ci = y * scale + offset_y;

    int iterations = 0;

    for (int i = 0; i < max; i++) {
        zr_tmp = zr * zr - zi * zi + cr;
        zi = 2.0 * zr * zi + ci;
        zr = zr_tmp;

        if (zr * zr + zi * zi > r2) break;

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

// clang-format off
const std::string TITLE = 
    "-----------------------------------------------------\n"
    "|                 Mandelbrot set                    |\n"
    "-----------------------------------------------------\n";
// clang-format on

int main(int argc, char const* argv[]) {
    const int CHANNELS = 4;
    const int max_iterations = MAX_ITERATIONS;
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed;
    auto start = std::chrono::high_resolution_clock::now();

    volatile bool is_done = false;
    std::cout << TITLE;

    std::cout << "Allocating " << WIDTH * HEIGHT * CHANNELS * sizeof(uint8_t)
              << " bytes of memory for image..." << std::endl;
    start = std::chrono::high_resolution_clock::now();
    uint8_t* pixels = new uint8_t[WIDTH * HEIGHT * CHANNELS];
    std::cout << "Time took: " << elapsed.count() << "s\n\n";

    std::cout << "Allocating " << WIDTH * HEIGHT * sizeof(int32_t)
              << " bytes of memory for Mandelbrot...\n";
    start = std::chrono::high_resolution_clock::now();
    int* fractals = new int[WIDTH * HEIGHT];
    elapsed = std::chrono::high_resolution_clock::now() - start;
    std::cout << "Time took: " << elapsed.count() << "s\n\n";

    const uint32_t n_threads = std::thread::hardware_concurrency();
    std::cout << "Hardware thread count: " << n_threads << "\n";
    const int thread_count = n_threads;
    std::cout << "Thread count: " << thread_count << "\n";
    std::thread* t = new std::thread[thread_count];
    std::mutex cout_lock;

    auto compute_fractal = [&](int row_start, int row_end, int col_start,
                               int col_end) {
        for (int i = row_start; i < row_end; i++) {
            double y = map<double>(i + 1, 1.0, HEIGHT, 1.0, -1.0);
            for (int j = col_start; j < col_end; j++) {
                double x = map<double>(j + 1, 1.0, HEIGHT, -1.0, 1.0);
                fractals[i * WIDTH + j] =
                    mandelbrot<double>(x, y, max_iterations, 1.1);
            }
        }

        cout_lock.lock();
        std::cout << "Thread ID: " << std::this_thread::get_id()
                  << " is done\n";
        cout_lock.unlock();
    };

    auto grid_split = [&](std::thread threads[], const int& thread_count,
                          const int& width, const int& height) {
        double intpart = 0;
        const double fractpart =
            std::modf(std::sqrt((double)thread_count), &intpart);

        const int row_count = (int)intpart;
        const int col_count =
            fractpart > 0.0 ? thread_count / row_count : row_count;

        const int row_offset = height / row_count;
        const int col_offset = width / col_count;

        for (int i = 0; i < row_count; i++) {
            int row_start = row_offset * i;
            int row_end = row_offset * (1 + i);
            for (int j = 0; j < col_count; j++) {
                int col_start = col_offset * j;
                int col_end = col_offset * (j + 1);

                threads[i * col_count + j] = std::thread(
                    compute_fractal, row_start, row_end, col_start, col_end);
            }
        }
    };

    auto row_split = [&](std::thread threads[], const int& thread_count,
                         const int& width, const int& height) {
        const int row_offset = height / thread_count;
        for (int i = 0; i < thread_count; i++) {
            threads[i] = std::thread(compute_fractal, row_offset * i,
                                     row_offset * (i + 1), 0, width);
        }
    };

    auto col_split = [&](std::thread threads[], const int& thread_count,
                         const int& width, const int& height) {
        const int offset = width / thread_count;
        for (int i = 0; i < thread_count; i++) {
            threads[i] = std::thread(compute_fractal, 0, height, offset * i,
                                     offset * (i + 1));
        }
    };

    std::cout << "Generating fractals...\n";
    start = std::chrono::high_resolution_clock::now();

    grid_split(t, thread_count, WIDTH, HEIGHT);
    // row_split(t, thread_count, WIDTH, HEIGHT);
    // col_split(t, thread_count, WIDTH, HEIGHT);

    for (int i = 0; i < thread_count; i++) { t[i].join(); }

    end = std::chrono::high_resolution_clock::now();
    elapsed = end - start;
    std::cout << "Time took: " << elapsed.count() << " s\n\n";

    std::cout << "Generating image...\n";
    start = std::chrono::high_resolution_clock::now();

    int index = 0;
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            int32_t n = fractals[i * WIDTH + j];

            if (n < max_iterations) {
                float c = std::sqrt((float)n / (float)max_iterations);
                // c = clamp<float>(c, 0, 1);

                int r = (int)((sin(.3f * 12.f * c - M_PI / 2.f) + 1.f) * 0.5f *
                              255.f);
                int g = (int)((sin(.1f * 12.f * c - M_PI / 2.f) + 1.f) * 0.5f *
                              255.f);
                int b = (int)((sin(.5f * 12.f * c - M_PI / 2.f) + 1.f) * 0.5f *
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
    std::cout << "Time took: " << elapsed.count() << "s\n\n";

    std::cout << "Writing image to disk...\n";
    start = std::chrono::high_resolution_clock::now();
    stbi_write_png("mandelbrot.png", WIDTH, HEIGHT, CHANNELS, pixels,
                   WIDTH * CHANNELS);
    elapsed = std::chrono::high_resolution_clock::now() - start;
    std::cout << "Time took: " << elapsed.count() << "s\n\n";

    std::cout << "Clean up...\n";
    delete[] pixels;
    delete[] fractals;

    std::cout << "Press enter to exit...";
    std::cin.get();

    return 0;
}
