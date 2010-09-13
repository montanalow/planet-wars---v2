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

#include "config.h"

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

#ifdef LOGGING_ENABLED
class logger {
public:
  static std::ofstream log;
};
#else /* LOGGING_ENABLED */
class dead_end {
public:
  dead_end& operator<<(bool&){return *this;}
  dead_end& operator<<(short&){return *this;}
  dead_end& operator<<(unsigned short&){return *this;}
  dead_end& operator<<(int&){return *this;}
  dead_end& operator<<(unsigned int&){return *this;}
  dead_end& operator<<(long&){return *this;}
  dead_end& operator<<(unsigned long&){return *this;}
  dead_end& operator<<(float&){return *this;}
  dead_end& operator<<(double&){return *this;}
  dead_end& operator<<(long double&){return *this;}
  dead_end& operator<<(const void*){return *this;}
  dead_end& operator<<(std::streambuf*){return *this;}
  dead_end& operator<<(std::ostream&){return *this;}
  dead_end& operator<<(std::ios&){return *this;}
  dead_end& operator<<(std::ios_base&){return *this;}
  dead_end& operator<<(char){return *this;}
  dead_end& operator<<(signed char){return *this;}
  dead_end& operator<<(unsigned char){return *this;}
  dead_end& operator<<(const char*){return *this;}
  dead_end& operator<<(const signed char*){return *this;}
  dead_end& operator<<(const unsigned char*){return *this;}
  void flush(){}
};

class logger {
public:
  static dead_end log;
};
#endif /* LOGGING_ENABLED */

}
#endif	/* UTIL_H */
