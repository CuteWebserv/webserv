#include "CgiHandler.hpp"

// canonical form

PostCgiHandler::PostCgiHandler() {}

PostCgiHandler::~PostCgiHandler() {}

PostCgiHandler::PostCgiHandler(Request& request_data, Response& response_data)
    : CgiHandler(request_data, response_data)
{
}

PostCgiHandler::PostCgiHandler(const PostCgiHandler& obj) { *this = obj; }

PostCgiHandler& PostCgiHandler::operator=(PostCgiHandler const& obj)
{
  if (this != &obj)
  {
    m_request_data = obj.m_request_data;
    m_response_data = obj.m_response_data;
    m_env_list = obj.m_env_list;
    m_env_list_parameter = obj.m_env_list_parameter;
    m_to_child_fds[READ] = obj.m_to_child_fds[READ];
    m_to_child_fds[WRITE] = obj.m_to_child_fds[WRITE];
    m_to_parent_fds[READ] = obj.m_to_parent_fds[READ];
    m_to_parent_fds[WRITE] = obj.m_to_parent_fds[WRITE];
    m_pid = obj.m_pid;
  }
  return (*this);
}

// member functions

void PostCgiHandler::executeCgi()
{
  close(m_to_child_fds[WRITE]);

  if (dup2(m_to_child_fds[READ], STDIN_FILENO) == -1)
  {
    LOG_ERROR("failed to dup2(%d, %d)", m_to_child_fds, STDIN_FILENO);
    close(m_to_child_fds[READ]);
    close(m_to_parent_fds[WRITE]);
    exit(EXIT_FAILURE);
  }
  close(m_to_child_fds[READ]);

  close(m_to_parent_fds[READ]);
  if (dup2(m_to_parent_fds[WRITE], STDOUT_FILENO) == -1)
  {
    LOG_ERROR("failed to dup2(%d, %d)", m_to_parent_fds, STDOUT_FILENO);
    close(m_to_child_fds[READ]);
    close(m_to_parent_fds[WRITE]);
    exit(EXIT_FAILURE);
  }
  close(m_to_parent_fds[WRITE]);

  setCgiEnv();
  const char* cgi_bin_path = m_response_data.cgi_bin_path.c_str();
  std::string cgi_file = m_response_data.root_path + "/" + m_response_data.file_name;
  const char* argv[] = {cgi_bin_path, cgi_file.c_str(), NULL};
  const char** envp = &m_env_list_parameter[0];
  if (execve(cgi_bin_path, const_cast<char* const*>(argv),
             const_cast<char* const*>(envp)) == RETURN_ERROR)
  {
    LOG_ERROR("Failed to execve function => strerrno: %s", strerror(errno));
    std::vector<char> error_message = makeErrorPage();
    write(STDOUT_FILENO, &error_message[0], error_message.size());
    exit(EXIT_FAILURE);
  }
}

void PostCgiHandler::outsourceCgiRequest(void)
{
  try
  {
    pipeAndFork();

    if (m_pid == CHILD_PROCESS)
    {
      executeCgi();
    }
    else
    {
      close(m_to_child_fds[READ]);
      close(m_to_parent_fds[WRITE]);

      m_response_data.read_pipe_fd = m_to_parent_fds[READ];
      m_response_data.write_pipe_fd = m_to_child_fds[WRITE];
      m_response_data.cgi_child_pid = m_pid;
    }
  }
  catch (const std::exception& e)
  {
    LOG_ERROR("catch error %s", e.what());
    m_response_data.body = makeErrorPage();
  }
}
