#include "rewind/common/version.hpp"

#include <iostream>
#include <string_view>

namespace {

void print_version() {
    std::cout << rewindfs::common::get_version_banner() << '\n';
}

void print_help() {
    std::cout << rewindfs::common::get_version_banner() << "\n\n"
              << "Usage: rewind [options] [command] [args...]\n\n"
              << "Options:\n"
              << "  -v, --version       Display version and platform information\n"
              << "  -h, --help          Display this help message\n\n"
              << "Planned Commands:\n"
              << "  init                Initialize a local recovery repository\n"
              << "  root                Manage monitored directory roots (add, list, remove)\n"
              << "  snapshot            Trigger or inspect snapshots\n"
              << "  timeline            Browse chronological history of files and directories\n"
              << "  diff                Compare file revisions\n"
              << "  restore             Safely restore earlier versions of files\n"
              << "  status              Check background observation and storage health\n"
              << "  verify              Verify repository integrity and detect bit-rot\n"
              << "  gc                  Prune unreferenced objects and enforce retention\n\n"
              << "For more information, see https://github.com/Swar-16/Rewind\n";
}

} // namespace

int main(int argc, char* argv[]) {
    if (argc <= 1) {
        print_help();
        return 0;
    }

    const std::string_view first_arg = argv[1];

    if (first_arg == "--version" || first_arg == "-v") {
        print_version();
        return 0;
    }

    if (first_arg == "--help" || first_arg == "-h") {
        print_help();
        return 0;
    }

    if (first_arg.starts_with('-')) {
        std::cerr << "rewind: unrecognized option '" << first_arg << "'\n"
                  << "Try 'rewind --help' for more information.\n";
        return 1;
    }

    std::cerr << "rewind: command '" << first_arg
              << "' is scheduled for subsequent implementation steps.\n"
              << "Try 'rewind --help' for available options.\n";
    return 1;
}
