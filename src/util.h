/* 
 * File:   util.h
 * Author: montana
 *
 * Created on September 11, 2010, 6:48 PM
 */

#ifndef UTIL_H
#define	UTIL_H

#include <string>
#include <vector>
#include <iostream>
#include <fstream>

namespace pw {

// This is a utility class that parses strings.
class string_util {
public:
  // Tokenizes a string s into tokens. Tokens are delimited by any of the
  // characters in delimiters. Blank tokens are omitted.
  static void tokenize(const std::string& s, const std::string& delimiters, std::vector<std::string>& tokens);

  // A more convenient way of calling the tokenize() method.
  static std::vector<std::string> tokenize(const std::string& s, const std::string& delimiters = std::string(" "));
};

}
#endif	/* UTIL_H */
