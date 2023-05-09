#include "utils.hpp"

size_t ft_strlen(const char *str) {
  size_t len;

  len = 0;
  if (str == NULL) {
    return (0);
  }
  while (str[len] != '\0') {
    len += 1;
  }
  return (len);
}

char* ft_strjoin(char const* s1, char const* s2) {
  size_t s1_len;
  size_t s2_len;
  char *result;

  if (s1 == NULL && s2 == NULL) {
    return (NULL);
  }
  s1_len = ft_strlen(s1);
  s2_len = ft_strlen(s2);
  result = new char[s1_len + s2_len + 1]();
  if (result == NULL) {
    return (NULL);
  }
  memmove(result, s1, s1_len);
  memmove(result + s1_len, s2, s2_len + 1);
  return (result);
}
