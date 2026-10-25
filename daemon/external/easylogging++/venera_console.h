// Copyright (c) 2025-2026 Venera Labs
#pragma once

#include <iostream>
#include <limits>
#include <string>
#include <windows.h>

namespace venera {
inline void write_console_utf8(const std::string& text) {
  HANDLE output = GetStdHandle(STD_OUTPUT_HANDLE);
  DWORD mode = 0;
  if (output == nullptr || output == INVALID_HANDLE_VALUE || !GetConsoleMode(output, &mode)) {
    std::cout.write(text.data(), static_cast<std::streamsize>(text.size()));
    return;
  }
  if (text.empty()) return;
  if (text.size() > static_cast<std::size_t>((std::numeric_limits<int>::max)())) {
    std::cout.setstate(std::ios::badbit);
    return;
  }
  const int bytes = static_cast<int>(text.size());
  const int length = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, text.data(), bytes, nullptr, 0);
  if (length <= 0) {
    std::cout.setstate(std::ios::badbit);
    return;
  }
  std::wstring converted(static_cast<std::size_t>(length), L'\0');
  if (MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, text.data(), bytes, &converted[0], length) != length) {
    std::cout.setstate(std::ios::badbit);
    return;
  }
  std::wstring console_text;
  console_text.reserve(converted.size());
  wchar_t previous = 0;
  for (wchar_t ch : converted) {
    if (ch == L'\n' && previous != L'\r') console_text.push_back(L'\r');
    console_text.push_back(ch);
    previous = ch;
  }
  std::cout.flush();
  std::size_t position = 0;
  while (position < console_text.size()) {
    const std::size_t remaining = console_text.size() - position;
    DWORD count = static_cast<DWORD>(remaining > 16384 ? 16384 : remaining);
    const wchar_t last = console_text[position + count - 1];
    if (count < remaining && last >= 0xd800 && last <= 0xdbff) --count;
    DWORD written = 0;
    if (!WriteConsoleW(output, console_text.data() + position, count, &written, nullptr) ||
        written == 0 || written > count) {
      std::cout.setstate(std::ios::badbit);
      return;
    }
    position += written;
  }
}
}
