#include "ADPCM.h"

namespace ADPCM
{
	short ADPCM::DecodeNibble(unsigned char nibble, long shift, long f0, long f1)
	{
		// Sign extend nibble
		long t = nibble;
		if (t & 0x8)
			t -= 0x10;

		// Decode and clip sample
		long s = (t << shift) + ((filter_old * f0 + filter_older * f1 + 32) / 64);
		if (s < -0x7FFF)
			s = -0x7FFF;
		if (s > 0x7FFF)
			s = 0x7FFF;

		// Write sample
		filter_older = filter_old;
		filter_old = s;

		return (short)s;
	}

	void ADPCM::DecodeBlock(char *in, short *out)
	{

		// Read shift filter and flag bytes
		unsigned char shift_filter = in[0];
		unsigned char flags = in[1];

		long shift = 12 - (shift_filter & 0x0F);
		if (shift < 0)
			shift = 0;
		long filter = (shift_filter & 0x70) >> 4;

		// Select filter
		static const struct
		{
			char f0, f1;
		} c_filters[8] = {
			{   0,   0 },
			{  60,   0 },
			{ 115, -52 },
			{  98, -55 },
			{ 122, -60 }, // Filter 4 is only available to SPU ADPCM
			{   0,   0 },
			{   0,   0 },
			{   0,   0 }
		};

		long f0 = c_filters[filter].f0;
		long f1 = c_filters[filter].f1;

		// Decode nibles
		short *outp = out;
		for (int i = 2; i < 16; i++)
		{
			*outp++ = DecodeNibble((in[i] >> 0) & 0xF, shift, f0, f1);
			*outp++ = DecodeNibble((in[i] >> 4) & 0xF, shift, f0, f1);
		}
	}
}
