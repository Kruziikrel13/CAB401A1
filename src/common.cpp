#include "common.hpp"
#include <random>
#include <fstream>

static float randomFloat() {
    static std::default_random_engine            generator;
    static std::uniform_real_distribution<float> distribution(0.0f, 1.0f);
    return distribution(generator);
}

std::vector<float> NCommon::generateRandomMatrix(size_t size) {
    std::vector<float> matrix(size * size);
    for (size_t i = 0; i < size * size; i++) {
        matrix[i] = randomFloat();
    }
    return matrix;
}

void NCommon::writeToBinary(std::vector<float>& matrix, std::string filename) {
    std::ofstream file(filename, std::ios::out | std::ios::binary);
    file.write(reinterpret_cast<const char*>(matrix.data()), matrix.size() * sizeof(float));
    file.close();
}
