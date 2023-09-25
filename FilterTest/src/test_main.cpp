#include "gtest/gtest.h"
#include "medianf.h"
#include <algorithm>

TEST(HistogramTest, ReturnsCorrectMedianValueFromGivenImage)
{
    Image image(3, 3, 3);
    
    uint8_t values[] = { 4, 12, 4, 3, 6, 8, 3, 4, 1};
    const size_t values_size = sizeof(values) / sizeof(uint8_t);

    for (int y = 0; y < image.GetHeight(); y++)
    {
        for (int x = 0; x < image.GetWidth(); x++)
        {
            uint8_t color_value = values[x + y * image.GetWidth()];
            image.SetPixel(x, y, { color_value, color_value, color_value });
        }
    }

    Histogram_C3 histogram(3);

    histogram.Reset();
    histogram.Calc(image);
    
    auto median_color = histogram.MedianColor();

    std::sort(values, values + values_size);
    
    EXPECT_EQ(median_color.r, values[values_size / 2]);
    EXPECT_EQ(median_color.g, values[values_size / 2]);
    EXPECT_EQ(median_color.b, values[values_size / 2]);
}