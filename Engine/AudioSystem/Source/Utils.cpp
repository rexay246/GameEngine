#include "Utils.h"
#include <fstream>

bool readFileToVector(const std::string& path, std::vector<uint8_t>& out)
{
    std::ifstream file(path, std::ios::binary);
    if (!file) return false;
    file.seekg(0, std::ios::end);
    size_t size = static_cast<size_t>(file.tellg());
    file.seekg(0, std::ios::beg);
    out.resize(size);
    file.read(reinterpret_cast<char*>(out.data()), size);
    return true;
}
