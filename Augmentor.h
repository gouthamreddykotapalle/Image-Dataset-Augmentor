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
        std::string out_path;
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
        explicit Augmentor(const std::string& in_path, const std::string& out_path);

        // Save current version of image into a file specified, with the default/specified quality (0-100)
        static void save(const std::string& fileName, Image* image, int quality = 95);

        // Resize the image - expand or shrink based on current width
        Augmentor& resize(image_size lower, image_size upper, double prob=1);

        Augmentor& resize(image_size new_size, double prob=1);

        Augmentor& resize(size_t lower_height, size_t lower_width, size_t upper_height, size_t upper_width, double prob=1);

        Augmentor& resize(size_t height, size_t width, double prob=1);

        Augmentor& crop(int height, int width, bool center, double prob=1);

        Augmentor& zoom(double min_factor=1.0, double max_factor=1.0, double prob=1);

        Augmentor& rotate(int min_degree, int max_degree, double prob=1);

        // Inverts the colors in the image
        Augmentor& invert(double prob=1);

        template<int K=5>
        Augmentor& blur(double sigma, double prob=1) {
            auto operation = std::make_unique<GaussianBlurOperation<Image, K>>(sigma, prob);
            operations.push_back(std::move(operation));
            return *this;
        }

        Augmentor& blur(double sigma, size_t kernel_size, double prob=1);

        Augmentor& rapid_blur(const double sigma, const unsigned int passes=3, double prob=1);

        Augmentor& pipeline();

        Augmentor& random_erase(image_size lower_mask_size, image_size upper_mask_size, double prob=1);

        Augmentor& random_erase(image_size mask_size, double prob=1);

        Augmentor& flip(const std::string& type, double prob=1);

        void sample(size_t size);
    };
}

#endif