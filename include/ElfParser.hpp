#pragma once
#include "BinaryReader.hpp"
#include "ElfHeader.hpp"
#include "ElfSectionHeader.hpp"
#include <string>
#include <vector>

namespace elf {
	class ElfParser {
	public:
		explicit ElfParser(const std::string& path);

		void read_header();
		void read_section_headers();
		void read_sections();

	private:
		BinaryReader m_binReader;
		ElfHeader m_header {};
		bool m_header_parsed {false};		

		void parse_header();
		std::vector<ElfSectionHeader> parse_section_headers();

		void parse_e_ident(ElfHeader& header);
		void parse_header_body(ElfHeader& header);
		void parse_section_header(ElfSectionHeader& s_header);
		
		void print_header(const ElfHeader& header);
		void print_section_headers(const std::vector<ElfSectionHeader>& s_headers);

		std::uint32_t get_section_name_offset(const std::vector<ElfSectionHeader>& sh_vec, std::uint32_t offset) const;
		std::string get_section_name(const std::vector<ElfSectionHeader>& sh_vec, std::uint32_t sh_name_offset);
	};
}