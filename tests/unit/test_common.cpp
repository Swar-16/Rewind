#include "rewind/common/error.hpp"
#include "rewind/common/result.hpp"
#include "rewind/common/version.hpp"

#include <gtest/gtest.h>

#include <cerrno>
#include <string>
#include <utility>

using namespace rewindfs::common;

// =============================================================================
// 1. Version Tests
// =============================================================================

TEST(VersionTest, SemanticVersionConstants) {
  EXPECT_EQ(VERSION_MAJOR, 0u);
  EXPECT_EQ(VERSION_MINOR, 1u);
  EXPECT_EQ(VERSION_PATCH, 0u);
  EXPECT_EQ(VERSION_STRING, "0.1.0");
  EXPECT_EQ(PRODUCT_NAME, "Rewind");
}

TEST(VersionTest, VersionBannerFormat) {
  const std::string banner = get_version_banner();
  EXPECT_FALSE(banner.empty());
  EXPECT_NE(banner.find("Rewind v0.1.0"), std::string::npos);
  EXPECT_NE(banner.find("C++20"), std::string::npos);
}

// =============================================================================
// 2. Error Tests
// =============================================================================

TEST(ErrorTest, DefaultConstructorIsSuccess) {
  RewindError err;
  EXPECT_TRUE(err.is_ok());
  EXPECT_EQ(err.code(), ErrorCode::Success);
  EXPECT_EQ(err.code_name(), "Success");
  EXPECT_EQ(err.to_string(), "Success");
}

TEST(ErrorTest, ErrorCodeMapping) {
  EXPECT_EQ(error_code_to_string(ErrorCode::Success), "Success");
  EXPECT_EQ(error_code_to_string(ErrorCode::FileNotFound), "FileNotFound");
  EXPECT_EQ(error_code_to_string(ErrorCode::PermissionDenied),
            "PermissionDenied");
  EXPECT_EQ(error_code_to_string(ErrorCode::DiskFull), "DiskFull");
  EXPECT_EQ(error_code_to_string(ErrorCode::DatabaseError), "DatabaseError");
  EXPECT_EQ(error_code_to_string(ErrorCode::HashMismatch), "HashMismatch");
  EXPECT_EQ(error_code_to_string(ErrorCode::CorruptedObject),
            "CorruptedObject");
  EXPECT_EQ(error_code_to_string(ErrorCode::InternalError), "InternalError");
}

TEST(ErrorTest, CustomErrorFormattingWithContext) {
  RewindError err(ErrorCode::FileNotFound, "Missing snapshot manifest",
                  "/var/rewind/snap1");
  EXPECT_FALSE(err.is_ok());
  EXPECT_EQ(err.code(), ErrorCode::FileNotFound);
  EXPECT_EQ(err.code_name(), "FileNotFound");
  EXPECT_EQ(err.message(), "Missing snapshot manifest");
  EXPECT_EQ(err.context_path(), "/var/rewind/snap1");
  EXPECT_EQ(
      err.to_string(),
      "[FileNotFound] Missing snapshot manifest (path: /var/rewind/snap1)");
}

TEST(ErrorTest, FromErrnoTranslation) {
  auto err_not_found =
      RewindError::from_errno(ENOENT, "Failed opening path", "/missing/file");
  EXPECT_EQ(err_not_found.code(), ErrorCode::FileNotFound);
  EXPECT_FALSE(err_not_found.is_ok());
  EXPECT_NE(err_not_found.message().find("Failed opening path"),
            std::string::npos);

  auto err_permission = RewindError::from_errno(EACCES, "Access denied");
  EXPECT_EQ(err_permission.code(), ErrorCode::PermissionDenied);

  auto err_disk_full = RewindError::from_errno(ENOSPC, "Write failed");
  EXPECT_EQ(err_disk_full.code(), ErrorCode::DiskFull);

  auto err_success = RewindError::from_errno(0, "No error");
  EXPECT_TRUE(err_success.is_ok());
}

TEST(ErrorTest, EqualityOperators) {
  RewindError a(ErrorCode::IOError, "Disk read failed", "/dev/sda");
  RewindError b(ErrorCode::IOError, "Disk read failed", "/dev/sda");
  RewindError c(ErrorCode::IOError, "Different error", "/dev/sda");

  EXPECT_EQ(a, b);
  EXPECT_NE(a, c);
}

// =============================================================================
// 3. Result<T> Tests
// =============================================================================

