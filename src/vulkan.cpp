#include "VulkanContext.hpp"
#include "common.hpp"
#include <iostream>
#include <spdlog/spdlog.h>
#include <chrono>
#include <vector>

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
    spdlog::trace("Random matrices generated.");

    std::chrono::time_point       start = std::chrono::high_resolution_clock::now();
    CVulkanContext                context(NCommon::MATRIX_SIZE);

    std::vector<float>            matrixC = context.runComputeShader(matrixA.data(), matrixB.data(), NCommon::MATRIX_SIZE);

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
