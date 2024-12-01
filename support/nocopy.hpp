#pragma once

struct NonCopyable
{
    NonCopyable() = default;
    NonCopyable(const NonCopyable&) noexcept = delete;
    NonCopyable& operator=(const NonCopyable&) const noexcept = delete;
    virtual ~NonCopyable() {}
};