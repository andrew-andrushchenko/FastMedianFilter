#define STBI_NO_PSD
#define STBI_NO_TGA
#define STBI_NO_HDR
#define STBI_NO_PIC
#define STBI_NO_PNM
#define STBI_NO_GIF

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "Image.h"
#include <iostream>
#include <string.h>

#include "../vendor/stb_image/stb_image.h"
#include "../vendor/stb_image/stb_image_write.h"

Image::Image() : m_Width(0), m_Height(0), m_Channels(0), m_Size(0), m_Data(nullptr)
{
}

Image::Image(const char* filename) : m_Width(0), m_Height(0), m_Channels(0)
{
    m_Data = stbi_load(filename, &m_Width, &m_Height, &m_Channels, 0);
    if (m_Data) 
    {
        m_Size = (uint64_t) m_Width * m_Height * m_Channels;
    }
    else
    {
        std::cerr << "LOAD ERROR: Failed to read image." << filename << std::endl;
        m_Size = 0;
    }
}

Image::Image(int width, int height, int channels)
    : m_Width(width), m_Height(height), m_Channels(channels)
{
    m_Size = (uint64_t) m_Width * m_Height * m_Channels;
    m_Data = new uint8_t[m_Size];
    memset(m_Data, 0, m_Size);
}

Image::Image(const Image& other)
{
    m_Width = other.m_Width;
    m_Height = other.m_Height;
    m_Channels = other.m_Channels;
    m_Size = other.m_Size;
    memcpy(m_Data, other.m_Data, m_Size);
}

Image::Image(Image&& other) noexcept : m_Data(nullptr)
{
    m_Width = other.m_Width;
    m_Height = other.m_Height;
    m_Channels = other.m_Channels;
    m_Size = other.m_Size;
    m_Data = other.m_Data;

    other.m_Data = nullptr;
}

Image& Image::operator=(Image&& other) noexcept
{
    if (this != &other)
    {
        delete[] m_Data;

        m_Width = other.m_Width;
        m_Height = other.m_Height;
        m_Channels = other.m_Channels;
        m_Size = other.m_Size;
        m_Data = other.m_Data;

        other.m_Data = nullptr;
    }

    return *this;
}

Image& Image::operator=(const Image& other)
{
    m_Width = other.m_Width;
    m_Height = other.m_Height;
    m_Channels = other.m_Channels;
    m_Size = other.m_Size;

    delete[] m_Data;
    m_Data = new uint8_t[m_Size];
    memcpy(m_Data, other.m_Data, m_Size);

    return *this;
}

Image::~Image()
{
    delete[] m_Data;
}

bool Image::WriteToFile(const char* filename)
{
    uint8_t is_write_successful;
    ImageType image_type = _GetFileType(filename);
    switch (image_type)
    {
        case ImageType::PNG:
            is_write_successful = stbi_write_png(filename, m_Width, m_Height, m_Channels, m_Data, m_Width * m_Channels);
            break;
        case ImageType::JPEG:
            is_write_successful = stbi_write_jpg(filename, m_Width, m_Height, m_Channels, m_Data, 100);
            break;
        case ImageType::BMP:
            is_write_successful = stbi_write_bmp(filename, m_Width, m_Height, m_Channels, m_Data);
            break;
        case ImageType::UNKNOWN:
        default:
            is_write_successful = 0;
            break;
        }
    return is_write_successful != 0;
}

Image Image::Crop(uint16_t cx, uint16_t cy, uint16_t cropped_width, uint16_t cropped_height) const
{
    Image out(cropped_width, cropped_height, this->m_Channels);

    for (uint16_t y = 0; y < cropped_height; y++)
    {
        if (y + cy >= this->m_Height) break; 
        for (uint16_t x = 0; x < cropped_width; x++)
        {
            if (x + cx >= this->m_Width) break;
            memcpy(&out.m_Data[(x + y * cropped_width) * out.m_Channels], &this->m_Data[(x + cx + (y + cy) * this->m_Width) * this->m_Channels], this->m_Channels);
        }
    }

    return out;
}

Image Image::Crop(uint16_t cx, uint16_t cy, uint16_t cropped_width, uint16_t cropped_height)
{
    Image out(cropped_width, cropped_height, this->m_Channels);

    for (uint16_t y = 0; y < cropped_height; y++) 
    {
        if (y + cy >= this->m_Height) break;
        for (uint16_t x = 0; x < cropped_width; x++)
        {
            if (x + cx >= this->m_Width) break;
            memcpy(&out.m_Data[(x + y * cropped_width) * out.m_Channels], &this->m_Data[(x + cx + (y + cy) * this->m_Width) * this->m_Channels], this->m_Channels);
        }
    }

    return out;
}

