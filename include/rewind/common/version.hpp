#pragma once

#include <string>
#include <string_view>

namespace rewindfs::common {

constexpr unsigned int VERSION_MAJOR = 0;
constexpr unsigned int VERSION_MINOR = 1;
constexpr unsigned int VERSION_PATCH = 0;

constexpr std::string_view VERSION_STRING = "0.1.0";
constexpr std::string_view PRODUCT_NAME = "Rewind";
constexpr std::string_view PRODUCT_DESCRIPTION = "Local-first transparent file recovery system";

[[nodiscard]] inline std::string get_version_banner() {
    std::string banner;
    banner.reserve(128);
    banner.append(PRODUCT_NAME);
    banner.append(" v");
    banner.append(VERSION_STRING);
    banner.append(" — ");
    banner.append(PRODUCT_DESCRIPTION);
    banner.append(" (Linux x86_64, C++20)");
    return banner;
}

} // namespace rewindfs::common
