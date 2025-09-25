#include "common.hpp"
#include <iostream>
#include <spdlog/spdlog.h>
#include <chrono>

int main() {
#ifndef NDEBUG
    spdlog::set_level(spdlog::level::debug);
#ifdef TRACE
    spdlog::set_level(spdlog::level::trace);
#endif
#endif

    spdlog::trace("Generating random matrices.");
    std::vector<float> matrixA = NCommon::generateRandomMatrix(NCommon::MATRIX_SIZE);
    std::vector<float> matrixB = NCommon::generateRandomMatrix(NCommon::MATRIX_SIZE);
    std::vector<float> matrixC(NCommon::MATRIX_SIZE * NCommon::MATRIX_SIZE, 0.0f);
    spdlog::trace("Random matrices generated.");

    spdlog::info("Starting sequential matrix multiplication.");
    std::chrono::time_point start = std::chrono::high_resolution_clock::now();
    for (size_t row = 0; row < NCommon::MATRIX_SIZE; ++row) {
        for (size_t col = 0; col < NCommon::MATRIX_SIZE; ++col) {
            float total = 0.0f;
            for (size_t k = 0; k < NCommon::MATRIX_SIZE; ++k) {
                total += matrixA[(row * NCommon::MATRIX_SIZE) + k] * matrixB[(k * NCommon::MATRIX_SIZE) + col];
            }
            matrixC[(row * NCommon::MATRIX_SIZE) + col] = total;
        }
    }
    std::chrono::time_point       end      = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;
    spdlog::info("Computation completed in {} seconds", duration.count());

    std::cout << "Press enter to continue...";
    std::cin.get();

    const size_t print_limit = 12;
    for (size_t i = 0; i < print_limit; i++) {
        for (size_t j = 0; j < print_limit; j++) {
            std::cout << "C[" << i << "][" << j << "] = " << matrixC[(i * NCommon::MATRIX_SIZE) + j] << std::endl;
        }
    }

    return EXIT_SUCCESS;
}
