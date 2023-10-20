#include <fstream>
#include <utils.hh>

auto read_file(const fs::path& path) -> std::string {
  FILE* f = std::fopen(path.c_str(), "r");

  if (not f) {
    eprint("cannot open \"%s\"\n  %s\n", path.c_str(), strerror(errno));
  }

  if (std::fseek(f, 0L, SEEK_END)) {
    eprint("%s\n", strerror(errno));
  }

  auto file_size = ftell(f);
  if (file_size < 0) {
    eprint("cannot get file size\n  %s\n", strerror(errno));
  }

  rewind(f);

  auto size = static_cast<usize>(file_size);
  auto content = std::string();
  content.resize(size);
  auto read_bytes = std::fread(content.data(), 1, size, f);

  if (read_bytes != size) {
    int e = feof(f);
    if (e) {
      eprint("unexpected end of file\n");
    } else {
      e = ferror(f);
      eprint("cannot read file\n  %s\n", strerror(e));
    }
  }

  fclose(f);

  return content;
}
