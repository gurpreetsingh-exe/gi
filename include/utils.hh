#ifndef UTILS_H
#define UTILS_H

#include <cassert>
#include <cerrno>
#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <filesystem>
#include <fmt/core.h>
#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

using u8 = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;
using i8 = int8_t;
using i16 = int16_t;
using i32 = int32_t;
using i64 = int64_t;
using usize = size_t;
using isize = long int;
using f32 = float;
using f64 = double;

namespace fs = std::filesystem;
namespace chr = std::chrono;

#define eprint(...)                                                            \
  {                                                                            \
    std::fprintf(stderr, __VA_ARGS__);                                         \
    std::exit(1);                                                              \
  }

auto read_file(const fs::path& path) -> std::string;

#endif // !UTILS_H
