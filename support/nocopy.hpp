#pragma once

struct NonCopyable
{
    NonCopyable() = default;
    NonCopyable(const NonCopyable&) noexcept = delete;
    NonCopyable& operator=(const NonCopyable&) noexcept = delete;
    NonCopyable(NonCopyable&&) noexcept = default;
    NonCopyable& operator=(NonCopyable&&) noexcept = default;
    virtual ~NonCopyable() {}
};