#pragma once

#include <vector>
#include <unordered_map>
#include <filesystem>
#include <string>
#include <iostream>

namespace Wad
{
	struct HedEntry
	{
		std::string name;
		unsigned long offset, size;
	};

	std::vector<HedEntry> ReadHed(std::istream &stream);

	static inline std::filesystem::path NameToPath(const std::string &name)
	{
		std::filesystem::path in(name);
		std::filesystem::path out;
		for (auto &component : in)
		{
			if (component == "\\" || component == "/")
				continue;
			out /= component;
		}
		return out;
	}
}
