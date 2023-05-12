#ifndef utils_H
#define utils_H

#include <algorithm>
#include <cctype>
#include <cstring>
#include <sstream>
#include <string>
#include <vector>

// 소문자 변환 펑터 객체
struct tolower_functor {
  char operator()(char c) const {
    return std::tolower(c);
  }
};

char* ft_strjoin(char const* s1, char const* s2);
size_t ft_strlen(const char* str);
char* ft_strdup(const char* src);
std::string ft_strtrim(const std::string& str);
std::vector<std::string> ft_split(const std::string& str, char delimiter,
                                  int count);
std::string ft_toLower(const std::string& str);

#endif