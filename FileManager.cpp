#include "FileManager.h"

FileManager::FileManager(int peerId, const std::string& fileName, size_t fileSize, size_t pieceSize, bool hasFile) {
    if (pieceSize == 0) {
        throw std::runtime_error("Piece size is zero");
    }

    this->fileSize = fileSize;
    this->pieceSize = pieceSize;

    std::string dir = "peer_" + std::to_string(peerId);
    std::filesystem::create_directories(dir);

    this->filePath = dir + "/" + fileName;

    if (hasFile) {
        std::ifstream file(filePath, std::ios::binary | std::ios::ate);

        if (!file.is_open()) {
            throw std::runtime_error("Source file doesn't exsist: " + filePath); 
        }
        std::streamsize size = file.tellg();
        file.close();

        if (size == std::streampos(-1)) {
            throw std::runtime_error("Unable to read file size");
        }

        if (static_cast<size_t>(size) != fileSize) {
            throw std::runtime_error("Source file has wrong size: " + filePath);
        }
    } else {
        std::ofstream file(filePath, std::ios::binary);

        if (!file.is_open()) {
            throw std::runtime_error("Unable to create destination file: " + filePath);
        }

        file.close();
    }
}

size_t FileManager::getPieceCount() const {
    if (fileSize == 0) {
        return 0;
    }

    return 1 + (fileSize - 1) / pieceSize;
}

size_t FileManager::getPieceSize(size_t index) const {
    size_t pieceCount = getPieceCount();

    if (index >= pieceCount) {
        throw std::runtime_error("Invalid piece index: " + std::to_string(index));
    }

    if (index == pieceCount - 1) {
        return fileSize - (pieceSize * index);
    }

    return pieceSize;
}

void FileManager::writePiece(size_t index, const std::vector<uint8_t>& data) {
    size_t currPieceSize = getPieceSize(index);

    if (data.size() != currPieceSize) {
        throw std::runtime_error("Piece data greater than piece size at index: " + std::to_string(index));
    }

    std::fstream file(filePath, std::ios::in | std::ios::out | std::ios::binary);

    if (!file.is_open()) {
        throw std::runtime_error("Unable to open file to write piece: " + filePath);
    }
    if (index != 0) {
        file.seekp(index * pieceSize, std::ios::beg);
    }

    file.write(reinterpret_cast<const char*>(data.data()), data.size());
    if (!file) {
        throw std::runtime_error("Failed to write to file: " + filePath);
    }

    file.close();
}

std::vector<uint8_t> FileManager::readPiece(size_t index) {
    size_t currPieceSize = getPieceSize(index);

    std::ifstream file(filePath, std::ios::out | std::ios::binary);

    if (!file.is_open()) {
        throw std::runtime_error("Unable to open file to write piece: " + filePath);
    }
    if (index != 0) {
        file.seekg(index * pieceSize, std::ios::beg);
    }

    std::vector<uint8_t> buffer(currPieceSize);
    file.read(reinterpret_cast<char*>(buffer.data()), currPieceSize);
    file.close();

    return buffer;
}

