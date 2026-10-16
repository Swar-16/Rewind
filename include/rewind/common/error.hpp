#pragma once

#include <cstdint>
#include <filesystem>
#include <iosfwd>
#include <string>
#include <string_view>

namespace rewindfs::common {

enum class ErrorCode : uint8_t {
    InvalidArgument = 1,
    FileNotFound,
    PathNotFound,
    AlreadyExists,
    PermissionDenied,
    DiskFull,
    DatabaseError,
    MigrationFailed,
    HashMismatch,
    CorruptedObject,
    ObjectNotFound,
    InvalidState,
    IOError,
    NotImplemented,
    InternalError
};

[[nodiscard]] std::string_view error_code_to_string(ErrorCode code) noexcept;

class RewindError {
public:
    RewindError(ErrorCode code, std::string message, std::filesystem::path context_path = {});

    [[nodiscard]] ErrorCode code() const noexcept { return m_code; }
    [[nodiscard]] std::string_view code_name() const noexcept;
    [[nodiscard]] const std::string& message() const noexcept { return m_message; }
    [[nodiscard]] const std::filesystem::path& context_path() const noexcept {
        return m_context_path;
    }
    [[nodiscard]] std::string to_string() const;

    [[nodiscard]] static RewindError from_errno(int err_num, std::string_view context_message,
                                                std::filesystem::path path = {});
    [[nodiscard]] static RewindError invalid_argument(std::string message,
                                                      std::filesystem::path context_path = {});
    [[nodiscard]] static RewindError io_error(std::string message,
                                              std::filesystem::path context_path = {});
    [[nodiscard]] static RewindError database_error(std::string message,
                                                    std::filesystem::path context_path = {});

    bool operator==(const RewindError& other) const noexcept = default;

private:
    ErrorCode m_code{ErrorCode::InternalError};
    std::string m_message{};
    std::filesystem::path m_context_path{};
};

std::ostream& operator<<(std::ostream& stream, const RewindError& error);

} // namespace rewindfs::common
