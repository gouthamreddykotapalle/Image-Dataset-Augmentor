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


namespace augmentorLib {

    const double LOWER_BOUND_PROB = 0.0;
    const double UPPER_BOUND_PROB = 1.0;
    const unsigned NULL_SEED = 0;


    template <typename DataType>
    class UniformDistributionGenerator {
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

        explicit UniformDistributionGenerator(unsigned seed):
                generator{init_seed(seed)},
                distribution{LOWER_BOUND_PROB, UPPER_BOUND_PROB} {}

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

        Operation(): Operation{UPPER_BOUND_PROB} {};

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
        int height;
        int width;
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
}

#endif //LIB_OPERATION_H


