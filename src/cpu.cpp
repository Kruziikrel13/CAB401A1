#include "common.hpp"
#include <spdlog/spdlog.h>
#include <chrono>

int main() {
    constexpr size_t N = 1024;
#ifndef NDEBUG
    spdlog::set_level(spdlog::level::debug);
#ifdef TRACE
    spdlog::set_level(spdlog::level::trace);
#endif
#endif

    spdlog::trace("Generating random matrices.");
    std::vector<float> matrixA = NCommon::generateRandomMatrix(N);
    std::vector<float> matrixB = NCommon::generateRandomMatrix(N);
    std::vector<float> matrixC = NCommon::generateRandomMatrix(N);
    spdlog::trace("Random matrices generated.");

    std::chrono::time_point start = std::chrono::high_resolution_clock::now();

    for (size_t i = 0; i < N; i++) {
        for (size_t j = 0; j < N; j++) {
            int total = 0;
            for (size_t k = 0; k < N; k++) {
                total += matrixA[(i * N) + k] * matrixB[(k * N) + j];
            }
            matrixC[(i * N) + j] = total;
        }
    }

    std::chrono::time_point       end      = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;
    spdlog::info("Computation completed in {} seconds", duration.count());

    NCommon::writeToBinary(matrixC, "cpu.bin");

    return EXIT_SUCCESS;
}
