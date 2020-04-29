//
// Created by Augustin Pan on 4/15/20.
//

#ifndef LIB_OPERATION_H
#define LIB_OPERATION_H

#include <random>
#include <chrono>
#include <utility>
#include <iostream>
#include "gaussian_filter.h"
#include <algorithm>
#include <vector>
#include <limits>
#include <type_traits>


namespace augmentorLib {

    const double LOWER_BOUND_PROB = 0.0;
    const double UPPER_BOUND_PROB = 1.0;
    const unsigned NULL_SEED = 0;

    /// A class to generate random numbers
    /// It has different implementations based on the datatype. It uses `uniform_real_distribution`
    /// to generate random numbers of floating numbers, and `uniform_int_distribution` to generate integers.
    ///
    template <typename DataType, bool IsReal = std::is_floating_point<DataType>::value>
    class UniformDistributionGenerator;

    ///
    /// A class to generate floating numbers
    ///
    template <typename DataType>
    class UniformDistributionGenerator<DataType, true> {
    private:
        std::default_random_engine generator;
        std::uniform_real_distribution<DataType> distribution;

        static unsigned init_seed(unsigned seed) {
            if (seed == NULL_SEED) {
                return std::chrono::system_clock::now().time_since_epoch().count();
            }
            return seed;
        }

    public:
        UniformDistributionGenerator(): UniformDistributionGenerator<DataType>(NULL_SEED) {};

        ~UniformDistributionGenerator() = default;

        ///
        /// if seed = 0, the program will automatically generate a seed based on current time.
        /// The range of random numbers is from 0 to 1.
        ///
        explicit UniformDistributionGenerator(unsigned seed):
                generator{init_seed(seed)},
                distribution{LOWER_BOUND_PROB, UPPER_BOUND_PROB} {}

        explicit UniformDistributionGenerator(unsigned seed, DataType lower, DataType upper):
                generator{init_seed(seed)},
                distribution{lower, upper} {}

        inline DataType operator()() {
            return distribution(generator);
        }
    };

    /// A class to generate int numbers
    /// Int numbers include from int8 to unsigned long long.
    ///
    template <typename DataType>
    class UniformDistributionGenerator<DataType, false> {
    private:
        std::default_random_engine generator;
        std::uniform_int_distribution<DataType> distribution;

        static unsigned init_seed(unsigned seed) {
            if (seed == NULL_SEED) {
                return std::chrono::system_clock::now().time_since_epoch().count();
            }
            return seed;
        }

    public:
        UniformDistributionGenerator(): UniformDistributionGenerator<DataType>(NULL_SEED) {};

        ~UniformDistributionGenerator() = default;

        ///
        /// if seed = 0, the program will automatically generate a seed based on current time.
        /// The range of random numbers from Datatype_min to Datatype_max
        ///
        explicit UniformDistributionGenerator(unsigned seed):
                generator{init_seed(seed)},
                distribution{std::numeric_limits<DataType>::min(), std::numeric_limits<DataType>::max()} {}

        ///
        /// if seed = 0, the program will automatically generate a seed based on current time.
        ///
        explicit UniformDistributionGenerator(unsigned seed, DataType lower, DataType upper):
                generator{init_seed(seed)},
                distribution{lower, upper} {}

        inline DataType operator()() {
            return distribution(generator);
        }
    };

    //TODO: use concept to constrain the value type to images
    template<typename Image>
    class Operation {
    private:
        typedef double _precision_type;
        double probability;
        UniformDistributionGenerator<_precision_type> generator;

    protected:
        inline bool operate_this_time() {
            return generator() <= probability;
        }

        inline _precision_type uniform_random_number() {
            return generator();
        }

        inline _precision_type uniform_random_number(const _precision_type lower, const  _precision_type upper) {
            return (upper - lower) * generator() + lower;
        }

    public:

        Operation(): probability{UPPER_BOUND_PROB}, generator{NULL_SEED} {};

        virtual ~Operation() = default;

        explicit Operation(double prob, unsigned seed = NULL_SEED): probability{prob}, generator{seed} {}

        template <typename Container>
        Container&& perform(Container&&);

