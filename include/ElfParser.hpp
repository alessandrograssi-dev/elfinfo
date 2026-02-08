#pragma once
#include "BinaryReader.hpp"
#include "ElfHeader.hpp"
#include <string>

namespace elf {
  class ElfParser {
  public:
      explicit ElfParser(const std::string& path);

      void read_headers();
      void read_sections();

  private:
      BinaryReader m_binReader;

      void parse_e_ident(ElfHeader& header);
      void parse_header_body(ElfHeader& header);
      void print_header(const ElfHeader& header) const;
  };
}