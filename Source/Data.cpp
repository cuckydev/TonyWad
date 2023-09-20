#include "Wad.h"

#include <iostream>
#include <fstream>

int main(int argc, char *argv[])
{
	// Check arguments
	if (argc < 4)
	{
		std::cout << "Usage: " << argv[0] << " <input.hed> <input.wad> <output>" << std::endl;
		return 0;
	}

	// Read hed
	std::vector<Wad::HedEntry> hed;

	{
		std::ifstream hed_stream(argv[1], std::ios::binary);
		if (!hed_stream)
		{
			std::cout << "Failed to open " << argv[1] << std::endl;
			return 1;
		}

		hed = Wad::ReadHed(hed_stream);
	}

	// Write out files
	std::ifstream wad_stream(argv[2], std::ios::binary);
	if (!wad_stream)
	{
		std::cout << "Failed to open " << argv[2] << std::endl;
		return 1;
	}

	for (auto &entry : hed)
	{
		// Seek to entry
		wad_stream.seekg(entry.offset);

		// Read data
		std::vector<char> data(entry.size);
		wad_stream.read(data.data(), entry.size);

		// Open output file
		std::filesystem::path name = entry.name;
		std::filesystem::path output_path = std::filesystem::path(argv[3]) / Wad::NameToPath(entry.name);

		std::filesystem::create_directories(output_path.parent_path());

		std::ofstream output_stream(output_path, std::ios::binary);
		if (!output_stream)
		{
			std::cout << "Failed to open " << output_path << std::endl;
			return 1;
		}

		// Write data
		output_stream.write(data.data(), entry.size);
	}

	return 0;
}
