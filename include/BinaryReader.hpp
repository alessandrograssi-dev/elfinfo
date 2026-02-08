#pragma once 

#include <cstdint>
#include <fstream>
#include <string>
#include <stdexcept>

class BinaryReader {
public:
    explicit BinaryReader(const std::string& filename)
      : m_file(filename, std::ios::in | std::ios::binary) {
        if (!m_file.is_open()) 
          throw std::runtime_error("Cannot open file: " + filename);
        m_file.seekg(0, std::ios::end);
        m_size = m_file.tellg();
        m_file.seekg(0, std::ios::beg);
    }
    
    std::uint64_t size() const {
        return m_size;
    }

    void seek(std::uint64_t offset) {
        m_file.seekg(offset, std::ios::beg);
    }

    // Reads little-endian values assuming host is little-endian.
    // Endianness handling will be extended in later commits.
    template<typename T>
    T read_le() {
        T ui;
        m_file.read(reinterpret_cast<char*>(&ui), sizeof(T));
        return ui;      
    }


private:
    std::ifstream m_file;
    std::uint64_t m_size {0};
};