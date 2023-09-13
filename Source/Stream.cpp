#include "Wad.h"
#include "ADPCM.h"
#include "PCM.h"

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

	// Write out WAVs
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
		size_t blocks = entry.size / 16;
		std::vector<short> adpcm_data(blocks * 28);

		{
			std::vector<char> data(entry.size);
			wad_stream.read(data.data(), entry.size);

			// Decode ADPCM
			ADPCM::ADPCM ctx;

			char *data_p = data.data();
			short *adpcm_p = adpcm_data.data();

			for (size_t i = blocks; i > 0; --i)
			{
				ctx.DecodeBlock(data_p, adpcm_p);
				data_p += 16;
				adpcm_p += 28;
			}
		}

		// Open output file
		std::filesystem::path name = entry.name;
		std::filesystem::path output_path = (std::filesystem::path(argv[3]) / Wad::NameToPath(entry.name)) += ".wav";

		std::filesystem::create_directories(output_path.parent_path());

		std::ofstream output_stream(output_path, std::ios::binary);
		if (!output_stream)
		{
			std::cout << "Failed to open " << output_path << std::endl;
			return 1;
		}

		// Write WAV
		PCM::WriteWAV(output_stream, 22050, 1, adpcm_data.data(), adpcm_data.size() * sizeof(short));
	}

	return 0;
}
