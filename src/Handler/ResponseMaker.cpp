#include "ResponseMaker.hpp"

// Default Constructor
ResponseMaker::ResponseMaker(void) {}

ResponseMaker::ResponseMaker(Request& reqeust_data, Config& config_data)
{
  // parsing의 유효성 검사(일반적) -> 상태코드 업데이트
  // 이닛할거 하고
  //   m_request_data = reqeust_data;
  //   m_config_data = config_data;
  // switch문으로  methodHandler 를 적당히넣기
  // try
  // {
  //   if (m_reuest_data.status_c)
  //     throw stsu
  //   m_method_handler.doSomething(); -> response 구조체 채워짐
  //   makeResponseMessage();
  // }
  // catch(StatusCode code_Num)
  // {
  //   errorPageMaker(Code_NUm);
  //    m_response_message = errorPageMaker.getMesaage();
  // }
}
// Copy Constructor
ResponseMaker::ResponseMaker(const ResponseMaker& src) {}

// Destructor
ResponseMaker::~ResponseMaker(void) {}

// Copy Assignment Operator
ResponseMaker& ResponseMaker::operator=(ResponseMaker const& rhs) {}