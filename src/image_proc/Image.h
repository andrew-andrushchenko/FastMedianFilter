#pragma once

#include <stdint.h>

class Image
{
public:
    struct ColorRGB
    {
        uint8_t r, g, b;

        inline bool operator==(const ColorRGB& other)
        {
            return r == other.r && g == other.g && b == other.b;
        }
    };

    enum class ImageType
    {
        PNG, JPEG, BMP, UNKNOWN
    };

    enum class BorderType
    {
        CONSTANT,   // iiiiii|abcdefgh|iiiiiii  with some specified 'i'
        REPLICATED  // aaaaaa|abcdefgh|hhhhhhh
    };

    Image();
    Image(const char* filename);
    Image(int width, int height, int channels);
    Image(const Image& other);
    Image(Image&& other) noexcept;
    ~Image();

    bool WriteToFile(const char* filename);

    Image& operator=(Image&& other) noexcept;
    Image& operator=(const Image& other);

    inline int GetWidth() const { return m_Width; }
    inline int GetWidth() { return m_Width; }

    inline int GetHeight() const { return m_Height; }
    inline int GetHeight() { return m_Height; }

    inline int GetChannels() const { return m_Channels; }
    inline int GetChannels() { return m_Channels; }

    inline size_t GetSize() const { return m_Size; }
    inline size_t GetSize() { return m_Size; }

    inline bool IsEmpty() const { return m_Size == 0; }
    inline bool IsEmpty() { return m_Size == 0; }

    inline uint8_t* GetData() const { return m_Data; }
    inline uint8_t* GetData() { return m_Data; }

    Image Crop(uint16_t cx, uint16_t cy, uint16_t cropped_width, uint16_t cropped_height) const;
    Image Crop(uint16_t cx, uint16_t cy, uint16_t cropped_width, uint16_t cropped_height);

    Image MakeBorder(uint16_t padding, BorderType border_type, ColorRGB border_color = {0, 0, 0}) const;
    Image MakeBorder(uint16_t padding, BorderType border_type, ColorRGB border_color = {0, 0, 0});

    void SetPixel(int x, int y, ColorRGB color);

private:
    int m_Width, m_Height, m_Channels;
    size_t m_Size;
    uint8_t* m_Data;

    ImageType _GetFileType(const char* filename);
};