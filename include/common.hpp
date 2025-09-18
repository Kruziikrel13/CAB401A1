#include <vector>
#include <string>
namespace NCommon {
    std::vector<float> generateRandomMatrix(size_t size);
    void               writeToBinary(std::vector<float>& matrix, std::string filename);
}
