#include <debug.hh>

void __panic(const char* file, int line, const char* format, ...) {
  fprintf(stderr, "panic at %s:%d", file, line);
  va_list args;
  va_start(args, format);
  vfprintf(stderr, format, args);
  va_end(args);
  fprintf(stderr, "\n");
  exit(1);
}