TEST(ResultTest, SuccessStateAndValueAccess) {
  Result<int> res(42);
  EXPECT_TRUE(res.is_ok());
  EXPECT_FALSE(res.is_err());
  EXPECT_TRUE(static_cast<bool>(res));
  EXPECT_EQ(res.value(), 42);
  EXPECT_EQ(res.value_or(0), 42);
}

TEST(ResultTest, ErrorStateAndThrowOnAccess) {
  RewindError original_err(ErrorCode::DatabaseError, "Locked database");
  Result<int> res(original_err);

  EXPECT_FALSE(res.is_ok());
  EXPECT_TRUE(res.is_err());
  EXPECT_FALSE(static_cast<bool>(res));
  EXPECT_EQ(res.error().code(), ErrorCode::DatabaseError);
  EXPECT_EQ(res.value_or(99), 99);
  EXPECT_THROW(static_cast<void>(res.value()), std::logic_error);
}

TEST(ResultTest, StaticOkAndErrFactories) {
  auto res_ok = Result<std::string>::ok("RewindSnapshot");
  EXPECT_TRUE(res_ok.is_ok());
  EXPECT_EQ(res_ok.value(), "RewindSnapshot");

  auto res_err =
      Result<std::string>::err(RewindError::invalid_argument("Empty path"));
  EXPECT_TRUE(res_err.is_err());
  EXPECT_EQ(res_err.error().code(), ErrorCode::InvalidArgument);
}

TEST(ResultTest, MoveSemantics) {
  struct MoveOnly {
    std::string payload;
    explicit MoveOnly(std::string p) : payload(std::move(p)) {}
    MoveOnly(const MoveOnly &) = delete;
    MoveOnly &operator=(const MoveOnly &) = delete;
    MoveOnly(MoveOnly &&) noexcept = default;
    MoveOnly &operator=(MoveOnly &&) noexcept = default;
  };

  Result<MoveOnly> res = MoveOnly("test-payload");
  EXPECT_TRUE(res.is_ok());

  MoveOnly extracted = std::move(res).value();
  EXPECT_EQ(extracted.payload, "test-payload");
}

TEST(ResultTest, MonadicTransform) {
  Result<int> res(10);
  auto transformed = res.transform([](int v) { return v * 2; });
  EXPECT_TRUE(transformed.is_ok());
  EXPECT_EQ(transformed.value(), 20);

  Result<int> err_res = RewindError(ErrorCode::InternalError, "Failure");
  auto failed_transform = err_res.transform([](int v) { return v * 2; });
  EXPECT_TRUE(failed_transform.is_err());
  EXPECT_EQ(failed_transform.error().code(), ErrorCode::InternalError);
}

TEST(ResultTest, MonadicAndThen) {
  auto divide = [](int a, int b) -> Result<int> {
    if (b == 0) {
      return Result<int>::err(
          RewindError::invalid_argument("Division by zero"));
    }
    return Result<int>::ok(a / b);
  };

  Result<int> val(100);
  auto divided = val.and_then([&](int v) { return divide(v, 5); });
  EXPECT_TRUE(divided.is_ok());
  EXPECT_EQ(divided.value(), 20);

  auto div_zero = val.and_then([&](int v) { return divide(v, 0); });
  EXPECT_TRUE(div_zero.is_err());
  EXPECT_EQ(div_zero.error().code(), ErrorCode::InvalidArgument);
}

// =============================================================================
// 4. Result<void> Tests
// =============================================================================

TEST(ResultVoidTest, SuccessAndErrorStates) {
  Result<void> ok_res = Result<void>::ok();
  EXPECT_TRUE(ok_res.is_ok());
  EXPECT_FALSE(ok_res.is_err());
  EXPECT_NO_THROW(ok_res.value());

  Result<void> err_res =
      Result<void>::err(RewindError::io_error("Disk unmounted"));
  EXPECT_FALSE(err_res.is_ok());
  EXPECT_TRUE(err_res.is_err());
  EXPECT_EQ(err_res.error().code(), ErrorCode::IOError);
  EXPECT_THROW(err_res.value(), std::logic_error);
}

TEST(ResultVoidTest, MonadicAndThen) {
  bool side_effect_executed = false;
  Result<void> ok_res = Result<void>::ok();

  auto chained = ok_res.and_then([&]() -> Result<int> {
    side_effect_executed = true;
    return Result<int>::ok(777);
  });

  EXPECT_TRUE(side_effect_executed);
  EXPECT_TRUE(chained.is_ok());
  EXPECT_EQ(chained.value(), 777);
}
