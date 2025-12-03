#include <cstring>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <unordered_map>
#include <vector>

int main()
{
	std::ifstream file("input");
	std::string str; 

	std::unordered_map<int, std::vector<int>> map;

	size_t ret = 0;
	
	while (std::getline(file, str))
	{
		map.clear();
		
		// create hashmap of number -> index array
		for(int i = 0; i < std::strlen(str.c_str()); i++)
		{
			map[str[i] - '0'].push_back(i);
		}

		// starting from 9, go down until a high pair is found
		for(int i = 9; i > 0; i--)
		{
			if(map.count(i) <= 0)
				continue;
			int start_index = map[i][0];
			for(int j = 9; j > 0; j--)
			{
				// error handling
				if(map.count(j) <= 0)
					continue;
				std::vector<int>* ref = &map[j];
				for(auto it : *ref)
				{
					if(it > start_index)
					{
						ret += (i * 10) + j;
						goto skip;
					}
				}
			}
		}

skip:
	{}
	}

	printf("Got: %d\n", ret);

	return 0;
}
