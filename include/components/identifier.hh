#ifndef IDENTIFIER_H
#define IDENTIFIER_H

#include <utils.hh>

struct IdentifierComponent {
  char identifier[64] = {};
  IdentifierComponent(const std::string& name) {
    auto sz = name.size() >= 64L ? 64L : name.size();
    std::memcpy(&identifier, name.c_str(), sz);
  }
};

#endif // !IDENTIFIER_H
