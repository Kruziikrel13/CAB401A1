#include <vector>
#include <string>
namespace NCommon {
    constexpr size_t   MATRIX_SIZE = 1024;
    std::vector<float> generateRandomMatrix(size_t size);
    void               writeToBinary(std::vector<float>& matrix, std::string filename);
}
