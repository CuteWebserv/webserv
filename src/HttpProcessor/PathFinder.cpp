#include "PathFinder.hpp"
// #include "./PathTest/PathFinder.hpp" // for test

#include <iostream>

PathFinder::PathFinder() {}

PathFinder::~PathFinder() {}

PathFinder::~PathFinder(const PathFinder& origin) 
{};

PathFinder& operator=(parser const& origin)
{
  return (*this);
};


bool PathFinder::is_directory(const std::string& path)
{
  DIR* dir = opendir(path.c_str());
  if (dir)
  {
    closedir(dir);
    return true;
  }
  return false;
}

bool PathFinder::checkExist(const std::string& path_or_file)
{
  // return (true);
  return (access(path_or_file.c_str(), F_OK) == 0);
}

void PathFinder::setMethod(std::string method, Response& response_data)
{
  response_data.accepted_method = method;
}

void PathFinder::setUpload(std::string upload, Response& response_data)
{
  response_data.uploaded_path = upload;
}

void PathFinder::setRoot(std::string root, Response& response_data)
{
  if (checkExist(root))
  {
    response_data.file_path = root;
    response_data.path_exist = true;
  }
  //   else
  //   {
  //     response_data.path_exist = false;
  //   } //넣을까말까 고민중.. 초기화때 false가 있으면 필요없을 듯
}

void PathFinder::setIndex(std::string index, Response& response_data)
{
  // Post method의 경우, 요청받은 file이 존재하지 않더라도
  // 요청받은 파일 name을 기록할 필요가 있다.
  response_data.file_name = index;
  if (checkExist(index))
  {
    response_data.file_exist = true;
  }
}

void PathFinder::setAutoIndex(std::string auto_index, Response& response_data)
{
  if (auto_index == "on")
  {
    response_data.auto_index = true;
  }
}

bool PathFinder::setCgi(std::string locationBlock, t_server server_data,
                        Response& response_data)
{
  std::size_t pos_last = locationBlock.find_last_of(".");
  if (pos_last == std::string::npos) return (false);
  if (locationBlock.substr(locationBlock.find_last_of(".")) == ".py")
  {
    response_data.cgi_flag = true;
    t_location current_location = server_data.locations.find(".py")->second;
    response_data.cgi_bin_path = current_location.ourcgi_pass;
    response_data.uploaded_path =
        current_location.uploaded_path;  // 경로 존재하는지
    setIndex(current_location.ourcgi_index, response_data);
    setMethod(current_location.accepted_method, response_data);
    return true;
  }
  return false;
}

void PathFinder::test_print_location(t_location& c)
{
  std::cout << "✅"
            << "location"
            << "✅" << std::endl;
  std::cout << "language : " << c.language << std::endl;
  std::cout << "root : " << c.root << std::endl;
  std::cout << "auto_index : " << c.auto_index << std::endl;
  std::cout << "index : " << c.index << std::endl;
  std::cout << "ourcgi_pass : " << c.ourcgi_pass << std::endl;
  std::cout << "ourcgi_index : " << c.ourcgi_index << std::endl;
  std::cout << "uploaded_path : " << c.uploaded_path << std::endl;
  std::cout << "accepted_method : " << c.accepted_method << std::endl;
  std::cout << "✅--------------------✅" << std::endl;
}

void PathFinder::test_print_basics(Response& c)
{
  std::cout << "✅"
            << "response"
            << "✅" << std::endl;
  std::cout << "accepted : " << c.accepted_method << std::endl;
  std::cout << "auto : " << c.auto_index << std::endl;
  std::cout << "file_exist : " << c.file_exist << std::endl;
  std::cout << "file : " << c.file_name << std::endl;
  std::cout << "path_exist : " << c.path_exist << std::endl;
  std::cout << "path : " << c.file_path << std::endl;
  std::cout << "cgi_flag : " << c.cgi_flag << std::endl;
  std::cout << "cgi_path : " << c.cgi_bin_path << std::endl;
  std::cout << "save_path : " << c.uploaded_path << std::endl;
  std::cout << "✅--------------------✅" << std::endl;
}

void PathFinder::setBasic(std::string method, std::string root,
                          std::string index, std::string auto_index,
                          std::string upload, Response& response_data)
{
  std::cout << "root : " << root << std::endl;
  std::cout << "index : " << index << std::endl;
  setMethod(method, response_data);
  setRoot(root, response_data);
  setIndex(index, response_data);
  setUpload(upload, response_data);
  setAutoIndex(auto_index, response_data);
}

PathFinder::PathFinder(Request request_data, t_server server_data,
                       Response& response_data)
{
  std::string locationBlock;
  t_location current_location;

  locationBlock = request_data.uri;

  std::map<std::string, t_location>::iterator temp_location;
  if ((locationBlock) == "/" || (locationBlock) == "")  // default block
  {
    current_location = server_data.locations.find("/")->second;
    setBasic(current_location.accepted_method, current_location.root + "/",
             current_location.index, current_location.auto_index,
             current_location.uploaded_path, response_data);
    return;
  }
  if (setCgi((locationBlock), server_data, response_data))
  {
    return;
  }
  std::size_t pos_last = (locationBlock).rfind("/");
  if (pos_last == 0)  // '/a'처럼 location 블록이름만 들어온 경우
  {
    temp_location = server_data.locations.find(locationBlock);
    if (temp_location == server_data.locations.end())
    {
      // 들어온 블록이름이 location에 존재하지 않음.
      //  '/파일이름'으로 들어온 경우 후에 처리를 원하면 이 블록에서 로직 추가
      response_data.path_exist = false;
      response_data.file_exist = false;
      response_data.auto_index = false;
    }
    else
    {
      current_location = temp_location->second;
      setBasic(current_location.accepted_method, current_location.root + "/",
               current_location.index, current_location.auto_index,
               current_location.uploaded_path, response_data);
    }
  }
  else
  {  // "/block_name/b/c/d", "/??(location에 없음)/b/c/d"
     //"a/b/c/d(디렉토리)", "/a/b/c/d/e(파일)"
     // '/'로 끝나는 경우와 "/파일이름.txt(경로없이)"인 경우는 고려하지 않음.
     //   std::string path = "/a/b/c/d";
     // std::cout << "in this block" << std::endl;
    std::string location_key =
        (locationBlock).substr(0, (locationBlock).find("/", 1));
    temp_location = server_data.locations.find(location_key);
    if (temp_location == server_data.locations.end())
    {  // "/??(location에 없음)/b/c/d" 경우
      response_data.path_exist = false;
      response_data.file_exist = false;
      response_data.auto_index = false;
      return;
    }
    current_location = temp_location->second;
    std::string rest_of_uri =
        (locationBlock).substr((locationBlock).find("/", 1));
    std::string entire_path = current_location.root + rest_of_uri;
    pos_last = entire_path.rfind("/");
    if (is_directory(entire_path))  //"a/b/c/d(존재하는 디렉토리)"
    {
      setBasic(current_location.accepted_method, entire_path + "/",
               current_location.index, current_location.auto_index,
               current_location.uploaded_path, response_data);
    }
    else
    {  //"/a/b/c/d/e(파일)" 경우
      std::cout << pos_last << std::endl;
      std::cout << "entire : " << entire_path << std::endl;
      setBasic(current_location.accepted_method,
               entire_path.substr(0, pos_last + 1),
               entire_path.substr(pos_last + 1), current_location.auto_index,
               current_location.uploaded_path, response_data);
      test_print_basics(response_data);
    }
  }
}