#include "util.h"

void pw::string_util::tokenize(const std::string& s, const std::string& delimiters, std::vector<std::string>& tokens) {
  std::string::size_type lastPos = s.find_first_not_of(delimiters, 0);
  std::string::size_type pos = s.find_first_of(delimiters, lastPos);
  while (std::string::npos != pos || std::string::npos != lastPos) {
    tokens.push_back(s.substr(lastPos, pos - lastPos));
    lastPos = s.find_first_not_of(delimiters, pos);
    pos = s.find_first_of(delimiters, lastPos);
  }
}

std::vector<std::string> pw::string_util::tokenize(const std::string& s, const std::string& delimiters) {
  std::vector<std::string> tokens;
  tokenize(s, delimiters, tokens);
  return tokens;
}