Image Image::MakeBorder(uint16_t padding, BorderType border_type, ColorRGB border_color) const
{
    const int target_width = this->m_Width + padding * 2;
    const int target_height = this->m_Height + padding * 2;

    Image out(target_width, target_height, this->m_Channels);
    // Copy image in the center of the new frame
    for (int y = 0; y < this->m_Height; y++)
    {
        for (int x = 0; x < this->m_Width; x++)
        {
            memcpy(&out.m_Data[(x + padding + (y + padding) * out.m_Width) * out.m_Channels], &this->m_Data[(x + y * this->m_Width) * this->m_Channels], this->m_Channels);
        }
    }

    switch (border_type)
    {
        case BorderType::CONSTANT:
            // No need to fill the border with black color, it is already black 
            if (border_color == ColorRGB{0, 0, 0}) break;

            // Fill upper border with border color
            for (int y = 0; y < padding; y++)
            {
                for (int x = 0; x < out.m_Width; x++)
                {
                    out.m_Data[ ( x + y * out.m_Width) * out.m_Channels ]    = border_color.r;
                    out.m_Data[ ( x + y * out.m_Width) * out.m_Channels + 1] = border_color.g;
                    out.m_Data[ ( x + y * out.m_Width) * out.m_Channels + 2] = border_color.b;
                }
            }

            // Fill lower border with border color
            for (int y = out.m_Height - padding; y < out.m_Height; y++)
            {
                for (int x = 0; x < out.m_Width; x++)
                {
                    out.m_Data[ ( x + y * out.m_Width) * out.m_Channels ]    = border_color.r;
                    out.m_Data[ ( x + y * out.m_Width) * out.m_Channels + 1] = border_color.g;
                    out.m_Data[ ( x + y * out.m_Width) * out.m_Channels + 2] = border_color.b;
                }
            }

            // Fill left border with border color
            for (int y = padding; y < out.m_Height - padding; y++)
            {
                for (int x = 0; x < padding; x++)
                {
                    out.m_Data[ ( x + y * out.m_Width) * out.m_Channels ]    = border_color.r;
                    out.m_Data[ ( x + y * out.m_Width) * out.m_Channels + 1] = border_color.g;
                    out.m_Data[ ( x + y * out.m_Width) * out.m_Channels + 2] = border_color.b;
                }
            }

            // Fill right border with border color
            for (int y = padding; y < out.m_Height - padding; y++)
            {
                for (int x = out.m_Width - padding; x < out.m_Width; x++)
                {
                    out.m_Data[ ( x + y * out.m_Width) * out.m_Channels ]    = border_color.r;
                    out.m_Data[ ( x + y * out.m_Width) * out.m_Channels + 1] = border_color.g;
                    out.m_Data[ ( x + y * out.m_Width) * out.m_Channels + 2] = border_color.b;
                }
            }

            break;
        case BorderType::REPLICATED:
            // Replicate original image's left column to all left columns of the new image
            for (int column = 0; column < padding; column++)
            {
                for (int y = 0; y < this->m_Height; y++)
                {
                    memcpy(&out.m_Data[(column + (y + padding) * out.m_Width) * out.m_Channels], &this->m_Data[(0 + y * this->m_Width) * this->m_Channels], this->m_Channels);
                }
            }
            // Replicate original image's right column to all right columns of new the image
            for (int column = out.m_Width - padding; column < out.m_Width; column++)
            {
                for (int y = 0; y < this->m_Height; y++)
                {
                    memcpy(&out.m_Data[(column + (y + padding) * out.m_Width) * out.m_Channels], &this->m_Data[((this->m_Width - 1) + y * this->m_Width) * this->m_Channels], this->m_Channels);
                }
            }

            // Replicate original image's top row to all top rows of new the image
            for (int row = 0; row < padding; row++)
            {
                for (int x = 0; x < this->m_Width; x++)
                {
                    memcpy(&out.m_Data[((x + padding) + row * out.m_Width) * out.m_Channels], &this->m_Data[x * this->m_Channels], this->m_Channels);
                }
            }

            // Replicate original image's bottom row to all bottom rows of new the image
            for (int row = out.m_Height - padding; row < out.m_Height; row++)
            {
                for (int x = 0; x < this->m_Width; x++)
                {
                    memcpy(&out.m_Data[((x + padding) + row * out.m_Width) * out.m_Channels], &this->m_Data[(x + (this->m_Height - 1) * this->m_Width) * this->m_Channels], this->m_Channels);
                }
            }

            // Replicate top left corner
            for (int row = 0; row < padding; row++)
            {
                for (int col = 0; col < padding; col++)
                {
                    memcpy(&out.m_Data[(col + row * out.m_Width) * out.m_Channels], &this->m_Data[0], this->m_Channels);
                }
            }

            // Replicate top right corner
            for (int row = 0; row < padding; row++)
            {
                for (int col = out.m_Width - padding; col < out.m_Width; col++)
                {
                    memcpy(&out.m_Data[(col + row * out.m_Width) * out.m_Channels], &this->m_Data[(this->m_Width - 1) * this->m_Channels], this->m_Channels);
                }
            }

            // Replicate bottom left corner
            for (int row = out.m_Height - padding; row < out.m_Height; row++)
            {
                for (int col = 0; col < padding; col++)
                {
                    memcpy(&out.m_Data[(col + row * out.m_Width) * out.m_Channels], &this->m_Data[((this->m_Height - 1) * this->m_Width) * this->m_Channels], this->m_Channels);
                }
            }

            // Replicate bottom right corner
            for (int row = out.m_Height - padding; row < out.m_Height; row++)
            {
                for (int col = out.m_Width - padding; col < out.m_Width; col++)
                {
                    memcpy(&out.m_Data[(col + row * out.m_Width) * out.m_Channels], &this->m_Data[(this->m_Width - 1 + (this->m_Height - 1) * this->m_Width) * this->m_Channels], this->m_Channels);
                }
            }
            break;
    }

    return out;
}

