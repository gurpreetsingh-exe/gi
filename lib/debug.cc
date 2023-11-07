#include <debug.hh>
#include <fmt/ranges.h>

#ifdef __linux__
#include <execinfo.h>
#endif

static auto print_backtrace() -> void {
#ifdef __linux__
  static void* trace[128];
  int n = backtrace(trace, 128);

  std::vector<std::string> trace_strs;
  trace_strs.reserve(usize(n));
  for (int i = 0; i < n; i++) {
    trace_strs.emplace_back(fmt::format("{:p}", trace[i]));
  }

  std::string sym = std::getenv("SYMBOLIZER_PATH") ?: "";
  if (sym.empty()) {
    sym = "llvm-symbolizer";
  }

  auto cmd = fmt::format("{} {} -e {} -s -p -C -i --color --output-style=GNU | "
                         "awk '{{ print \"#\" NR, $0 }}'",
                         sym, fmt::join(trace_strs, " "),
                         fs::canonical("/proc/self/exe").native());
  std::system(cmd.c_str());
#endif
}

void __panic(const char* file, int line, const char* format, ...) {
  fprintf(stderr, "panic at %s:%d", file, line);
  va_list args;
  va_start(args, format);
  vfprintf(stderr, format, args);
  va_end(args);
  fprintf(stderr, "\n");
  print_backtrace();
  exit(1);
}
