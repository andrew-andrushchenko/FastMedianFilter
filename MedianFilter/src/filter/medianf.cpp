#include "medianf.h"
#include <iostream>

Histogram_C3::Histogram_C3(int ksize) 
	: r{0}, g{0}, b{0}, m_Ksize(ksize), m_HalfWindowSize((ksize * ksize) / 2)
{
}

void Histogram_C3::Reset()
{
	for (int i = 0; i < 256; i++)
	{
		this->r[i] = 0;
		this->g[i] = 0;
		this->b[i] = 0;
	}
}

void Histogram_C3::Calc(const Image& win)
{
	for (int y = 0; y < this->m_Ksize; y++)
	{
		for (int x = 0; x < this->m_Ksize; x++)
		{
			this->r[ win.GetData()[ (x + y * win.GetWidth()) * win.GetChannels()]     ]++;
			this->g[ win.GetData()[ (x + y * win.GetWidth()) * win.GetChannels() + 1] ]++;
			this->b[ win.GetData()[ (x + y * win.GetWidth()) * win.GetChannels() + 2] ]++;
		}
	}
}

void Histogram_C3::Update(const Image& last_window, const Image& new_window)
{
	for (int y = 0; y < this->m_Ksize; y++)
	{
		this->r[ last_window.GetData()[ (y * last_window.GetWidth()) * last_window.GetChannels() ]    ]--;
		this->g[ last_window.GetData()[ (y * last_window.GetWidth()) * last_window.GetChannels() + 1] ]--;
		this->b[ last_window.GetData()[ (y * last_window.GetWidth()) * last_window.GetChannels() + 2] ]--;

		this->r[ new_window.GetData()[ ( (this->m_Ksize - 1) + y * new_window.GetWidth() ) * new_window.GetChannels() ]     ]++;
		this->g[ new_window.GetData()[ ( (this->m_Ksize - 1) + y * new_window.GetWidth() ) * new_window.GetChannels() + 1 ] ]++;
		this->b[ new_window.GetData()[ ( (this->m_Ksize - 1) + y * new_window.GetWidth() ) * new_window.GetChannels() + 2 ] ]++;
	}
}

Image::ColorRGB Histogram_C3::MedianColor()
{
	return { Median(this->r), Median(this->g), Median(this->b) };
}

uint8_t Histogram_C3::Median(const uint8_t* channel)
{
	uint8_t i = 0;
	uint32_t accum = 0;

	while (i < 256) // less than uint8_t max value
	{
		accum += channel[i];
		if (accum > this->m_HalfWindowSize) break;
		i++;
	}

	return i;
}

bool MedianSmooth(const Image& in, Image& out, int ksize)
{
	if (!(ksize % 2))
	{
		std::cerr << "Kernel size should be an odd number" << std::endl;
		return false;
	}

	if (ksize == 1)
	{
		out = in;
		return true;
	}

	int original_width = in.GetWidth();
	int original_height = in.GetHeight();

	Image in0 = in.MakeBorder(ksize / 2, Image::BorderType::REPLICATED);

	int processing_width = in0.GetWidth() - ksize / 2;
	int processing_height = in0.GetHeight() - ksize / 2;

	out = Image(original_width + ksize, original_height + ksize, in0.GetChannels());

	Histogram_C3 histogram(ksize);

	Image current_window, last_window;

	for (int y = ksize / 2; y < processing_height; y++)
	{
		histogram.Reset();
		for (int x = ksize / 2; x < processing_width; x++)
		{
			current_window = in0.Crop(x - ksize / 2, y - ksize / 2, ksize, ksize);

			if (x == ksize / 2)
				histogram.Calc(current_window);
			else
				histogram.Update(last_window, current_window);

			out.SetPixel(x, y, histogram.MedianColor());

			last_window = current_window;
		}
	}

	out = out.Crop(ksize / 2, ksize / 2, original_width, original_height);

	return true;
}