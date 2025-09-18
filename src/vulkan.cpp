#include "VulkanContext.hpp"
#include "common.hpp"
#include <spdlog/spdlog.h>
#include <chrono>
#include <vector>

int main() {
    constexpr size_t N = 12;
#ifndef NDEBUG
    spdlog::set_level(spdlog::level::debug);
#ifdef TRACE
    spdlog::set_level(spdlog::level::trace);
#endif
#endif
    spdlog::trace("Generating random matrices.");
    std::vector<float> matrixA = NCommon::generateRandomMatrix(N);
    std::vector<float> matrixB = NCommon::generateRandomMatrix(N);
    spdlog::trace("Random matrices generated.");

    std::chrono::time_point       start = std::chrono::high_resolution_clock::now();
    CVulkanContext                context(N);

    std::vector<float>            matrixC = context.runComputeShader(matrixA.data(), matrixB.data(), N);

    std::chrono::time_point       end      = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;
    spdlog::info("Computation completed in {} seconds", duration.count());

    for (size_t i = 0; i < N; i++) {
        for (size_t j = 0; j < N; j++) {
            spdlog::info("C[{}][{}] = {}", i, j, matrixC[(i * N) + j]);
        }
    }

    return EXIT_SUCCESS;
}
