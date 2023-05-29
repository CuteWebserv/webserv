#include "/Users/surinlee/Desktop/42/5circle/our_webserv/include/PathFinder.hpp"
// #include "../../include/PathFinder.hpp"
// #include "PathFinder.hpp"

PathFinder::PathFinder() {}

PathFinder::~PathFinder() {}

bool is_directory(const std::string& path)
{
  DIR* dir = opendir(path.c_str());
  if (dir)
  {
    closedir(dir);
    return true;
  }
  return false;
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
  if (checkExist(index))
  {
    response_data.file_name = index;
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

PathFinder::PathFinder(Request requset_data, t_server server_data,
                       Response& response_data)
{
  std::string locationBlock = requset_data.uri;
  t_location current_location;
  std::map<std::string, t_location>::iterator temp_location;

  if (locationBlock == "/" || locationBlock == "")  // default block
  {
    current_location = server_data.locations.find("/")->second;
    setRoot(current_location.root, response_data);
    setIndex(current_location.index, response_data);
    setAutoIndex(current_location.auto_index, response_data);
    return;
  }
  std::size_t pos = locationBlock.find_last_of("/");
  if (pos == 0)  // '/a'처럼 location 블록이름만 들어온 경우
  {
    temp_location = server_data.locations.find(locationBlock);
    if (temp_location == server_data.locations.end())
    {
      response_data.path_exist = false;
      response_data.file_exist = false;
      response_data.auto_index = false;
    }
    else
    {
      current_location = temp_location->second;
      setRoot(current_location.root, response_data);
      setIndex(current_location.index, response_data);
      setAutoIndex(current_location.auto_index, response_data);
    }
  }
  else
  {  // "/block_name/b/c/d", "/??(location에 없음)/b/c/d"
     //"a/b/c/d(디렉토리)", "/a/b/c/d/e(파일)"
     // '/'로 끝나는 경우와 "/파일이름.txt(경로없이)"인 경우는 고려하지 않음.
     //   std::string path = "/a/b/c/d";
    std::string location_key =
        locationBlock.substr(0, locationBlock.find("/", 1));
    temp_location = server_data.locations.find(location_key);
    if (temp_location == server_data.locations.end())
    {  //"/??(location에 없음)/b/c/d" 경우
      response_data.path_exist = false;
      response_data.file_exist = false;
      response_data.auto_index = false;
      return;
    }
    current_location = temp_location->second;
    std::string rest_of_uri = locationBlock.substr(locationBlock.find("/", 1));
    std::string entire_path = current_location.root + rest_of_uri;
    if (is_directory(entire_path))  //"a/b/c/d(디렉토리)"
    {
      setRoot(entire_path, response_data);
      setIndex(current_location.index, response_data);
      setAutoIndex(current_location.auto_index, response_data);
    }
  }
}

bool PathFinder::checkExist(const std::string& path_or_file)
{
  return (access(path_or_file.c_str(), F_OK) == 0);
}