        // use pointer here, because we can use nullptr to indicate the Operation did not occur.
        virtual Image* perform(Image* image) = 0;
    };


    template<typename Image>
    class StdoutOperation: public Operation<Image> {
    private:
        std::string str;
    public:
        StdoutOperation(): str{""}, Operation<Image>{} {};

        explicit StdoutOperation(std::string s, double prob = UPPER_BOUND_PROB, unsigned seed = NULL_SEED):
                Operation<Image>{prob, seed}, str{std::move(s)} {};

        Image * perform(Image* image) override;

    };

    struct image_size {
        size_t height;
        size_t width;
    };

    template<typename Image>
    class ResizeOperation: public Operation<Image> {
    private:
        image_size lower;
        image_size upper;

    public:
        ResizeOperation() = delete;

        explicit ResizeOperation(image_size lower, image_size upper, double prob = UPPER_BOUND_PROB,
                unsigned seed = NULL_SEED): Operation<Image>{prob, seed}, lower{lower}, upper{upper} {};

        Image * perform(Image* image) override;

    };

    template<typename Image>
    class InvertOperation: public Operation<Image> {
    public:
        explicit InvertOperation(double prob = UPPER_BOUND_PROB, unsigned seed = NULL_SEED):
                Operation<Image>{prob, seed} {}

        Image * perform(Image* image) override;

    };

    template<typename Image, int Kernel = 0>
    class BlurOperation: public Operation<Image> {
    private:
        gaussian_blur_filter_1D<Kernel> filter;
    public:
        explicit BlurOperation(const double sigma, const size_t n,
                double prob = UPPER_BOUND_PROB, unsigned seed = NULL_SEED): Operation<Image>{prob, seed},
                filter(sigma, n) {}

        explicit BlurOperation(const double sigma, double prob = UPPER_BOUND_PROB, unsigned seed = NULL_SEED):
            Operation<Image>{prob, seed}, filter(sigma) {}

        Image * perform(Image* image) override;

    };

    template<typename Image>
    class RandomEraseOperation: public Operation<Image> {
    private:
        typedef typename Image::pixel_value_type pixel_value_type;
        UniformDistributionGenerator<size_t> xy_generator;
        UniformDistributionGenerator<pixel_value_type> noise_generator;
        image_size lower_mask_size;
        image_size upper_mask_size;
    public:
        explicit RandomEraseOperation(image_size lower_mask_size, image_size upper_mask_size,
                double prob = UPPER_BOUND_PROB, unsigned seed = NULL_SEED, unsigned xy_seed = NULL_SEED,
                unsigned noise_seed = NULL_SEED):
                Operation<Image>{prob, seed},
                xy_generator(xy_seed),
                noise_generator(noise_seed),
                lower_mask_size{lower_mask_size}, upper_mask_size{upper_mask_size} {}


        Image * perform(Image* image) override;

    };


    // Below is the implementation
    template<typename Image>
    template<typename Container>
    Container&& Operation<Image>::perform(Container&& container) {
        auto results = Container();
        for (auto& image : container) {
            results.push(Operation<Image>::perform(image));
        }
        return results;
    }


    template<typename Image>
    Image * StdoutOperation<Image>::perform(Image* image) {
        if (!Operation<Image>::operate_this_time()) {
            return nullptr;
        }
        std::cout << "(Image*) Stdout Operation is called:" << std::endl << str << std::endl;
        return image;
    }


    template<typename Image>
    Image *ResizeOperation<Image>::perform(Image *image) {
        if (!Operation<Image>::operate_this_time()) {
            return image;
        }
        auto factor = Operation<Image>::uniform_random_number();
        int height = (upper.height - lower.height) * factor + lower.height;
        int width = (upper.width - lower.width) * factor + lower.width;

        image->resize(height, width);
        return image;
    }


    template<typename Image>
    Image *InvertOperation<Image>::perform(Image *image) {
        if (!Operation<Image>::operate_this_time()) {
            return image;
        }
        // Invert image
        for(size_t y = 0; y < image->getHeight(); y++) {
            for(size_t x = 0; x < image->getWidth(); x++) {
                std::vector<uint8_t> pixels = image->getPixel(x, y);

                for(uint8_t &p: pixels){
                    p = ~p;
                }
                image->setPixel(x,y,pixels);
            }
        }
        return image;
    }


