#include "ElfParser.hpp"
#include <iostream>
#include <cstring>
#include <memory>
#include <iomanip>
#include <sstream>
#include <cassert>
#include <ios>

namespace elf {
    ElfParser::ElfParser(const std::string& path)
        : m_binReader(path) {
		parse_header();
	}

    void ElfParser::read_header() {
        print_header(m_header);
    }

	void ElfParser::read_section_headers() {
		std::vector<ElfSectionHeader> s_headers = parse_section_headers();
		print_section_headers(s_headers);
	}

    void ElfParser::read_sections() {
		// TODO
    }

    void ElfParser::parse_header() {
		if (m_header_parsed)
			return;

        parse_e_ident(m_header);
        parse_header_body(m_header);
		m_header_parsed = true;
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
        if (h.e_class == ElfClass::Elf32)
            assert(h.e_ehsize == 52);
        else
            assert(h.e_ehsize == 64);
            
        if (!m_binReader.read(h.e_phentsize))
            throw std::runtime_error("Error while reading e_phentsize");
        if (!m_binReader.read(h.e_phnum))
            throw std::runtime_error("Error while reading e_phnum");
        if (!m_binReader.read(h.e_shentsize))
            throw std::runtime_error("Error while reading e_shentsize");
        if (!m_binReader.read(h.e_shnum))
            throw std::runtime_error("Error while reading e_shnum");
        if (!m_binReader.read(h.e_shstrndx))
            throw std::runtime_error("Error while reading e_shstrndx");
    }


    void ElfParser::print_header(const ElfHeader& header) {
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

	std::vector<ElfSectionHeader> ElfParser::parse_section_headers() {
		std::vector<ElfSectionHeader> s_headers;
        s_headers.reserve(m_header.e_shnum);

		parse_header();
		m_binReader.seek(m_header.e_shoff);
		for (std::size_t i = 0; i<m_header.e_shnum; ++i) {
			ElfSectionHeader sh {};
			parse_section_header(sh);
			s_headers.push_back(sh);
		}
		return s_headers;
	}

    void ElfParser::parse_section_header(ElfSectionHeader& s_header) {
        if (!m_binReader.read(s_header.sh_name)) 
            throw std::runtime_error("Error while reading sh_name");
        if (!m_binReader.read(s_header.sh_type)) 
            throw std::runtime_error("Error while reading sh_type");

        if (m_header.e_class == ElfClass::Elf32) {
            std::uint32_t tmp;
            if (!m_binReader.read(tmp))
                throw std::runtime_error("Error while reading sh_flags");
            s_header.sh_flags = tmp;
            if (!m_binReader.read(tmp))
                throw std::runtime_error("Error while reading sh_addr");
            s_header.sh_addr = tmp;
            if (!m_binReader.read(tmp))
                throw std::runtime_error("Error while reading sh_offset");
            s_header.sh_offset = tmp;
            if (!m_binReader.read(tmp))
                throw std::runtime_error("Error while reading sh_size");
            s_header.sh_size = tmp;
        } else {
            if (!m_binReader.read(s_header.sh_flags))
                throw std::runtime_error("Error while reading sh_flags");
            if (!m_binReader.read(s_header.sh_addr))
                throw std::runtime_error("Error while reading sh_addr");
            if (!m_binReader.read(s_header.sh_offset))
                throw std::runtime_error("Error while reading sh_offset");
            if (!m_binReader.read(s_header.sh_size))
                throw std::runtime_error("Error while reading sh_size");
        }

        if (!m_binReader.read(s_header.sh_link))
            throw std::runtime_error("Error while reading sh_link");
        if (!m_binReader.read(s_header.sh_info))
            throw std::runtime_error("Error while reading sh_info");

		if (m_header.e_class == ElfClass::Elf32) {
			std::uint32_t tmp;
            if (!m_binReader.read(tmp))
                throw std::runtime_error("Error while reading sh_addralign");
            s_header.sh_addralign = tmp;
            if (!m_binReader.read(tmp))
                throw std::runtime_error("Error while reading sh_entsize");
            s_header.sh_entsize = tmp;
		} else {
			if (!m_binReader.read(s_header.sh_addralign))
                throw std::runtime_error("Error while reading sh_addralign");
            if (!m_binReader.read(s_header.sh_entsize))
                throw std::runtime_error("Error while reading sh_entsize");
		}
    }


	void ElfParser::print_section_headers(const std::vector<ElfSectionHeader>& s_headers) {
		std::cout 	<< "There are "<< s_headers.size() << " section headers, starting at offset " 
					<< "0x" << std::hex << m_header.e_shoff << ":\n\n";
		std::cout 	<< "Section Headers:\n";
		std::cout   << "  [Nr] Name              Type             Address           Offset\n";
		std::cout   << "       Size              EntSize          Flags  Link  Info  Align\n";
		for (std::size_t i = 0; i<s_headers.size(); ++i) {
			std::cout << std::setfill(' ') << std::right;
			std::cout << std::setw(7) << std::dec << "  ["  + std::to_string(i) + "] ";
            std::cout << std::left;
			std::cout << std::setw(18) << get_section_name(s_headers, s_headers.at(i).sh_name) + ' ';
			std::cout << std::setw(16) << std::left << SHType_to_cstring(s_headers.at(i).sh_type) << ' ';
            std::cout << std::right;
            std::cout << std::setw(16) << std::setfill('0') << std::hex << s_headers.at(i).sh_addr << "  ";
            std::cout << std::setw(8)  << std::setfill('0') << std::hex << s_headers.at(i).sh_offset << '\n';

            std::cout << std::right;
			std::cout << "       " << std::setw(16) << std::setfill('0') << std::hex << s_headers.at(i).sh_size << "  ";
            std::cout << std::setw(16) << std::setfill('0') << std::hex << s_headers.at(i).sh_entsize;
            std::cout << std::setfill(' ') << std::right;
            std::cout << "  " << std::left << std::setw(2) << SHFlags_to_cstring(s_headers.at(i).sh_flags);
            std::cout << std::right << std::setw(8) << std::dec << s_headers.at(i).sh_link;
            std::cout << std::setw(6) << std::dec << s_headers.at(i).sh_info;
            std::cout << std::setw(6) << std::dec << s_headers.at(i).sh_addralign << '\n';
        }
        
        std::cout << "Key to Flags:\n";
        std::cout << "  W (write), A (alloc), X (execute), M (merge), S (strings), I (info),\n";
        std::cout << "  L (link order), O (extra OS processing required), G (group), T (TLS),\n";
        std::cout << "  C (compressed), x (unknown), o (OS specific), E (exclude),\n";
        std::cout << "  D (mbind), l (large), p (processor specific)\n";

	}

    std::uint32_t ElfParser::get_section_name_offset(const std::vector<ElfSectionHeader>& sh_vec, std::uint32_t offset) const {
        std::uint32_t total_offset = sh_vec.at(m_header.e_shstrndx).sh_offset + offset;
        if (total_offset > m_binReader.size())
            throw std::runtime_error("Error: the offset of a section name is larger than the file size");
        return total_offset;
    }

    std::string ElfParser::get_section_name(const std::vector<ElfSectionHeader>& sh_vec, std::uint32_t sh_name_offset) {
        auto old_offset = m_binReader.tell();
        auto offset = get_section_name_offset(sh_vec, sh_name_offset);
        m_binReader.seek(offset);
        std::string name;
        if (!m_binReader.read_string(name))
            throw std::runtime_error("Error reading a section name");
        m_binReader.seek(old_offset);
        if (name.length() > 17) {
            name = name.substr(0,12) + "[...]";
        }
        return name;
    }

    
}