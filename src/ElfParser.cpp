#include "ElfParser.hpp"

namespace elf {
  ElfParser::ElfParser(const std::string& path)
      : m_binReader(path)
  {}

  void ElfParser::read_headers() {
      // TODO: implement
  }

  void ElfParser::read_sections() {
      // TODO: implement
  }
}