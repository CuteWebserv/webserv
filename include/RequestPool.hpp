#ifndef REQUEST_POOL_HPP
#define REQUEST_POOL_HPP

#include <string>

struct RequestPool
{
  char* total_line;
  size_t line_len;
  size_t prev_offset;
  size_t offset;

  // Default Constructor
  RequestPool(void) : total_line(NULL) {}

  // Destructor
  ~RequestPool(void)
  {
    if (total_line != NULL)
    {
      delete total_line;
    }
  }

  // Copy constructor
  RequestPool(const RequestPool& src)
  {
    if (this != &src)
    {
      *this = src;
    }
  }

  // Copy assignment
  RequestPool& operator=(const RequestPool& rhs)
  {
    if (this != &rhs)
    {
      line_len = rhs.line_len;
      prev_offset = rhs.prev_offset;
      offset = rhs.offset;
      if (total_line != NULL)
      {
        delete total_line;
      }
      total_line = new char[line_len];
      std::memmove(total_line, rhs.total_line, line_len + 1);
    }
    return (*this);
  }
};

#endif