#include "rewind/common/error.hpp"

#include <cerrno>
#include <cstring>
#include <ostream>
#include <utility>

namespace rewindfs::common {

std::string_view error_code_to_string(ErrorCode code) noexcept {
  switch (code) {
  case ErrorCode::Success:
    return "Success";
  case ErrorCode::InvalidArgument:
    return "InvalidArgument";
  case ErrorCode::FileNotFound:
    return "FileNotFound";
  case ErrorCode::PathNotFound:
    return "PathNotFound";
  case ErrorCode::AlreadyExists:
    return "AlreadyExists";
  case ErrorCode::PermissionDenied:
    return "PermissionDenied";
  case ErrorCode::DiskFull:
    return "DiskFull";
  case ErrorCode::DatabaseError:
    return "DatabaseError";
  case ErrorCode::MigrationFailed:
    return "MigrationFailed";
  case ErrorCode::HashMismatch:
    return "HashMismatch";
  case ErrorCode::CorruptedObject:
    return "CorruptedObject";
  case ErrorCode::ObjectNotFound:
    return "ObjectNotFound";
  case ErrorCode::InvalidState:
    return "InvalidState";
  case ErrorCode::IOError:
    return "IOError";
  case ErrorCode::NotImplemented:
    return "NotImplemented";
  case ErrorCode::InternalError:
    return "InternalError";
  }
  return "UnknownError";
}

RewindError::RewindError(ErrorCode code, std::string message,
                         std::string context_path)
    : m_code(code), m_message(std::move(message)),
      m_context_path(std::move(context_path)) {}

std::string_view RewindError::code_name() const noexcept {
  return error_code_to_string(m_code);
}

std::string RewindError::to_string() const {
  if (m_code == ErrorCode::Success) {
    return "Success";
  }

  std::string result;
  result.reserve(64 + m_message.size() + m_context_path.size());
  result.push_back('[');
  result.append(code_name());
  result.append("] ");
  result.append(m_message);

  if (!m_context_path.empty()) {
    result.append(" (path: ");
    result.append(m_context_path);
    result.push_back(')');
  }

  return result;
}

RewindError RewindError::success() noexcept {
  return RewindError(ErrorCode::Success, "Operation succeeded");
}

RewindError RewindError::from_errno(int err_num,
                                    std::string_view context_message,
                                    std::string_view path) {
  ErrorCode code = ErrorCode::IOError;
  switch (err_num) {
  case 0:
    return success();
  case ENOENT:
    code = ErrorCode::FileNotFound;
    break;
  case EACCES:
  case EPERM:
    code = ErrorCode::PermissionDenied;
    break;
  case EEXIST:
    code = ErrorCode::AlreadyExists;
    break;
  case ENOSPC:
    code = ErrorCode::DiskFull;
    break;
  case EINVAL:
    code = ErrorCode::InvalidArgument;
    break;
  case ENOTDIR:
  case EISDIR:
    code = ErrorCode::PathNotFound;
    break;
  default:
    code = ErrorCode::IOError;
    break;
  }

  std::string detailed_message;
  if (!context_message.empty()) {
    detailed_message.append(context_message);
    detailed_message.append(": ");
  }
  detailed_message.append(std::strerror(err_num));

  return RewindError(code, std::move(detailed_message), std::string(path));
}

RewindError RewindError::invalid_argument(std::string message,
                                          std::string context_path) {
  return RewindError(ErrorCode::InvalidArgument, std::move(message),
                     std::move(context_path));
}

RewindError RewindError::io_error(std::string message,
                                  std::string context_path) {
  return RewindError(ErrorCode::IOError, std::move(message),
                     std::move(context_path));
}

RewindError RewindError::database_error(std::string message,
                                        std::string context_path) {
  return RewindError(ErrorCode::DatabaseError, std::move(message),
                     std::move(context_path));
}

bool RewindError::operator==(const RewindError &other) const noexcept {
  return m_code == other.m_code && m_message == other.m_message &&
         m_context_path == other.m_context_path;
}

std::ostream &operator<<(std::ostream &os, const RewindError &error) {
  os << error.to_string();
  return os;
}

} // namespace rewindfs::common
