#include "HttpProcessor.hpp"

#include "CgiHandler.hpp"
#include "Log.hpp"

bool checkExist(const std::string& path_or_file)
{
  return (access(path_or_file.c_str(), F_OK) == 0);
}

void HttpProcessor::setErrorPage(t_server server_data, Response& response_data)
{
  if (server_data.error_page.size() == 0)
  {
    response_data.error_keyword = false;
    return;
  }
  response_data.error_keyword = true;
  if (checkExist(server_data.error_page[0]))
    response_data.error_page_path = server_data.error_page[0];
};

HttpProcessor::~HttpProcessor(void) {}

HttpProcessor::HttpProcessor(Request& request_data, Response& response_data,
                             t_server& server_data)
    : m_request_data(request_data), m_response_data(response_data)
{
  setErrorPage(server_data, m_response_data);
  if (request_data.status == NO_PROBLEM) request_data.status = OK_200;
  try
  {
    if (request_data.status != OK_200) throw request_data.status;
  }
  catch (StatusCode code_num)
  {
    m_response_data.status_code = code_num;
    return;
  }
  try
  {
    PathFinder path_finder(request_data, server_data, m_response_data);
    if (m_request_data.method == "GET" &&
        m_response_data.accepted_method.find("GET") != std::string::npos)
    {
      if (m_response_data.cgi_flag == true)
      {
        GetCgiHandler cgi_handler(m_request_data, m_response_data);
        cgi_handler.outsourceCgiRequest();
        m_response_data = cgi_handler.get_m_response_data();
      }
      else
      {
        GetMethodHandler method_handler(m_request_data, m_response_data);
        method_handler.methodRun();
        // m_response_data = method_handler.get_m_response_data();
      }
    }
    else if (m_request_data.method == "POST" &&
             m_response_data.accepted_method.find("POST") != std::string::npos)
    {
      if (m_response_data.cgi_flag == true)
      {
        PostCgiHandler cgi_handler(m_request_data, m_response_data);
        cgi_handler.outsourceCgiRequest();
        m_response_data = cgi_handler.get_m_response_data();
      }
      else
      {
        PostMethodHandler method_handler(m_request_data, m_response_data);
        method_handler.methodRun();
        // m_response_data = method_handler.get_m_response_data();
      }
    }
    else if (m_request_data.method == "PUT" &&
             m_response_data.accepted_method.find("PUT") != std::string::npos)
    {
      PutMethodHandler method_handler(m_request_data, m_response_data);
      method_handler.methodRun();
      // m_response_data = method_handler.get_m_response_data();
    }
    else if (m_request_data.method == "HEAD" &&
             m_response_data.accepted_method.find("HEAD") != std::string::npos)
    {
      GetMethodHandler method_handler(m_request_data, m_response_data);
      method_handler.methodRun();
      // m_response_data = method_handler.get_m_response_data();
    }
    else if (m_request_data.method == "DELETE" &&
             m_response_data.accepted_method.find("DELETE") !=
                 std::string::npos)
    {
      DeleteMethodHandler method_handler(m_request_data, m_response_data);
      method_handler.methodRun();
      // m_response_data = method_handler.get_m_response_data();
    }
    else
      throw METHOD_NOT_ALLOWED_405;
  }
  catch (StatusCode code_num)
  {
    LOG_ERROR("[HttpProcessor] constructor catches error (status code: %d)",
              code_num);
    m_response_data.status_code = code_num;
  }
}

HttpProcessor::HttpProcessor(const HttpProcessor& obj)
    : m_request_data(obj.m_request_data), m_response_data(obj.m_response_data)
{
}

HttpProcessor& HttpProcessor::operator=(HttpProcessor const& obj)
{
  if (this == &obj) return (*this);
  return (*this);
}

struct Response& HttpProcessor::get_m_response(void)
{
  return (m_response_data);
}
