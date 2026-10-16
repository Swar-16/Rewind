#pragma once

#include "rewind/common/error.hpp"

#include <functional>
#include <optional>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <variant>

namespace rewindfs::common {

template <typename T> class [[nodiscard]] Result {
public:
    using value_type = T;
    using error_type = RewindError;

    template <typename U = T>
        requires(!std::is_same_v<std::decay_t<U>, Result<T>> &&
                 !std::is_same_v<std::decay_t<U>, RewindError> && std::is_constructible_v<T, U>)
    Result(U&& val) : m_data(std::in_place_index<0>, std::forward<U>(val)) {}

    Result(RewindError err) noexcept : m_data(std::in_place_index<1>, std::move(err)) {}

    Result(const Result&) = default;
    Result(Result&&) noexcept = default;
    Result& operator=(const Result&) = default;
    Result& operator=(Result&&) noexcept = default;
    ~Result() = default;

    template <typename... Args>
        requires std::is_constructible_v<T, Args...>
    [[nodiscard]] static Result ok(Args&&... args) {
        return Result(T(std::forward<Args>(args)...));
    }

    [[nodiscard]] static Result err(RewindError error) { return Result(std::move(error)); }

    [[nodiscard]] bool is_ok() const noexcept { return m_data.index() == 0; }

    [[nodiscard]] bool is_err() const noexcept { return m_data.index() == 1; }

    explicit operator bool() const noexcept { return is_ok(); }

    [[nodiscard]] const T& value() const& {
        if (is_err()) {
            throw std::logic_error(error().to_string());
        }
        return std::get<0>(m_data);
    }

    [[nodiscard]] T& value() & {
        if (is_err()) {
            throw std::logic_error(error().to_string());
        }
        return std::get<0>(m_data);
    }

    [[nodiscard]] T&& value() && {
        if (is_err()) {
            throw std::logic_error(error().to_string());
        }
        return std::get<0>(std::move(m_data));
    }

    [[nodiscard]] const RewindError& error() const& {
        if (is_ok()) {
            throw std::logic_error("Attempted to access error of successful Result");
        }
        return std::get<1>(m_data);
    }

    [[nodiscard]] RewindError&& error() && {
        if (is_ok()) {
            throw std::logic_error("Attempted to access error of successful Result");
        }
        return std::get<1>(std::move(m_data));
    }

    template <typename U> [[nodiscard]] T value_or(U&& default_value) const& {
        if (is_ok()) {
            return std::get<0>(m_data);
        }
        return static_cast<T>(std::forward<U>(default_value));
    }

    template <typename Func> auto and_then(Func&& func) const& {
        using ReturnType = std::invoke_result_t<Func, const T&>;
        if (is_err()) {
            return ReturnType::err(error());
        }
        return std::invoke(std::forward<Func>(func), value());
    }

    template <typename Func> auto and_then(Func&& func) && {
        using ReturnType = std::invoke_result_t<Func, T&&>;
        if (is_err()) {
            return ReturnType::err(std::move(*this).error());
        }
        return std::invoke(std::forward<Func>(func), std::move(*this).value());
    }

    template <typename Func> auto transform(Func&& func) const& {
        using TransformedType = std::invoke_result_t<Func, const T&>;
        if (is_err()) {
            return Result<TransformedType>::err(error());
        }
        return Result<TransformedType>::ok(std::invoke(std::forward<Func>(func), value()));
    }

    template <typename Func> auto transform(Func&& func) && {
        using TransformedType = std::invoke_result_t<Func, T&&>;
        if (is_err()) {
            return Result<TransformedType>::err(std::move(*this).error());
        }
        return Result<TransformedType>::ok(
            std::invoke(std::forward<Func>(func), std::move(*this).value()));
    }

private:
    std::variant<T, RewindError> m_data;
};

template <> class [[nodiscard]] Result<void> {
public:
    using value_type = void;
    using error_type = RewindError;

    Result() noexcept : m_error(std::nullopt) {}
    Result(RewindError err) noexcept : m_error(std::move(err)) {}

    Result(const Result&) = default;
    Result(Result&&) noexcept = default;
    Result& operator=(const Result&) = default;
    Result& operator=(Result&&) noexcept = default;
    ~Result() = default;

    [[nodiscard]] static Result ok() noexcept { return Result(); }

    [[nodiscard]] static Result err(RewindError error) { return Result(std::move(error)); }

    [[nodiscard]] bool is_ok() const noexcept { return !m_error.has_value(); }

    [[nodiscard]] bool is_err() const noexcept { return m_error.has_value(); }

    explicit operator bool() const noexcept { return is_ok(); }

    void value() const {
        if (is_err()) {
            throw std::logic_error(error().to_string());
        }
    }

    [[nodiscard]] const RewindError& error() const& {
        if (is_ok()) {
            throw std::logic_error("Attempted to access error of successful Result<void>");
        }
        return *m_error;
    }

    [[nodiscard]] RewindError&& error() && {
        if (is_ok()) {
            throw std::logic_error("Attempted to access error of successful Result<void>");
        }
        return std::move(*m_error);
    }

    template <typename Func> auto and_then(Func&& func) const& {
        using ReturnType = std::invoke_result_t<Func>;
        if (is_err()) {
            return ReturnType::err(error());
        }
        return std::invoke(std::forward<Func>(func));
    }

private:
    std::optional<RewindError> m_error{std::nullopt};
};

} // namespace rewindfs::common
