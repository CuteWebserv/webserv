#include "HttpProcessor.hpp"
#include "Log.hpp"
#include "ResponseGenerator.hpp"
#include "Server.hpp"
#include "ServerFinder.hpp"

void Server::addStaticRequestEvent(struct kevent *current_event,
                                   t_event_udata *current_udata,
                                   struct Request &request,
                                   struct Response &response)
{
  Request *new_request;
  Response *new_response;

  // GET, HEAD, DELETE
  if (response.static_read_file_fd != -1)
  {
    off_t file_size;
    t_event_udata *udata;

    file_size = lseek(response.static_read_file_fd, 0, SEEK_END);
    if (file_size == -1)
    {
      throw INTERNAL_SERVER_ERROR_500;
    }
    lseek(response.static_read_file_fd, 0, SEEK_SET);
    response.static_read_file_size = file_size;
    response.body.reserve(file_size);

    new_request = new Request(request);
    m_udata_map[current_udata->m_client_sock].push_back(new_request);

    new_response = new Response(response);
    m_udata_map[current_udata->m_client_sock].push_back(new_response);

    udata = new t_event_udata(STATIC_FILE, current_udata->m_servers,
                              new_request, new_response);
    m_udata_map[current_udata->m_client_sock].push_back(udata);

    udata->m_client_sock = current_event->ident;
    addEventToChangeList(m_kqueue.change_list, response.static_read_file_fd,
                         EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, udata);
  }
  // POST, PUT
  else if (response.static_write_file_fd != -1)
  {
    t_event_udata *udata;

    new_request = new Request(request);
    m_udata_map[current_udata->m_client_sock].push_back(new_request);
    new_response = new Response(response);
    m_udata_map[current_udata->m_client_sock].push_back(new_response);
    udata = new t_event_udata(STATIC_FILE, current_udata->m_servers,
                              new_request, new_response);
    m_udata_map[current_udata->m_client_sock].push_back(udata);
    udata->m_client_sock = current_event->ident;
    addEventToChangeList(m_kqueue.change_list, response.static_write_file_fd,
                         EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, udata);
  }
  // auto_index
  else
  {
    ResponseGenerator response_generator(request, response);
    std::vector<char> response_message;
    t_event_udata *udata;

    response_message = response_generator.generateResponseMessage();
    udata = new t_event_udata(CLIENT, current_udata->m_servers, NULL, NULL);
    m_udata_map[current_udata->m_client_sock].push_back(udata);
    udata->m_response_write.message = response_message;
    udata->m_response_write.offset = 0;
    udata->m_response_write.length = response_message.size();

    addEventToChangeList(m_kqueue.change_list, current_event->ident,
                         EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, udata);

    Log::printRequestResult(current_udata);
  }
}

void Server::staticFileReadEvent(struct kevent *current_event)
{
  ssize_t read_byte;
  char buf[BUF_SIZE];
  t_event_udata *current_udata;

  current_udata = static_cast<t_event_udata *>(current_event->udata);
  read_byte = read(current_event->ident, buf, BUF_SIZE);
  if (read_byte > 0)
  {
    for (ssize_t idx = 0; idx < read_byte; ++idx)
    {
      current_udata->m_response->body.push_back(buf[idx]);
    }
  }
  if (read_byte < BUF_SIZE)
  {
    close(current_event->ident);
    t_event_udata *udata;
    Request *request = current_udata->m_request;
    Response *response = current_udata->m_response;
    ResponseGenerator response_generator(*request, *response);

    udata = new t_event_udata(CLIENT, current_udata->m_servers, NULL, NULL);
    m_udata_map[current_udata->m_client_sock].push_back(udata);
    udata->m_response_write.message =
        response_generator.generateResponseMessage();
    udata->m_response_write.offset = 0;
    udata->m_response_write.length = udata->m_response_write.message.size();

    Log::printRequestResult(current_udata);
    addEventToChangeList(m_kqueue.change_list, current_udata->m_client_sock,
                         EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, udata);
    ft_delete(&current_udata->m_request, current_udata->m_client_sock);
    ft_delete(&current_udata->m_response, current_udata->m_client_sock);
    ft_delete(&current_udata, current_udata->m_client_sock);
  }
}

void Server::fileWriteEvent(struct kevent *current_event)
{
  t_event_udata *current_udata;
  int possible_write_length;
  size_t request_body_size;
  size_t file_write_length;
  size_t file_write_offset;
  ssize_t write_byte;

  current_udata = static_cast<t_event_udata *>(current_event->udata);
  struct Request &current_request = *current_udata->m_request;
  possible_write_length = current_event->data;
  file_write_offset = current_udata->m_file_write_offset;
  request_body_size = current_request.body.size();

  if (possible_write_length > 0)
  {
    file_write_length = request_body_size - file_write_offset;
    write_byte =
        write(current_event->ident, &current_request.body[file_write_offset],
              file_write_length);
    if (write_byte == -1)
    {
      Log::print(ERROR, "write error");
    }
    else
    {
      current_udata->m_file_write_offset += write_byte;
    }
  }

  if (current_udata->m_file_write_offset == request_body_size)
  {
    ResponseGenerator response_generator(*current_udata->m_request,
                                         *current_udata->m_response);
    std::vector<char> response_message;
    t_event_udata *udata;

    response_message = response_generator.generateResponseMessage();
    udata = new t_event_udata(CLIENT, current_udata->m_servers, NULL, NULL);
    m_udata_map[current_udata->m_client_sock].push_back(udata);
    udata->m_response_write.message = response_message;
    udata->m_response_write.offset = 0;
    udata->m_response_write.length = response_message.size();

    addEventToChangeList(m_kqueue.change_list, current_udata->m_client_sock,
                         EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, udata);
    Log::printRequestResult(current_udata);
    close(current_event->ident);
    ft_delete(&current_udata->m_request, current_udata->m_client_sock);
    ft_delete(&current_udata->m_response, current_udata->m_client_sock);
    ft_delete(&current_udata, current_udata->m_client_sock);
  }
}
