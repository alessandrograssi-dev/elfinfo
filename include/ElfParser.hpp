#pragma once
#include "BinaryReader.hpp"
#include <string>

namespace elf {
  class ElfParser {
  public:
      explicit ElfParser(const std::string& path);

      void read_headers();
      void read_sections();

  private:
      BinaryReader m_binReader;
  };
}