    inline void convert2pixel(const std::vector<double>& src, std::vector<uint8_t>& target, size_t n) {
        for (size_t i = 0; i < n; ++i) {
            target[i] = src[i];
        }
    }

    template<typename Image, int Kernel>
    Image *BlurOperation<Image, Kernel>::perform(Image *image) {
        if (!Operation<Image>::operate_this_time()) {
            return image;
        }
        auto kernel_size = filter.size();
        auto transient = Image(image->getWidth(), image->getHeight(), image->getPixelSize(), image->getColorSpace());
        auto pixel_size = image->getPixelSize();
        auto val = std::vector<double>(pixel_size);
        auto new_pixel = std::vector<uint8_t>(pixel_size);

        // convolute at height axis
        for (size_t i = 0; i< image->getWidth(); ++i) {
            for (size_t j = 0; j < image->getHeight(); ++j) {
                std::fill(val.begin(), val.end(), 0);
                long x0 = i - (kernel_size / 2);

                for (size_t k = 0; k < kernel_size; ++k) {
                    size_t x = std::min((size_t) std::max(x0++, 0l), image->getWidth() - 1);
//                    std::cout << i << " " << kernel_size << " " << x0 << " " <<  x << " " << j << " " << image->getWidth() << std::endl;
                    auto pixel = image->getPixel(x, j);
                    for (size_t p = 0; p < pixel_size; ++p) {
                        val[p] += pixel[p] * filter[k];
                    }
                }
                convert2pixel(val, new_pixel, pixel_size);
                transient.setPixel(i, j, new_pixel);
            }
        }

        // convolute at width axis
        for (size_t i = 0; i< image->getWidth(); ++i) {
            for (size_t j = 0; j < image->getHeight(); ++j) {
                std::fill(val.begin(), val.end(), 0);
                long y0 = j - (kernel_size / 2);

                for (size_t k = 0; k < kernel_size; ++k) {
                    size_t y = std::min((size_t) std::max(y0++, 0l), image->getHeight() - 1);
                    auto pixel = image->getPixel(i, y);
                    for (size_t p = 0; p < pixel_size; ++p) {
                        val[p] += pixel[p] * filter[k];
                    }
                }
                convert2pixel(val, new_pixel, pixel_size);
                image->setPixel(i, j, new_pixel);
            }
        }

        return image;
    }

    template<typename Image>
    Image* RandomEraseOperation<Image>::perform(Image *image) {
        if (!Operation<Image>::operate_this_time()) {
            return image;
        }

        auto lower_erase_size = image_size{
                std::min(image->getHeight(), lower_mask_size.height),
                std::min(image->getWidth(), lower_mask_size.width),
        };

        auto upper_erase_size = image_size{
                std::min(image->getHeight(), upper_mask_size.height),
                std::min(image->getWidth(), upper_mask_size.width),
        };

        auto factor = RandomEraseOperation<Image>::uniform_random_number();
        auto erase_size = image_size{
                (size_t) ((upper_erase_size.height - lower_erase_size.height) * factor) + lower_erase_size.height,
                (size_t) ((upper_erase_size.width - lower_erase_size.width) * factor) + lower_erase_size.width
        };

        auto top = xy_generator() % (image->getHeight() - erase_size.height + 1);
        auto left = xy_generator() % (image->getWidth() - erase_size.width + 1);

        auto pixel_size = image->getPixelSize();
        auto new_pixel = std::vector<RandomEraseOperation::pixel_value_type>(pixel_size);

        for (size_t i = left; i < left + erase_size.width; ++i) {
            for (size_t j = top; j < top + erase_size.height; ++j) {
                for (size_t k = 0; k < pixel_size; ++k) {
                    new_pixel[k] = noise_generator();
                }
                image->setPixel(i, j, new_pixel);
            }
        }

        return image;
    }
}

#endif //LIB_OPERATION_H


