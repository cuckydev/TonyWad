#pragma once

namespace ADPCM
{
	class ADPCM
	{
		private:
			long filter_old = 0, filter_older = 0;

			short DecodeNibble(unsigned char nibble, long shift, long f0, long f1);

		public:
			void DecodeBlock(char *in, short *out);
	};
}
