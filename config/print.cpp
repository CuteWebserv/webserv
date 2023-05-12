#include "Config.hpp"

void showIterVector(std::vector<std::string> vector)
{
	for (std::vector<std::string>::iterator iter = vector.begin(); iter != vector.end(); ++iter)
	{
		std::cout << iter[0] << " ";
	}
}

void showIterMap(map_string_string map)
{
	for (map_string_string::iterator iter = map.begin(); iter != map.end(); ++iter)
	{
		std::cout << "		" << iter->first << " " << iter->second[0] << std::endl;
	}
}

void Config::showServerConf()
{
	for(config_map::iterator iter = serverConf.begin(); iter != serverConf.end(); ++iter)
	{
		std::cout << iter->first << " ";
		showIterVector(iter->second.first);
		std::cout << std::endl;
		if (iter->second.second != nullMap)
		{
			showIterMap(iter->second.second);
			std::cout << "	}" << std::endl;
		}
	}
}
