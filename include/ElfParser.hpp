#pragma once
#include "BinaryReader.hpp"
#include "ElfHeader.hpp"
#include "ElfSectionHeader.hpp"
#include "ElfProgramHeader.hpp"
#include <string>
#include <vector>
#include <map>

namespace elf {
	class ElfParser {
	public:
		explicit ElfParser(const std::string& path);

		void read_header();
		void read_section_headers();
		void read_sections();
		void read_program_headers();

	private:
		BinaryReader m_binReader;
		ElfHeader m_header {};
		std::vector<ElfSectionHeader> m_sheaders;
		std::vector<ElfProgramHeader> m_pheaders;

		bool m_header_parsed {false};
		bool m_sheader_parsed {false};
		bool m_pheader_parsed {false};

		void parse_header();
		void parse_section_headers();
		void parse_program_headers();

		void parse_e_ident(ElfHeader& header);
		void parse_header_body(ElfHeader& header);
		void parse_section_header(ElfSectionHeader& s_header);
		void parse_program_header(ElfProgramHeader& p_header);

		std::vector<std::vector<std::string>> map_sections_to_segments();
		
		void print_header();
		void print_section_headers();
		void print_program_headers();
		void print_section_to_segment_mapping(const std::vector<std::vector<std::string>>& map) const;

		std::uint32_t get_section_name_offset(const std::vector<ElfSectionHeader>& sh_vec, std::uint32_t offset) const;
		std::string get_section_name(const std::vector<ElfSectionHeader>& sh_vec, std::uint32_t sh_name_offset, bool cut = false);
	};
}