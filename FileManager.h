#pragma once

#include <string>
#include <vector>
#include <cstdint>
#include <cstddef>
using namespace std;

class FileManager {

private:
    string filePath;
    size_t fileSize;
    size_t pieceSize;

public:
    FileManager(int peerId, const string& fileName, size_t fileSize, size_t pieceSize, bool hasFile);

    size_t getPieceCount() const;
    size_t getPieceSize(size_t index) const;

    vector<uint8_t> readPiece(size_t index);
    void writePiece(size_t index, const vector<uint8_t>& data);
};
