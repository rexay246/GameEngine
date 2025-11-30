#pragma once
#include <string>
#include <vector>

// Reads a file fully into a byte vector
bool readFileToVector(const std::string& path, std::vector<uint8_t>& out);