Image Image::MakeBorder(uint16_t padding, BorderType border_type, ColorRGB border_color)
{
    const int target_width = this->m_Width + padding * 2;
    const int target_height = this->m_Height + padding * 2;

    Image out(target_width, target_height, this->m_Channels);
    // Copy image in the center of the new frame
    for (int y = 0; y < this->m_Height; y++) 
    {
        for (int x = 0; x < this->m_Width; x++) {
            memcpy(&out.m_Data[(x + padding + (y + padding) * out.m_Width) * out.m_Channels], &this->m_Data[(x + y * this->m_Width) * this->m_Channels], this->m_Channels);
        }
    }

    switch (border_type)
    {
        case BorderType::CONSTANT:
            // No need to fill the border with black color, it is already black 
            if (border_color == ColorRGB{0, 0, 0}) break;

            // Fill upper border with border color
            for (int y = 0; y < padding; y++)
            {
                for (int x = 0; x < out.m_Width; x++)
                {
                    out.m_Data[ ( x + y * out.m_Width) * out.m_Channels ]    = border_color.r;
                    out.m_Data[ ( x + y * out.m_Width) * out.m_Channels + 1] = border_color.g;
                    out.m_Data[ ( x + y * out.m_Width) * out.m_Channels + 2] = border_color.b;
                }
            }

            // Fill lower border with border color
            for (int y = out.m_Height - padding; y < out.m_Height; y++)
            {
                for (int x = 0; x < out.m_Width; x++)
                {
                    out.m_Data[ ( x + y * out.m_Width) * out.m_Channels ]    = border_color.r;
                    out.m_Data[ ( x + y * out.m_Width) * out.m_Channels + 1] = border_color.g;
                    out.m_Data[ ( x + y * out.m_Width) * out.m_Channels + 2] = border_color.b;
                }
            }

            // Fill left border with border color
            for (int y = padding; y < out.m_Height - padding; y++)
            {
                for (int x = 0; x < padding; x++)
                {
                    out.m_Data[ ( x + y * out.m_Width) * out.m_Channels ]    = border_color.r;
                    out.m_Data[ ( x + y * out.m_Width) * out.m_Channels + 1] = border_color.g;
                    out.m_Data[ ( x + y * out.m_Width) * out.m_Channels + 2] = border_color.b;
                }
            }

            // Fill right border with border color
            for (int y = padding; y < out.m_Height - padding; y++)
            {
                for (int x = out.m_Width - padding; x < out.m_Width; x++)
                {
                    out.m_Data[ ( x + y * out.m_Width) * out.m_Channels ]    = border_color.r;
                    out.m_Data[ ( x + y * out.m_Width) * out.m_Channels + 1] = border_color.g;
                    out.m_Data[ ( x + y * out.m_Width) * out.m_Channels + 2] = border_color.b;
                }
            }

            break;
        case BorderType::REPLICATED:
            // Replicate original image's left column to all left columns of the new image
            for (int column = 0; column < padding; column++)
            {
                for (int y = 0; y < this->m_Height; y++)
                {
                    memcpy(&out.m_Data[(column + (y + padding) * out.m_Width) * out.m_Channels], &this->m_Data[(0 + y * this->m_Width) * this->m_Channels], this->m_Channels);
                }
            }
            // Replicate original image's right column to all right columns of new the image
            for (int column = out.m_Width - padding; column < out.m_Width; column++)
            {
                for (int y = 0; y < this->m_Height; y++)
                {
                    memcpy(&out.m_Data[(column + (y + padding) * out.m_Width) * out.m_Channels], &this->m_Data[((this->m_Width - 1) + y * this->m_Width) * this->m_Channels], this->m_Channels);
                }
            }

            // Replicate original image's top row to all top rows of new the image
            for (int row = 0; row < padding; row++)
            {
                for (int x = 0; x < this->m_Width; x++)
                {
                    memcpy(&out.m_Data[((x + padding) + row * out.m_Width) * out.m_Channels], &this->m_Data[x * this->m_Channels], this->m_Channels);
                }
            }

            // Replicate original image's bottom row to all bottom rows of new the image
            for (int row = out.m_Height - padding; row < out.m_Height; row++)
            {
                for (int x = 0; x < this->m_Width; x++)
                {
                    memcpy(&out.m_Data[((x + padding) + row * out.m_Width) * out.m_Channels], &this->m_Data[(x + (this->m_Height - 1) * this->m_Width) * this->m_Channels], this->m_Channels);
                }
            }

            // Replicate top left corner
            for (int row = 0; row < padding; row++)
            {
                for (int col = 0; col < padding; col++)
                {
                    memcpy(&out.m_Data[(col + row * out.m_Width) * out.m_Channels], &this->m_Data[0], this->m_Channels);
                }
            }

            // Replicate top right corner
            for (int row = 0; row < padding; row++)
            {
                for (int col = out.m_Width - padding; col < out.m_Width; col++)
                {
                    memcpy(&out.m_Data[(col + row * out.m_Width) * out.m_Channels], &this->m_Data[(this->m_Width - 1) * this->m_Channels], this->m_Channels);
                }
            }

            // Replicate bottom left corner
            for (int row = out.m_Height - padding; row < out.m_Height; row++)
            {
                for (int col = 0; col < padding; col++)
                {
                    memcpy(&out.m_Data[(col + row * out.m_Width) * out.m_Channels], &this->m_Data[((this->m_Height - 1) * this->m_Width) * this->m_Channels], this->m_Channels);
                }
            }

            // Replicate bottom right corner
            for (int row = out.m_Height - padding; row < out.m_Height; row++)
            {
                for (int col = out.m_Width - padding; col < out.m_Width; col++)
                {
                    memcpy(&out.m_Data[(col + row * out.m_Width) * out.m_Channels], &this->m_Data[(this->m_Width - 1 + (this->m_Height - 1) * this->m_Width) * this->m_Channels], this->m_Channels);
                }
            }
            break;
    }

    return out;
}

void Image::SetPixel(int x, int y, ColorRGB color)
{
    if (m_Channels != 3) return;
    if (m_Data != nullptr)
    {
        m_Data[(x + y * m_Width) * m_Channels]     = color.r;
        m_Data[(x + y * m_Width) * m_Channels + 1] = color.g;
        m_Data[(x + y * m_Width) * m_Channels + 2] = color.b;
    }
}

Image::ImageType Image::_GetFileType(const char* filename)
{
    const char* ext = strrchr(filename, '.');
    if (ext)
    {
        if (strcmp(ext, ".png") == 0) return ImageType::PNG;
        else if (strcmp(ext, ".jpg") == 0 || strcmp(ext, ".jpeg") == 0) return ImageType::JPEG;
        else if (strcmp(ext, ".bmp") == 0) return ImageType::BMP;
    }
    return ImageType::UNKNOWN;
}