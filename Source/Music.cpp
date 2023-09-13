#include "Wad.h"
#include "ADPCM.h"
#include "PCM.h"

#include <iostream>
#include <fstream>

#define CHANNELS 2 // Music is stereo
#define BUFFER 0x1800 // In blocks, one channel

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
		// NOTE: The ADPCM stream always contains full buffers for each channel, even if the size is cut off
		// The entry's size in the hed moreso represents the length of the song itself without padding
		size_t true_size = (entry.size + (BUFFER * CHANNELS * 16) - 1) / (BUFFER * CHANNELS * 16) * (BUFFER * CHANNELS * 16);

		size_t buffers = (true_size / 16) / (BUFFER * CHANNELS);
		std::vector<short> adpcm_data((buffers * (BUFFER * CHANNELS)) * 28);

		{
			std::vector<char> data(true_size);
			wad_stream.read(data.data(), true_size);

			// Decode ADPCM
			ADPCM::ADPCM ctx[CHANNELS];

			char *data_p = data.data();
			short *adpcm_p = adpcm_data.data();

			for (size_t i = buffers; i > 0; --i)
			{
				for (size_t j = 0; j < CHANNELS; j++)
				{
					short *adpcm_p_ch = adpcm_p + j;
					for (size_t b = 0; b < BUFFER; b++)
					{
						short temp[28];
						ctx[j].DecodeBlock(data_p, temp);
						data_p += 16;

						for (size_t t = 0; t < 28; t++)
						{
							*adpcm_p_ch = temp[t];
							adpcm_p_ch += CHANNELS;
						}
					}
				}

				adpcm_p += BUFFER * CHANNELS * 28;
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
		PCM::WriteWAV(output_stream, 48000, 2, adpcm_data.data(), (entry.size / 16) * (28 * 2));
	}

	return 0;
}
