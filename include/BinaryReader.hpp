#pragma once 

#include <cstdint>
#include <fstream>
#include <string>
#include <stdexcept>
#include <type_traits>

class BinaryReader {
    template<typename T>
    T byteswap(T value);

public:
    explicit BinaryReader(const std::string& filename)
      : m_file(filename, std::ios::in | std::ios::binary) {
        if (!m_file.is_open()) 
          throw std::runtime_error("Cannot open file: " + filename);
        m_file.seekg(0, std::ios::end);
        m_size = m_file.tellg();
        m_file.seekg(0, std::ios::beg);
    }
    
    [[nodiscard]] std::uint64_t size() const {
        return m_size;
    }

    void seek(std::uint64_t offset) {
        if (offset > m_size) {
            throw std::out_of_range("Seek past end of file");
        }
        m_file.seekg(offset, std::ios::beg);
    }

    // Reads little-endian values assuming host is little-endian.
    // Endianness handling will be extended in later commits.
    template<typename T>
    bool read_le(T& ui) {
        static_assert(std::is_integral_v<T>,
                  "Error: read_le only supports integral types");

        m_file.read(reinterpret_cast<char*>(&ui), sizeof(T));
        if (!m_file.good()) {
          return false;
        }
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
        value = byteswap(ui);
#endif
        return true;
    }


private:
    std::ifstream m_file;
    std::uint64_t m_size {0};
};