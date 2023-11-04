#ifndef DEBUG_H
#define DEBUG_H

#include <utils.hh>

#define PANIC(...) (::__panic(__FILE__, __LINE__, "    " __VA_ARGS__))

#define ASSERT(cond, ...)                                                      \
  (((cond) ? static_cast<void>(0)                                              \
           : PANIC("assertion failed \"" #cond "\"\n    " __VA_ARGS__)))

[[noreturn]] [[gnu::format(printf, 3, 4)]]
void __panic(const char* file, int line, const char* format, ...);

#endif // !DEBUG_H
