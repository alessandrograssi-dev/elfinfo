#include "ElfParser.hpp"
#include <iostream>
#include <cstring>
#include <memory>
#include <iomanip>
#include <sstream>

namespace elf {
    ElfParser::ElfParser(const std::string& path)
        : m_binReader(path)
    {}

    void ElfParser::read_headers() {
        ElfHeader header{};
        parse_e_ident(header);
        parse_header_body(header);
        print_header(header);
    }

    void ElfParser::read_sections() {
        // TODO: implement
    }

    void ElfParser::parse_e_ident(ElfHeader& h) {
        const char magic_number[] = {0x7F, 'E', 'L', 'F'};

        char e_ident[16];
        if (!m_binReader.read_bytes(e_ident, sizeof(e_ident)))
            throw std::runtime_error("Unexpected EOF while reading e_ident");

        for (int i=0; i<4; ++i) {
            if (magic_number[i] != e_ident[i])
                throw std::runtime_error("Invalid ELF magic number");
        }

        if (e_ident[4] != 1 && e_ident[4] != 2)
            throw std::runtime_error("Error: bit for 32/64 bits format is malformed");
        h.e_class = static_cast<ElfClass>(e_ident[4]);
        h.e_endianness = static_cast<ElfEndianness>(e_ident[5]);
        if (h.e_endianness == ElfEndianness::BigEndian)
            m_binReader.set_endianness(BinaryReader::Endianness::BigEndian);

        h.e_version = static_cast<uint32_t>(e_ident[6]);
        if (h.e_version != 1)
            throw std::runtime_error("Error: version should be 1");
        h.e_osabi = static_cast<ElfOSABI>(e_ident[7]);
        h.e_abiversion = static_cast<uint8_t>(e_ident[8]);
    }

    void ElfParser::parse_header_body(ElfHeader& h) {
        if (!m_binReader.read(h.e_type))
            throw std::runtime_error("Error while reading e_type");
        if (!m_binReader.read(h.e_machine))
            throw std::runtime_error("Error while reading e_machine");
        if (!m_binReader.read(h.e_version))
            throw std::runtime_error("Error while reading e_version");
        
        // Differences between Elf32 and Elf64
        if (h.e_class == ElfClass::Elf32)  {
            uint32_t tmp;
            if (!m_binReader.read(tmp))
                throw std::runtime_error("Error while reading e_entry");
            h.e_entry = tmp;
            if (!m_binReader.read(tmp))
                throw std::runtime_error("Error while reading e_phoff");
            h.e_phoff = tmp;
            if (!m_binReader.read(tmp))
                throw std::runtime_error("Error while reading e_shoff");
            h.e_shoff = tmp;
        } else {
            if (!m_binReader.read(h.e_entry))
                throw std::runtime_error("Error while reading e_entry");
            if (!m_binReader.read(h.e_phoff))
                throw std::runtime_error("Error while reading e_phoff");
            if (!m_binReader.read(h.e_shoff))
                throw std::runtime_error("Error while reading e_shoff");
        }
        // End differences

        if (!m_binReader.read(h.e_flags))
            throw std::runtime_error("Error while reading e_flags");
        if (!m_binReader.read(h.e_ehsize))
            throw std::runtime_error("Error while reading e_ehsize");
        if (!m_binReader.read(h.e_phentsize))
            throw std::runtime_error("Error while reading e_phentsize");
        if (!m_binReader.read(h.e_phnum))
            throw std::runtime_error("Error while reading e_phnum");
        if (!m_binReader.read(h.e_shentsize))
            throw std::runtime_error("Error while reading e_shentoff");
        if (!m_binReader.read(h.e_shnum))
            throw std::runtime_error("Error while reading e_shnum");
        if (!m_binReader.read(h.e_shstrndx))
            throw std::runtime_error("Error while reading e_shstrndx");
    }


    void ElfParser::print_header(const ElfHeader& header) const {
        std::cout << "ELF Header:\n";
        std::cout << "  Magic:   " << "7f 45 4c 46 ";
        std::cout << std::hex << std::setw(2) << std::setfill('0');
        std::cout << static_cast<int>(header.e_class) << ' ';
        std::cout << std::hex << std::setw(2) << std::setfill('0');
        std::cout << static_cast<int>(header.e_endianness) << ' ';
        std::cout << std::hex << std::setw(2) << std::setfill('0');
        std::cout << static_cast<int>(header.e_version) << ' ';
        std::cout << std::hex << std::setw(2) << std::setfill('0');
        std::cout << static_cast<int>(header.e_osabi) << ' ';
        std::cout << std::hex << std::setw(2) << std::setfill('0');
        std::cout << static_cast<int>(header.e_abiversion) << ' ';
        std::cout << "00 00 00 00 00 00 00\n";



        if (header.e_class == ElfClass::Elf32)
            std::cout << "  Class:                             ELF32\n";
        else
            std::cout << "  Class:                             ELF64\n";

        switch (header.e_endianness)
        {
        case ElfEndianness::LittleEndian:
            std::cout << "  Data:                              2's complement, little endian\n";
            break;
        case ElfEndianness::BigEndian:
            std::cout << "  Data:                              2's complement, big endian\n";
            break;           
        }
        std::cout << "  Version:                           1 (current)\n";
        std::cout << "  OS/ABI:                            " << os_abi_names(header.e_osabi) << '\n';
        std::cout << "  ABI Version:                       " << (int)header.e_abiversion << '\n';
        std::cout << "  Type:                              " << type_name(header.e_type) << '\n';
        std::cout << "  Machine:                           " << machine_name(header.e_machine) << '\n';
        std::cout << "  Version:                           0x1\n";
        std::cout << "  Entry point address:               0x" << std::hex << header.e_entry << '\n';
        std::cout << "  Start of program headers:          " << std::dec << header.e_phoff << " (bytes into file)\n";
        std::cout << "  Start of section headers:          " << std::dec << header.e_shoff << " (bytes into file)\n";
        std::cout << "  Flags:                             0x" << std::hex << header.e_flags << '\n';
        std::cout << "  Size of this header:               " << std::dec << header.e_ehsize << " (bytes)\n";
        std::cout << "  Size of program headers:           " << std::dec << header.e_phentsize << " (bytes)\n";
        std::cout << "  Number of program headers:         " << std::dec << header.e_phnum << '\n';
        std::cout << "  Size of section headers:           " << std::dec << header.e_shentsize << " (bytes)\n";
        std::cout << "  Number of section headers:         " << std::dec << header.e_shnum << '\n';
        std::cout << "  Section header string table index: " << std::dec << header.e_shstrndx << '\n';
    }

}