#include "PCM.h"

namespace PCM
{
	void WriteWAV(std::ostream &stream, unsigned long sample_rate, unsigned short channels, void *data, size_t size)
	{
		auto write32 = [&stream](unsigned long value)
		{
			for (int i = 0; i < 4; i++)
			{
				stream.put((char)(value & 0xFF));
				value >>= 8;
			}
		};

		auto write16 = [&stream](unsigned short value)
			{
				for (int i = 0; i < 2; i++)
				{
				stream.put((char)(value & 0xFF));
				value >>= 8;
			}
		};

		// Write WAV header
		stream.write("RIFF", 4);

		size_t riff_size = 36 + size;
		write32((unsigned long)riff_size);

		stream.write("WAVEfmt ", 8);

		unsigned long fmt_size = 16;
		write32(fmt_size);

		unsigned short fmt = 1;
		write16(fmt);

		write16(channels);

		write32(sample_rate);

		unsigned long byte_rate = sample_rate * channels * (16 / 8);
		write32(byte_rate);

		unsigned short block_align = channels * (16 / 8);
		write16(block_align);

		unsigned short bits_per_sample = 16;
		write16(bits_per_sample);

		stream.write("data", 4);
		write32((unsigned long)size);

		stream.write((char*)data, size);
	}
}
