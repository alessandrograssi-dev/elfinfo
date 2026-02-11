#include "ElfParser.hpp"
#include <iostream>
#include <cstring>
#include <memory>
#include <iomanip>
#include <sstream>
#include <cassert>
#include <ios>
#include <functional>

namespace elf {
    ElfParser::ElfParser(const std::string& path)
        : m_binReader(path), m_header({}), m_sheaders(), m_pheaders() {
		parse_header();
	}

    void ElfParser::read_header() {
        parse_header();
        print_header();
    }

	void ElfParser::read_section_headers() {
        parse_header();
		parse_section_headers();
		print_section_headers();
	}

    void ElfParser::read_sections() {
		// TODO
    }

    void ElfParser::read_program_headers() {
        parse_header();
        parse_program_headers();
        print_program_headers();
		
        parse_section_headers();
        auto map = map_sections_to_segments();
        print_section_to_segment_mapping(map);
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


    void ElfParser::print_header() {
        std::cout << "ELF Header:\n";
        std::cout << "  Magic:   " << "7f 45 4c 46 ";
        std::cout << std::hex << std::setw(2) << std::setfill('0');
        std::cout << static_cast<int>(m_header.e_class) << ' ';
        std::cout << std::hex << std::setw(2) << std::setfill('0');
        std::cout << static_cast<int>(m_header.e_endianness) << ' ';
        std::cout << std::hex << std::setw(2) << std::setfill('0');
        std::cout << static_cast<int>(m_header.e_version) << ' ';
        std::cout << std::hex << std::setw(2) << std::setfill('0');
        std::cout << static_cast<int>(m_header.e_osabi) << ' ';
        std::cout << std::hex << std::setw(2) << std::setfill('0');
        std::cout << static_cast<int>(m_header.e_abiversion) << ' ';
        std::cout << "00 00 00 00 00 00 00\n";



        if (m_header.e_class == ElfClass::Elf32)
            std::cout << "  Class:                             ELF32\n";
        else
            std::cout << "  Class:                             ELF64\n";

        switch (m_header.e_endianness)
        {
        case ElfEndianness::LittleEndian:
            std::cout << "  Data:                              2's complement, little endian\n";
            break;
        case ElfEndianness::BigEndian:
            std::cout << "  Data:                              2's complement, big endian\n";
            break;           
        }
        std::cout << "  Version:                           1 (current)\n";
        std::cout << "  OS/ABI:                            " << os_abi_names(m_header.e_osabi) << '\n';
        std::cout << "  ABI Version:                       " << (int)m_header.e_abiversion << '\n';
        std::cout << "  Type:                              " << type_name(m_header.e_type) << '\n';
        std::cout << "  Machine:                           " << machine_name(m_header.e_machine) << '\n';
        std::cout << "  Version:                           0x1\n";
        std::cout << "  Entry point address:               0x" << std::hex << m_header.e_entry << '\n';
        std::cout << "  Start of program headers:          " << std::dec << m_header.e_phoff << " (bytes into file)\n";
        std::cout << "  Start of section headers:          " << std::dec << m_header.e_shoff << " (bytes into file)\n";
        std::cout << "  Flags:                             0x" << std::hex << m_header.e_flags << '\n';
        std::cout << "  Size of this header:               " << std::dec << m_header.e_ehsize << " (bytes)\n";
        std::cout << "  Size of program headers:           " << std::dec << m_header.e_phentsize << " (bytes)\n";
        std::cout << "  Number of program headers:         " << std::dec << m_header.e_phnum << '\n';
        std::cout << "  Size of section headers:           " << std::dec << m_header.e_shentsize << " (bytes)\n";
        std::cout << "  Number of section headers:         " << std::dec << m_header.e_shnum << '\n';
        std::cout << "  Section header string table index: " << std::dec << m_header.e_shstrndx << '\n';
    }

	void ElfParser::parse_section_headers() {
        if (m_sheader_parsed)
            return;

        m_sheaders.reserve(m_header.e_shnum);

		parse_header();
		m_binReader.seek(m_header.e_shoff);
		for (std::size_t i = 0; i<m_header.e_shnum; ++i) {
			ElfSectionHeader sh {};
			parse_section_header(sh);
			m_sheaders.push_back(sh);
		}
        m_sheader_parsed = true;
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


	void ElfParser::print_section_headers() {
		std::cout 	<< "There are "<< m_sheaders.size() << " section headers, starting at offset " 
					<< "0x" << std::hex << m_header.e_shoff << ":\n\n";
		std::cout 	<< "Section Headers:\n";
		std::cout   << "  [Nr] Name              Type             Address           Offset\n";
		std::cout   << "       Size              EntSize          Flags  Link  Info  Align\n";
		for (std::size_t i = 0; i<m_sheaders.size(); ++i) {
			std::cout << std::setfill(' ') << std::right;
			std::cout << std::setw(7) << std::dec << "  ["  + std::to_string(i) + "] ";
            std::cout << std::left;
			std::cout << std::setw(17) << get_section_name(m_sheaders, m_sheaders.at(i).sh_name, true) << ' ';
			std::cout << std::setw(16) << std::left << SHType_to_cstring(m_sheaders.at(i).sh_type) << ' ';
            std::cout << std::right;
            std::cout << std::setw(16) << std::setfill('0') << std::hex << m_sheaders.at(i).sh_addr << "  ";
            std::cout << std::setw(8)  << std::setfill('0') << std::hex << m_sheaders.at(i).sh_offset << '\n';

            std::cout << std::right;
			std::cout << "       " << std::setw(16) << std::setfill('0') << std::hex << m_sheaders.at(i).sh_size << "  ";
            std::cout << std::setw(16) << std::setfill('0') << std::hex << m_sheaders.at(i).sh_entsize;
            std::cout << std::setfill(' ') << std::right;
            std::cout << "  " << std::left << std::setw(2) << SHFlags_to_cstring(m_sheaders.at(i).sh_flags);
            std::cout << std::right << std::setw(8) << std::dec << m_sheaders.at(i).sh_link;
            std::cout << std::setw(6) << std::dec << m_sheaders.at(i).sh_info;
            std::cout << std::setw(6) << std::dec << m_sheaders.at(i).sh_addralign << '\n';
        }
        
        std::cout << "Key to Flags:\n";
        std::cout << "  W (write), A (alloc), X (execute), M (merge), S (strings), I (info),\n";
        std::cout << "  L (link order), O (extra OS processing required), G (group), T (TLS),\n";
        std::cout << "  C (compressed), x (unknown), o (OS specific), E (exclude),\n";
        std::cout << "  D (mbind), l (large), p (processor specific)\n";

	}

    std::uint32_t ElfParser::get_section_name_offset(const std::vector<ElfSectionHeader>& sh_vec, std::uint32_t offset) const {
        std::uint32_t total_offset = sh_vec.at(m_header.e_shstrndx).sh_offset + offset;
        if (total_offset >= m_binReader.size())
            throw std::runtime_error("Error: the offset of a section name is larger than the file size");
        return total_offset;
    }

    std::string ElfParser::get_section_name(const std::vector<ElfSectionHeader>& sh_vec, std::uint32_t sh_name_offset, bool cut) {
        auto old_offset = m_binReader.tell();
        auto offset = get_section_name_offset(sh_vec, sh_name_offset);
        m_binReader.seek(offset);
        std::string name;
        if (!m_binReader.read_string(name))
            throw std::runtime_error("Error reading section name");
        m_binReader.seek(old_offset);
        if (name.length() > 17 && cut) {
            name = name.substr(0,12) + "[...]";
        }
        return name;
    }

    void ElfParser::parse_program_headers() {
        if (m_pheader_parsed)
            return;
        m_pheaders.resize(m_header.e_phnum);
        m_binReader.seek(m_header.e_phoff);

        for (ElfProgramHeader& ph : m_pheaders) {
            parse_program_header(ph);
        }
        
        m_pheader_parsed = true;
    }

    void ElfParser::parse_program_header(ElfProgramHeader& p_header) {
        if (!m_binReader.read(p_header.p_type))
            throw std::runtime_error("Error reading program header p_type");
        if (m_header.e_class == ElfClass::Elf32) {
            std::uint32_t tmp;
            if (!m_binReader.read(tmp))
                throw std::runtime_error("Error reading program header p_offset");
            p_header.p_offset = tmp;
            if (!m_binReader.read(tmp))
                throw std::runtime_error("Error reading program header p_vaddr");
            p_header.p_vaddr = tmp;
            if (!m_binReader.read(tmp))
                throw std::runtime_error("Error reading program header p_paddr");
            p_header.p_paddr = tmp;
            if (!m_binReader.read(tmp))
                throw std::runtime_error("Error reading program header p_filesz");
            p_header.p_filesz = tmp;
            if (!m_binReader.read(tmp))
                throw std::runtime_error("Error reading program header p_memsz");
            p_header.p_memsz = tmp;
            if (!m_binReader.read(tmp))
                throw std::runtime_error("Error reading program header p_flags");
            p_header.p_flags = tmp;
            if (!m_binReader.read(tmp))
                throw std::runtime_error("Error reading program header p_align");
            p_header.p_align = tmp;
        } else {
            if (!m_binReader.read(p_header.p_flags))
                throw std::runtime_error("Error reading program header p_flags");
            if (!m_binReader.read(p_header.p_offset))
                throw std::runtime_error("Error reading program header p_offset");
            if (!m_binReader.read(p_header.p_vaddr))
                throw std::runtime_error("Error reading program header p_vaddr");
            if (!m_binReader.read(p_header.p_paddr))
                throw std::runtime_error("Error reading program header p_paddr");
            if (!m_binReader.read(p_header.p_filesz))
                throw std::runtime_error("Error reading program header p_filesz");
            if (!m_binReader.read(p_header.p_memsz))
                throw std::runtime_error("Error reading program header p_memsz");
            if (!m_binReader.read(p_header.p_align))
                throw std::runtime_error("Error reading program header p_align");
        }
    } 

    void ElfParser::print_program_headers() {
        std::cout   << "\nElf file type is " << type_name(m_header.e_type) << '\n';
        std::cout   << "Entry point 0x" << std::hex << m_header.e_entry << '\n';
        std::cout   << "There are " << std::dec << m_header.e_phnum 
                    << " program headers, starting at offset " << m_header.e_phoff << "\n\n";
        std::cout   << "Program Headers:\n";
        std::cout   << "  Type           Offset             VirtAddr           PhysAddr\n";
        std::cout   << "                 FileSiz            MemSiz              Flags  Align\n";

        for (const auto& ph : m_pheaders) {
            std::cout   << "  " << std::setw(15) << std::left << PHtype_to_string(ph.p_type);
            std::cout   << std::right << std::setfill('0') << std::hex;
            std::cout   << "0x" << std::setw(16) << ph.p_offset << ' ';
            std::cout   << "0x" << std::setw(16) << ph.p_vaddr  << ' ';
            std::cout   << "0x" << std::setw(16) << ph.p_paddr  << '\n';

            std::cout   << std::setfill(' ') << std::setw(17) << " " << std::setfill('0');
            std::cout   << "0x" << std::setw(16) << ph.p_filesz << ' ';
            std::cout   << "0x" << std::setw(16) << ph.p_memsz << ' ';
            std::cout   << std::setfill(' ');
            std::cout   << ' ' << std::setw(6) << std::left <<  PHflags_to_string(ph.p_flags);
            std::cout   << "0x" << ph.p_align << '\n';
        }
    }

    std::vector<std::vector<std::string>>
    ElfParser::map_sections_to_segments() {
        std::vector<std::vector<std::string>> result;
        result.resize(m_pheaders.size());

        for (std::uint32_t i = 0; i < m_pheaders.size(); ++i) {
            const auto& ph = m_pheaders[i];

            for (const auto& sh : m_sheaders) {

                auto sec_start = sh.sh_offset;
                auto sec_end   = sh.sh_offset + sh.sh_size;

                auto seg_start = ph.p_offset;

                auto seg_end = ph.p_offset + ph.p_filesz;

                // Special case for NOBITS (.bss)
                if (sh.sh_type == static_cast<uint32_t>(SHType::Nobits)) {
                    seg_end = ph.p_offset + ph.p_memsz;
                }

                if (sec_start >= seg_start && sec_end <= seg_end) {
                    result[i].push_back(get_section_name(m_sheaders, sh.sh_name));
                }
            }
        }

        return result;
    }


    void ElfParser::print_section_to_segment_mapping(const std::vector<std::vector<std::string>>& map) const {
        std::cout << std::dec;
        std::cout << "\n Section to Segment mapping:\n";
        std::cout << "  Segment Sections...\n";

        for (std::uint32_t i = 0; i < map.size(); ++i) {
            std::cout << "   " << std::setw(2) << std::setfill('0') << std::right << i << "     ";

            if (map.at(i).size() > 0) {
                std::cout << map.at(i).at(0);
                for (std::size_t j = 1; j < map.at(i).size(); ++j)
                    std::cout << " " << map.at(i).at(j);
            }
            std::cout << '\n';
        }
    }
}