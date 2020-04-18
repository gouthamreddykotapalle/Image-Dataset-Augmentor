#ifndef LIB_AUGMENTOR_H
#define LIB_AUGMENTOR_H

#include "jpeg.h"
#include "Operation.h"
#include <iostream>
#include <string>
#include <stdexcept>
#include <vector>
#include <memory>

using namespace marengo::jpeg;

namespace augmentorLib {

    class Augmentor {
        // Provided by the jpeg.h file. Representation of an decompressed image file.
        Image img;
        // unique points for base classes
        std::vector<std::unique_ptr< Operation<Image> >> operations;
    public:
        // Default Constructor.
        Augmentor() = default;

        ~Augmentor() = default;

        // Load an image file. Currently only accepts RGB colorspace images.
        explicit Augmentor(char *filename);

        // Save current version of image into a file specified, with the default/specified quality (0-100)
        Augmentor& save(const std::string& fileName, int quality = 95, double prob=1);

        // Resize the image - expand or shrink based on current width
        Augmentor& resize(int newHeight, int newWidth, double prob=1);

        // Inverts the colors in the image
        Augmentor& invert(double prob=1);

        void run();
    };
}

#endif