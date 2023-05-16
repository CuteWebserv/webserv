#ifndef Handler_HPP
#define Handler_HPP

#include <map>
#include <string>

#include "Parser.hpp"

// response 예시
// HTTP/1.0 200 OK
// Content-type: image/gif
// Content-length: 8572
//
// dfsfsafsdfx3241uyi4dsf32423
// dfsfsafsdfx3241uyi4dsf32423
// dfsfsafsdfx3241uyi4dsf32423

struct Response
{
  std::string http_version;
  StatusCode status;
  std::string reason_phrase;
  std::map<std::string, std::string> headers;
  std::vector<char> body;
};

class Handler
{
 public:
  // Canonical Form

  // Handler를 다룰 때 Request구조체의 정보가 필요함
  // 이것을 생성자에서 어떤 식으로 넘겨줄지 고민
  Handler(void);
  Handler(const Handler& src);
  virtual ~Handler(void);
  Handler& operator=(Handler const& rhs);

  // Member Functions
  char* makeResponseMessage(void);

 private:
  // Member Variables
  struct Request m_request_data;
  struct Response m_response_data;

  // Member Functions
  void makeStartLine(void);
  void makeHeaders(void);
  void makeBody(void);
};

#endif
