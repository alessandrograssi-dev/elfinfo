#pragma once 

#include <cstdint>
#include <fstream>
#include <string>
#include <stdexcept>
#include <type_traits>

class BinaryReader {
    template<typename T>
    T byteswap(T value) {
        static_assert(std::is_integral_v<T>, "byteswap only supports integral types");

        T result = 0;
        for (size_t i = 0; i < sizeof(T); ++i) {
            result <<= 8;
            result |= (value & 0xFF);
            value >>= 8;
        }
        return result;
    }

public: 
    enum class Endianness {
        LittleEndian    = 1,
        BigEndian       = 2
    };

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

    std::streamoff tell() {
        return m_file.tellg();
    }

    // Reads integral values and applies endianness if needed
    // Endianness handling will be extended in later commits.
    template<typename T>
    bool read(T& ui) {
        static_assert(std::is_integral_v<T>,
                  "Error: read_le only supports integral types");

        
        if (!read_bytes(reinterpret_cast<char*>(&ui), sizeof(T))) {
          return false;
        }
        if (m_machine_endianness != m_file_endianness)
            ui = byteswap(ui); 
        return true;
    }

    bool read_bytes(char *buffer, std::size_t size) {
        m_file.read(buffer, size);
        return m_file.gcount() == static_cast<std::streamsize>(size);
    }

    bool read_string(std::string& s) {
        s.clear();
        char c;
        do {
            m_file.read(&c, 1);
            if (!m_file.good())
                return false;
            if (c != '\0')
                s += c;
        } while (c != '\0');
        return true;
    }

    inline void reset() {
        seek(0);
    }

    inline void set_endianness(Endianness e) {
        m_file_endianness = e;
    }

private:
    std::ifstream m_file;
    std::uint64_t m_size {0};
    Endianness m_file_endianness { Endianness::LittleEndian };
    Endianness m_machine_endianness {host_is_little_endian()};

    Endianness host_is_little_endian() {
        uint16_t x = 1;
        if (*reinterpret_cast<uint8_t*>(&x) == 1) {
            return Endianness::LittleEndian;
        }
        return Endianness::BigEndian;
    }
};