#pragma once

#include "../image_proc/Image.h"

class Histogram_C3
{
public:
	Histogram_C3(int ksize);

	void Reset();

	void Calc(const Image& window);

	void Update(const Image& last_window, const Image& new_window);

	Image::ColorRGB MedianColor();

private:
	uint8_t r[256], g[256], b[256];

	int m_Ksize;
	// Window is a square neighbourhood of a pixel
	int m_HalfWindowSize;
	
	uint8_t Median(const uint8_t* channel);

};

bool MedianSmooth(const Image& in, Image& out, int ksize);
