// #include "PathFinder.hpp"
// #include "Log.hpp"

// #include "./PathTest/Log.hpp" //for test
#include "./PathFinder.hpp" // for test

#include <iostream>

PathFinder::PathFinder() {}

PathFinder::~PathFinder() {}

PathFinder::PathFinder(const PathFinder& origin){};

PathFinder& PathFinder::operator=(PathFinder const& origin) { return (*this); };

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

void PathFinder::setIndex(std::string root, std::string index,
                          Response& response_data)
{
  // Post method의 경우, 요청받은 file이 존재하지 않더라도
  // 요청받은 파일 name을 기록할 필요가 있다.
  response_data.file_name = index;
  if (checkExist(root + index))
  {
    response_data.file_exist = true;
  }
  else
  {
    response_data.file_exist = false;
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
    setIndex(current_location.root + "/", current_location.ourcgi_index,
             response_data);
    setMethod(current_location.accepted_method, response_data);
    return true;
  }
  return false;
}

void PathFinder::setRedirection(std::string redirection,
                                Response& response_data)
{
  if (redirection != "")
  {
    response_data.rediretion_location = redirection;
    response_data.redirection_exist = true;
  }
}

void PathFinder::test_print_location(t_location& c)
{
  std::cout <<   "🧪 test_print_location 🧪" << std::endl;
  std::cout <<   "language: " << c.language.c_str() << std::endl;
  std::cout << "root: " << c.root.c_str() << std::endl;
  std::cout <<   "auto_index: "<< c.auto_index.c_str() << std::endl;
  std::cout << "index: " << c.index.c_str() << std::endl;
  std::cout << "ourcgi_pass: " <<  c.ourcgi_pass.c_str() << std::endl;
  std::cout << "ourcgi_index: " <<  c.ourcgi_index.c_str() << std::endl;
  std::cout << "uploaded_path: " <<  c.uploaded_path.c_str() << std::endl;
  std::cout << "accepted_method: " <<  c.accepted_method.c_str() << std::endl;
  std::cout << "🧪 test_print_location 🧪" << std::endl;
}

void PathFinder::test_print_basics(Response& c)
{
  std::cout << "🧪 test_print_basics 🧪" << std::endl;
  std::cout << "accepted_method: " <<  c.accepted_method.c_str() << std::endl;
  std::cout << "auto_index: " <<  c.auto_index << std::endl;
  std::cout << "file_exist: " <<  c.file_exist << std::endl;
  std::cout << "file_name: " <<  c.file_name.c_str() << std::endl;
  std::cout << "path_exist: " <<  c.path_exist << std::endl;
  std::cout << "file_path: " <<  c.file_path.c_str() << std::endl;
  std::cout << "cgi_flag: " <<  c.cgi_flag << std::endl;
  std::cout << "cgi_bin_path: " <<  c.cgi_bin_path.c_str() << std::endl;
  std::cout << "uploaded_path: " <<  c.uploaded_path.c_str() << std::endl;
  std::cout << "🧪 test_print_basics 🧪" << std::endl;
}

void PathFinder::setBasic(std::string method, std::string root,
                          std::string index, std::string auto_index,
                          std::string upload, std::string redirection,
                          Response& response_data)
{
  std::cout << "Default server block (root: , index: " << root << "," << index << std::endl;
  setMethod(method, response_data);
  setRoot(root, response_data);
  setIndex(root, index, response_data);
  setUpload(upload, response_data);
  setAutoIndex(auto_index, response_data);
  setRedirection(redirection, response_data);
}

PathFinder::PathFinder(Request& request_data, t_server& server_data,
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
             current_location.uploaded_path, current_location.redirection,
             response_data);
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
      current_location = server_data.locations.find("/")->second;
      if (checkExist(current_location.root + "/" + locationBlock))
      { // '/' 기본 블럭 뒤 파일 이름 or 디렉토리 이름 허용 -> default 위치 auto 인덱스 하려면 꼭 필요
        setBasic(current_location.accepted_method, current_location.root + locationBlock,
               current_location.index, current_location.auto_index,
               current_location.uploaded_path, current_location.redirection,
               response_data);
        if (!is_directory(current_location.root + locationBlock))
          setIndex(current_location.root + "/", locationBlock, response_data);
      }
      else
      {
      // 들어온 블록이름이 location에 존재하지 않음.
      response_data.path_exist = false;
      response_data.file_exist = false;
      response_data.auto_index = false;
      }
    }
    else
    {
      current_location = temp_location->second;
      setBasic(current_location.accepted_method, current_location.root + "/",
               current_location.index, current_location.auto_index,
               current_location.uploaded_path, current_location.redirection,
               response_data);
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
    {  // "/??(location에 없음)/b/c/d" 경우 + "/(기본디렉토리)/(그 안의 디렉토리)/그 안의 파일"
       current_location = server_data.locations.find("/")->second;
      if (checkExist(current_location.root + "/" + locationBlock))
      { //  기본 블럭 뒤 파일 이름 or 디렉토리 이름 허용 -> default 위치 auto 인덱스 하려면 꼭 필요
        std::string rest_of_uri =
            (locationBlock).substr((locationBlock).find("/"));
        std::cout << rest_of_uri << std::endl;
        std::string entire_path = current_location.root + rest_of_uri;
        pos_last = entire_path.rfind("/");
        if (!is_directory(current_location.root + locationBlock))
        {// 파일로 끝나는 경로가 온 경우
          setBasic(current_location.accepted_method,
               entire_path.substr(0, pos_last + 1),
               entire_path.substr(pos_last + 1), current_location.auto_index,
               current_location.uploaded_path, current_location.redirection,
               response_data);
          return ;
        }
        // 디렉토리로 끝나는 경우가 온 경우
        setBasic(current_location.accepted_method, current_location.root + "/",
               current_location.index, current_location.auto_index,
               current_location.uploaded_path, current_location.redirection,
               response_data);
      }
      else
      { // 존재하지 않는 블럭 && 디폴트 폴더 내부 파일 or 디렉토리도 아님
        response_data.path_exist = false;
        response_data.file_exist = false;
        response_data.auto_index = false;
      }
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
               current_location.uploaded_path, current_location.redirection,
               response_data);
    }
    else
    {  //"/a/b/c/d/e(파일)" 경우
      std::cout << "pos_last: " << pos_last << std::endl;
      std::cout << "entire_path: " << 
                 entire_path.c_str() << std::endl;
      setBasic(current_location.accepted_method,
               entire_path.substr(0, pos_last + 1),
               entire_path.substr(pos_last + 1), current_location.auto_index,
               current_location.uploaded_path, current_location.redirection,
               response_data);
      test_print_basics(response_data);
    }
  }
}