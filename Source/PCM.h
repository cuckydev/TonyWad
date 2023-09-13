#pragma once

#include <iostream>

#include <cstddef>

namespace PCM
{
	void WriteWAV(std::ostream &stream, unsigned long sample_rate, unsigned short channels, void *data, size_t size);
}