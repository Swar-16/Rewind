#pragma once

#include <cstdint>
#include <iosfwd>
#include <string>
#include <string_view>

namespace rewindfs::common {

enum class ErrorCode : uint8_t {
    Success = 0,
    InvalidArgument,
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
    RewindError() noexcept = default;
    RewindError(ErrorCode code, std::string message, std::string context_path = "");

    [[nodiscard]] ErrorCode code() const noexcept { return m_code; }
    [[nodiscard]] std::string_view code_name() const noexcept;
    [[nodiscard]] const std::string& message() const noexcept { return m_message; }
    [[nodiscard]] const std::string& context_path() const noexcept { return m_context_path; }
    [[nodiscard]] std::string to_string() const;

    [[nodiscard]] bool is_ok() const noexcept { return m_code == ErrorCode::Success; }

    [[nodiscard]] static RewindError success() noexcept;
    // NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
    [[nodiscard]] static RewindError from_errno(int err_num, std::string_view context_message,
                                                std::string_view path = "");
    [[nodiscard]] static RewindError invalid_argument(std::string message,
                                                      std::string context_path = "");
    [[nodiscard]] static RewindError io_error(std::string message, std::string context_path = "");
    [[nodiscard]] static RewindError database_error(std::string message,
                                                    std::string context_path = "");

    bool operator==(const RewindError& other) const noexcept;
    bool operator!=(const RewindError& other) const noexcept { return !(*this == other); }

private:
    ErrorCode m_code{ErrorCode::Success};
    std::string m_message;
    std::string m_context_path;
};

std::ostream& operator<<(std::ostream& stream, const RewindError& error);

} // namespace rewindfs::common
