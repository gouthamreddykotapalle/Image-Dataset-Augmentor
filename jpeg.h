#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

// forward declarations of jpeglib struct
struct jpeg_error_mgr;

namespace jpegimageSTL::jpeg
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
            typedef uint8_t pixel_value_type;

            ///Image constructor
            ///
            /// \param x width of the image
            /// \param y height of the image
            /// \param pixelSize size of a single pixel
            /// \param colourSpace Type of the colourSpace - Currently only supports RGB
            /// @see Image() Image( const std::string& fileName )
            explicit Image(
                    size_t x,
                    size_t y,
                    size_t pixelSize = 3,
                    int colourSpace = 2 //const does not have an effect in the function declaration
            );


            /// Image constructor
            ///
            /// Construct with an existing file. Will throw if file cannot be loaded, or is in the wrong format or some other error is encountered.
            /// \param fileName path to the input file
            explicit Image( const std::string& fileName );

            /// copy constructor
            ///
            /// We can construct from an existing image object. This allows us to work on a copy (e.g. shrink then save) without affecting the original we have in memory.
            /// \param rhs Source image object
            Image( const Image& rhs );

            ~Image();

            Image();

            /// Save
            ///
            /// saves the image to the path specified
            /// \param fileName output path to save the image to
            /// \param quality quality of the image to save
            /// @note Will throw if file cannot be saved. Quality's usable values are 0-100
            void save( const std::string& fileName, int quality = 95 ) const;

            [[nodiscard]] size_t getHeight()    const { return m_height; }
            [[nodiscard]] size_t getWidth()     const { return m_width;  }
            [[nodiscard]] size_t getPixelSize() const { return m_pixelSize; }
            [[nodiscard]] int getColorSpace() const { return m_colourSpace; }

            /// GetPixel
            ///
            /// Will return a vector of pixel components. The vector's size will be 1 for monochrome or 3 for RGB. Elements for the latter will be in order R, G, B.
            /// \param x x coordinate of the pizel
            /// \param y y cooridinate of the pixel
            /// \return A pixelvalue: vector of characters that make that pixel
            [[nodiscard]] std::vector<uint8_t> getPixel( size_t x, size_t y ) const;

            /// Set Pixel
            ///
            /// Used to set one pixel's value at the x,y coordinate. Takes in a vector of subpixels.
            /// \param x x coordinate of the pizel
            /// \param y y cooridinate of the pixel
            /// \param pixelValue An RGB vector of characters [R, G, B] that make that pixel
            void setPixel(size_t x, size_t y, std::vector<uint8_t> pixelValue);

            // Convenience function to resize image using height, width
            void resize( size_t newHeight, size_t newWidth );

        };

    }
