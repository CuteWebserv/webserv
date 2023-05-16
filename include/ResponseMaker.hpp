#ifndef ResponseMaker_HPP
#define ResponseMaker_HPP

#include <map>
#include <string>

#include "Config.hpp"
#include "Parser.hpp"

// response 예시
// HTTP/1.0 200 OK
// Cotent-type: image/gif
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
  // header부분 만들 때, MIME를 어떻게 할 지고민
  std::map<std::string, std::string> headers;
  std::vector<char> body;
};

// 추상클래스로만들기
class MethodHandler
{
};

// PostMethodHandler, DeleteMethodHandler도 만들기
class GetMethodHandler : public MethodHandler
{
};

class ResponseMaker
{
 public:
  ResponseMaker(void);
  ResponseMaker(Request& reqeust_data, Config& config_data);
  ResponseMaker(const ResponseMaker& src);
  virtual ~ResponseMaker(void);
  ResponseMaker& operator=(ResponseMaker const& rhs);

  // Member Functions
  char* makeResponseMessage(Response& obj);

 private:
  Request m_request_data;
  Response m_response_data;
  Config m_config_data;
  MethodHandler* m_method_handler;
  char* m_response_message;

 private:
};

// 에러가 throw되었을 때 에러페이지를 상태코드를 받아서 페이지를 만듦
class ErrorPageMaker
{
};

#endif
