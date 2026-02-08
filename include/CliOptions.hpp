#pragma once

#include <string>
#include <stdexcept>

struct CliOptions {
    std::string filename;
    bool show_headers = false;
    bool show_sections = false;

    static inline CliOptions parse_arguments(int argc, char* const argv[]);
};

inline CliOptions CliOptions::parse_arguments(int argc, char* const argv[]) {
    if (argc == 2) {
        std::string arg = argv[1];
        if (arg == "--help" || arg == "-h") {
            throw std::runtime_error("Usage: elfinfo <filename> [--headers] [--sections]");
        }
    }

    if (argc < 2) {
        throw std::runtime_error("Usage: elfinfo <filename> [--headers] [--sections]");
    }

    CliOptions opts;
    opts.filename = argv[1];

    for (int i = 2; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--headers" || arg == "-H") {
            opts.show_headers = true;
        } else if (arg == "--sections" || arg == "-S") {
            opts.show_sections = true;
        } else {
            throw std::runtime_error("Unknown option: " + arg);
        }
    }

    return opts;
}
