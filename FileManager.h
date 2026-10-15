#pragma once

#include <string>
#include <vector>
#include <cstdint>
#include <cstddef>
#include <fstream>
#include <filesystem>
#include <cmath>
class FileManager {

private:
    std::string filePath;
    size_t fileSize;
    size_t pieceSize;

public:
    FileManager(int peerId, const std::string& fileName, size_t fileSize, size_t pieceSize, bool hasFile);

    size_t getPieceCount() const;
    size_t getPieceSize(size_t index) const;

    std::vector<uint8_t> readPiece(size_t index);
    void writePiece(size_t index, const std::vector<uint8_t>& data);
};
