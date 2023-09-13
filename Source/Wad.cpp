#include "Wad.h"

namespace Wad
{
	std::vector<HedEntry> ReadHed(std::istream &stream)
	{
		// Read in full HED file
		stream.seekg(0, std::ios::end);
		size_t size = stream.tellg();
		stream.seekg(0, std::ios::beg);

		std::vector<char> hed(size);
		stream.read(hed.data(), size);

		// Read in HED entries
		std::vector<HedEntry> entries;

		auto in_begin = hed.begin();
		auto in_end = hed.end();

		auto in_it = in_begin;

		auto in_word = [&]()
		{
			unsigned long word = 0;
			for (int i = 0; i < 4; i++)
			{
				if (in_it == in_end)
					return word;
				word |= (unsigned long)(unsigned char)*in_it++ << (i * 8);
			}
			return word;
		};

		bool is_thaw = false;
		while (in_it != in_end)
		{
			HedEntry entry;

			// Read offset
			entry.offset = in_word();
			if (entry.offset == 0xFFFFFFFF)
				break;
			if (entry.offset & 0x7FF)
				is_thaw = true; // THAW stores offsets in sectors, if any of our entries are not sector aligned, assume it's a THAW wad

			// Read size
			entry.size = in_word();

			// Read name
			while (in_it != in_end)
			{
				char c = *in_it++;
				if (c == 0)
					break;
				entry.name += c;
			}

			// Align to 4 bytes
			while (in_it != in_end)
			{
				if ((in_it - in_begin) % 4 == 0)
					break;
				in_it++;
			}

			// Add entry
			entries.push_back(entry);
		}
		
		if (is_thaw)
		{
			for (auto &i : entries)
				i.offset <<= 11;
		}

		return entries;
	}
}
