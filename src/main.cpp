#include <iostream>
#include "CliOptions.hpp"
#include "ElfParser.hpp"

using elf::ElfParser;

int main(int argc, char* argv[]) {
    try {
        CliOptions opts = CliOptions::parse_arguments(argc, argv);

        ElfParser parser(opts.filename);
        if (opts.show_headers) {
            parser.read_header();
        }
        if (opts.show_section_headers) {
            parser.read_section_headers();
        }
        if (opts.show_sections) {
            parser.read_sections();
        }
        
    } catch (const std::exception& e) {
        std::cerr << e.what() << "\n";
        return 1;
    }

    return 0;
}
