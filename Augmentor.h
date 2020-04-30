#ifndef LIB_AUGMENTOR_H
#define LIB_AUGMENTOR_H

#include "jpeg.h"
#include "Operation.h"
#include <iostream>
#include <string>
#include <stdexcept>
#include <vector>
#include <memory>

using namespace jpegimageSTL::jpeg;

namespace augmentorLib {
    /// This is the Augmentor Class.
    ///
    /// This is the main class of the library, an instance of which the user would create for sampling images
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
        /// Default Constructor.
        Augmentor() = default;

        ~Augmentor() = default;

        /// Load an image file. Currently only accepts RGB colorspace images.
        /// @param in_path - Input directory path
        /// @param out_path - Output directory path
        explicit Augmentor(const std::string& in_path, const std::string& out_path);

        /// Save current version of image into a file specified, with the default/specified quality (0-100)
        /// @param fileName - Name of the augmented output file
        /// @param image - an image of type Image
        /// @param quality - quality of the output image
        /// @returns A reference to the Augmentor object
        static void save(const std::string& fileName, Image* image, int quality = 95);

        /// Resize the image
        ///
        /// expand or shrink based on a size selected in random from the range specified
        /// @param lower - lower bound on the dimensions of resize
        /// @param upper - upper bound on the dimensions of resize
        /// @param prob - probability of performing the resize operation
        /// @returns A reference to the Augmentor object
        /// @see resize(image_size new_size, double prob=1) resize(size_t lower_height, size_t lower_width, size_t upper_height, size_t upper_width, double prob=1) resize(size_t height, size_t width, double prob=1)
        Augmentor& resize(image_size lower, image_size upper, double prob=1);

        /// Resize the image
        ///
        /// expand or shrink based on current width
        /// @param new_size - new size of the augmented image
        /// @param prob - probability of performing the resize operation
        /// @returns A reference to the Augmentor object
        Augmentor& resize(image_size new_size, double prob=1);

        /// Resize the image
        ///
        /// expand or shrink based on a size selected in random from the range specified
        /// @param lower_height - min height for the range
        /// @param lower_width - min width of the range
        /// @param upper_height - max height of the range
        /// @param upper_width - max width of the range
        /// @param prob - probability of performing the resize operation
        /// @returns A reference to the Augmentor object
        Augmentor& resize(size_t lower_height, size_t lower_width, size_t upper_height, size_t upper_width, double prob=1);

        /// Resize the image
        ///
        /// expand or shrink based on current width
        /// @param new_size - new size of the augmented image
        /// @param prob - probability of performing the resize operation
        /// @returns A reference to the Augmentor object
        Augmentor& resize(size_t height, size_t width, double prob=1);

        /// Crop the image
        ///
        /// Crop based on current width and height
        /// @param height - new height of the augmented image
        /// @param width - new width of the augmented image
        /// @param center - boolean value to crop by using the center as pivot or not
        /// @param prob - probability of performing the resize operation
        /// @returns A reference to the Augmentor object
        Augmentor& crop(int height, int width, bool center, double prob=1);

        /// Zoom the image
        ///
        /// Zoom based on current width and height based on a factor selected in random from the range specified
        /// @param min_factor - minimum zoom factor of range
        /// @param max_factor - maximum zoom factor of range
        /// @param prob - probability of performing the resize operation
        /// @returns A reference to the Augmentor object
        Augmentor& zoom(double min_factor=1.0, double max_factor=1.0, double prob=1);

        /// Rotate the image
        ///
        /// Rotate based on current width and height based on a angle selected in random from the range specified
        /// @param min_factor - minimum zoom factor of range
        /// @param max_factor - maximum zoom factor of range
        /// @param prob - probability of performing the resize operation
        /// @returns A reference to the Augmentor object
        Augmentor& rotate(int min_degree, int max_degree, double prob=1);

        /// Invert the image
        /// Inverts the colors in the image
        /// \param prob probability of performing the resize operation
        /// \return A reference to the Augmentor object
        Augmentor& invert(double prob=1);

        /// Blur
        ///
        /// Blurs the image based on the sigma value
        /// \tparam K Kernel value for Gaussian distribution
        /// \param sigma Sigma value for Gaussian distribution
        /// \param prob probability of performing the resize operation
        /// \return A reference to the Augmentor object
        template<int K=5>
        Augmentor& blur(double sigma, double prob=1) {
            auto operation = std::make_unique<GaussianBlurOperation<Image, K>>(sigma, prob);
            operations.push_back(std::move(operation));
            return *this;
        }

        /// Blur
        ///
        /// Blurs the image based on the sigma value
        /// \param sigma Sigma value for Gaussian distribution
        /// \param kernel_size Kernel size for Gaussian distribution
        /// \param prob probability of performing the resize operation
        /// \return A reference to the Augmentor object
        Augmentor& blur(double sigma, size_t kernel_size, double prob=1);

        Augmentor& rapid_blur(const double sigma, const unsigned int passes=3, double prob=1);

        /// Pipeline
        /// Creates an input image array to operate on
        /// \return A reference to the Augmentor object
        Augmentor& pipeline();

        /// Random erase
        ///
        /// Randomly erase a part of the image based on a mask size selected in random from the range specified
        /// \param lower_mask_size lower mask size used as { height, width }
        /// \param upper_mask_size upper mask size used as { height, width }
        /// \param prob probability of performing the resize operation
        /// \attention pixels are lost in the output image
        /// \return A reference to the Augmentor object
        Augmentor& random_erase(image_size lower_mask_size, image_size upper_mask_size, double prob=1);

        /// Random erase
        ///
        /// Randomly erase a part of the image based on a specified mask size
        /// \param mask_size mask size used as { height, width }
        /// \param prob probability of performing the resize operation
        /// \attention pixels are lost in the output image
        /// \return A reference to the Augmentor object
        Augmentor& random_erase(image_size mask_size, double prob=1);

        /// Flip
        ///
        /// Flips the image either vertically or horozontally
        /// \param type Takes any of the two options - VERTICAL or HORIZONTAL
        /// \param prob probability of performing the resize operation
        /// \return A reference to the Augmentor object
        Augmentor& flip(const std::string& type, double prob=1);

        /// Sample
        ///
        /// creates the specifed number of augmented images
        /// \param size number of augmented images to specify
        void sample(size_t size);
    };
}

#endif