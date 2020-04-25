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
        //dir dir_path
        std::string dir_path;
        // Provided by the jpeg.h file. Representation of an decompressed image file.
        std::vector<std::string> image_paths;
        std::vector<std::string> output_array;
        // unique points for base classes
        std::vector<std::unique_ptr< Operation<Image> >> operations;
    public:
        // Default Constructor.
        Augmentor() = default;

        ~Augmentor() = default;

        // Load an image file. Currently only accepts RGB colorspace images.
        explicit Augmentor(const std::string& path);

        // Save current version of image into a file specified, with the default/specified quality (0-100)
        static void save(const std::string& fileName, Image* image, int quality = 95);

        // Resize the image - expand or shrink based on current width
        Augmentor& resize(image_size lower, image_size upper, double prob=1);

        Augmentor& resize(image_size new_size, double prob=1);

        Augmentor& resize(int lower_height, int lower_width, int upper_height, int upper_width, double prob=1);

        Augmentor& resize(int height, int width, double prob=1);

        // Inverts the colors in the image
        Augmentor& invert(double prob=1);

        Augmentor& pipeline(const std::string& directory_path);

        void sample(size_t size);
    };
}

#endif