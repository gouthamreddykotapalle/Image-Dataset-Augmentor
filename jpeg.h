#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

// forward declarations of jpeglib struct
struct jpeg_error_mgr;

namespace marengo
{
    namespace jpeg
    {

        class Image
        {
        private:
            // Note that m_errorMgr is a shared ptr and will be shared
            // between objects if one copy constructs from another
            std::shared_ptr< ::jpeg_error_mgr > m_errorMgr;
            std::vector< std::vector<uint8_t> > m_bitmapData;
            size_t                            m_width;
            size_t                            m_height;
            size_t                            m_pixelSize;
            int                               m_colourSpace;

        public:
            explicit Image(
                    const size_t x,
                    const size_t y,
                    const size_t pixelSize = 3,
                    const int colourSpace = 2
            );

            // Construct with an existing file.
            // Will throw if file cannot be loaded, or is in the wrong format,
            // or some other error is encountered.
            explicit Image( const std::string& fileName );

            // We can construct from an existing image object. This allows us
            // to work on a copy (e.g. shrink then save) without affecting the
            // original we have in memory.
            Image( const Image& rhs );

            ~Image();

            Image();

            // Will throw if file cannot be saved.
            // Quality's usable values are 0-100
            void save( const std::string& fileName, int quality = 95 ) const;

            // Mainly for testing, writes an uncompressed PPM file
            void savePpm( const std::string& fileName ) const;

            size_t getHeight()    const { return m_height; }
            size_t getWidth()     const { return m_width;  }
            size_t getPixelSize() const { return m_pixelSize; }

            // Will return a vector of pixel components. The vector's
            // size will be 1 for monochrome or 3 for RGB.
            // Elements for the latter will be in order R, G, B.
            std::vector<uint8_t> getPixel( size_t x, size_t y ) const;

            // Used to set one pixel's value at the x,y coordinate. Takes
            // in a vector of subpixels.
            void setPixel(size_t x, size_t y, std::vector<uint8_t> pixelValue);

            // Convenience function to resize image using height, width
            void resize( size_t newHeight, size_t newWidth );

        };

    } // namespace jpeg
} // namespace marengo
