#ifndef LIB_AUGMENTOR_H
#define LIB_AUGMENTOR_H

#include "jpeg.h"
#include <iostream>
#include <string>
#include <stdexcept>

using namespace marengo::jpeg;

namespace augmentorLib {

    class Augmentor {
        // Provided by the jpeg.h file. Representation of an decompressed image file.
        Image output;

    public:
        // Default Constructor.
        Augmentor(){}

        ~Augmentor(){}

        // Load an image file. Currently only accepts RGB colorspace images.
        Augmentor(char *filename);

        // Save current version of image into a file specified, with the default/specified quality (0-100)
        Augmentor& save(const std::string& fileName, int quality = 95);

        // Resize the image - expand or shrink based on current width
        Augmentor& resize(int newHeight, int newWidth);

        // Inverts the colors in the image
        Augmentor& invert();

    };
}

#endif