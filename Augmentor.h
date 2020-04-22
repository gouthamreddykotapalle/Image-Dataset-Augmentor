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
        std::vector<std::string> imgs;
        std::vector<std::string> output_array;
        // unique points for base classes
        std::vector<std::unique_ptr< Operation<Image> >> operations;
    public:
        //dir path
        std::string path;
        // Default Constructor.
        Augmentor() = default;

        ~Augmentor() = default;

        // Load an image file. Currently only accepts RGB colorspace images.
        explicit Augmentor(const std::string& path);

        // Save current version of image into a file specified, with the default/specified quality (0-100)
        Augmentor& save(const std::string& fileName, Image* image, int quality = 95, double prob=1);

        // Resize the image - expand or shrink based on current width
        Augmentor& resize(int newHeight, int newWidth, double prob=1);

        // Inverts the colors in the image
        Augmentor& invert(double prob=1);

        Augmentor& pipeline(const std::string& directory_path);

        void sample(size_t size);
    };
}

#endif