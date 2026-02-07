#include <iostream>
#include "CliOptions.hpp"

int main(int argc, char* argv[]) {
    try {
        CliOptions opts = CliOptions::parse_arguments(argc, argv);

        std::cout << "Inspecting file: " << opts.filename << "\n";
        // TODO: ELF parsing logic

    } catch (const std::exception& e) {
        std::cerr << e.what() << "\n";
        return 1;
    }

    return 0;
}
