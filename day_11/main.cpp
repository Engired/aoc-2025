#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include <vector>
#include <algorithm>

const std::vector<std::string> terminator_1 = {"ujd", "odk", "zlj", "qqo", "vxh"};
const std::vector<std::string> terminator_2 = {"qbn", "you", "zpa", "jku", "niz"};

size_t path(std::unordered_map<std::string, std::vector<std::string>>& map,
			std::vector<std::string>& next, std::string find, 
			const std::vector<std::string>& terminator)
{
	if(std::find(next.begin(), next.end(), find) != next.end())
		return 1;

	size_t ret = 0;
	for(auto& i : next)
	{
		if(std::find(terminator.begin(), terminator.end(), i) != terminator.end())
			continue;
		
		ret += path(map, map[i], find, terminator);
	}

	return ret;
}

int main()
{	
	std::ifstream file("input");
	std::vector<std::string> file_string;
	std::string str; 

	std::unordered_map<std::string, std::vector<std::string>> map;
	
	size_t ret = 0;
	
	while (std::getline(file, str))
	{
		file_string.push_back(str);
	}
	
	for(auto& i : file_string)
	{
		// add new empty vectors to hashmap
		map.emplace(i.substr(0, 3), (std::vector<std::string>){});
		
		for(int j = 5; j < i.size(); j += 4)
		{
			map[i.substr(0, 3)].push_back(i.substr(j, 3));
		}
	}

	std::cout << "starting trace...\n";

	// find amount of paths to fft
	size_t answer = path(map, map["svr"], "fft", terminator_1);
	std::cout << "found fft\n";
	answer *= path(map, map["fft"], "dac", terminator_2);
	std::cout << "fft -> dac\n";
	answer *= path(map, map["dac"], "out", {});
	
	std::cout << answer << '\n';
	
	return 0;
}
