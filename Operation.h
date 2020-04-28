//
// Created by Augustin Pan on 4/15/20.
//

#ifndef LIB_OPERATION_H
#define LIB_OPERATION_H
#define PI 3.14159

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

    template <typename DataType, bool IsReal = std::is_floating_point<DataType>::value>
    class UniformDistributionGenerator;

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

        explicit UniformDistributionGenerator(unsigned seed):
                generator{init_seed(seed)},
                distribution{std::numeric_limits<DataType>::min(), std::numeric_limits<DataType>::max()} {}

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
    class CropOperation: public Operation<Image> {
    private:
        image_size size;
        bool center; //True - use fixed center. False - use random center

    public:
        CropOperation() = delete;

        explicit CropOperation(image_size size, bool center,  double prob = UPPER_BOUND_PROB,
                                 unsigned seed = NULL_SEED): Operation<Image>{prob, seed}, size{size}, center{center} {};

        Image * perform(Image* image) override;

    };

    struct rotate_range {
        int min_rotate;
        int max_rotate;
    };

    template<typename Image>
    class RotateOperation: public Operation<Image> {
    private:
        rotate_range range;

    public:
        RotateOperation() = delete;

        explicit RotateOperation(rotate_range range, double prob = UPPER_BOUND_PROB,
                               unsigned seed = NULL_SEED): Operation<Image>{prob, seed}, range{range} {};

        Image * perform(Image* image) override;

    };

    struct zoom_factor {
        double min_factor;
        double max_factor;
    };

    template<typename Image>
    class ZoomOperation: public Operation<Image> {
    private:
        zoom_factor factor;
//        bool center; //True - use fixed center. False - use random center

    public:
        ZoomOperation() = delete;

        explicit ZoomOperation(zoom_factor factor,  double prob = UPPER_BOUND_PROB,
                               unsigned seed = NULL_SEED): Operation<Image>{prob, seed}, factor{factor} {};

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

    template<typename Image>
    class FlipOperation: public Operation<Image> {
    private:
        const std::string& type;
    public:
        explicit FlipOperation(const std::string& type,
                               double prob = UPPER_BOUND_PROB, unsigned seed = NULL_SEED): Operation<Image>{prob, seed},
                                                                                           type(type) {}//super.

        Image * perform(Image* image) override;

    };

    template<typename Image>
    Image *FlipOperation<Image>::perform(Image *image) {
        if (!Operation<Image>::operate_this_time()) {
            return image;
        }

        if(type=="Horizontal")
        {
            for(size_t y = 0; y < image->getHeight(); ++y) {
                for(size_t x = 0; x < image->getWidth()/2; ++x) {
                    std::vector<uint8_t> left_pixels = image->getPixel(x, y);
                    std::vector<uint8_t> right_pixels = image->getPixel(image->getWidth() - x - 1, y);

                    image->setPixel(x, y, right_pixels);
                    image->setPixel(image->getWidth()-x-1, y, left_pixels);
                }
            }
        } else if(type=="Vertical"){
            for(size_t y = 0; y < image->getHeight()/2; ++y) {
                for(size_t x = 0; x < image->getWidth(); ++x) {
                    std::vector<uint8_t> top_pixels = image->getPixel(x, y);
                    std::vector<uint8_t> bottom_pixels = image->getPixel(x, image->getHeight() - y - 1);

                    image->setPixel(x, y, bottom_pixels);
                    image->setPixel(x, image->getHeight() - y - 1, top_pixels);
                }
            }
        }
        else
        {
            throw std::out_of_range("Unknown Flip type - Choose wither 'Horizontal' or 'Vertical'");
        }
        return image;
    }


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
        //std::cout << "(Image*) Stdout Operation is called:" << std::endl << str << std::endl;
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
    Image *CropOperation<Image>::perform(Image *image) {;
        if (!Operation<Image>::operate_this_time()) {
            return image;
        }

        int w = image->getWidth();
        int h = image->getHeight();


        Image temp(size.width, size.height);
        if (center){
            auto x = w/2;
            auto y = h/2;


            auto left_offset = x - size.width/2;
            auto down_offset = y - size.height/2;

//            if (left_offset < 0 || left_offset >= w) {
//                throw std::out_of_range("xOffset is out of range");
//            }
//            if (down_offset < 0 || down_offset >= h){
//                throw std::out_of_range("yOffset is out of range");
//            }
//            if (size.width < 0 || (size.width + left_offset) >= w) {
//                throw std::out_of_range("widthCrop is out of range");
//            }
//            if (size.height < 0 || (size.height + down_offset) >= h) {
//                throw std::out_of_range("heightCrop is out of range");
//            }
//            std::cout<<temp->getWidth()<<" "<<temp->getHeight()<<std::endl;

            for(unsigned long i=left_offset, i1=0; i<left_offset+size.width; i++, i1++){
                for(unsigned long  j=down_offset, j1=0; j<down_offset+size.height; j++, j1++){
                    temp.setPixel(i1, j1, image->getPixel(i,j));
                }
            }

            //return image;
            }
        else{
//            TODO: For random centers
//            auto left_shift = Operation<Image>::uniform_random_number(0, w - size.width);
//            auto down_shift = Operation<Image>::uniform_random_number(0, h - size.height);
        }


//        image->crop(left_shift, down_shift, center);
        *image = temp;
        return image;
    }

    template<typename Image>
    Image *ZoomOperation<Image>::perform(Image *image) {
        if (!Operation<Image>::operate_this_time()) {
            return image;
        }

        double zoom_level = Operation<Image>::uniform_random_number(factor.min_factor, factor.max_factor);
        zoom_level = static_cast<float>(static_cast<int>(zoom_level * 10.)) / 10.;

        int w = image->getWidth();
        int h = image->getHeight();

        //TODO: int double issue
        int w_zoomed = w*zoom_level;
        int h_zoomed = h*zoom_level;

        image->resize(h_zoomed, w_zoomed);

        auto operation = CropOperation<Image>(
                image_size{static_cast<size_t>(h), static_cast<size_t>(w)}, true, 1
        );

        image = operation.perform(image);

        return image;
    }

    template<typename Image>
    Image *RotateOperation<Image>::perform(Image *image) {
        if (!Operation<Image>::operate_this_time()) {
            return image;
        }

        double rotate_degree = Operation<Image>::uniform_random_number(range.min_rotate, range.max_rotate);

        int w = image->getWidth();
        int h = image->getHeight();
        Image temp(w, h);

        int hwidth = w / 2;
        int hheight = h / 2;
        double angle = rotate_degree * PI / 180.0; //TODO: Add PI Value

        for (int x = 0; x < w;x++) {

            for (int y = 0; y < h;y++) {


                int xt = x - hwidth;
                int yt = y - hheight;


                int xs = (int)round((cos(angle) * xt - sin(angle) * yt) + hwidth);
                int ys = (int)round((sin(angle) * xt + cos(angle) * yt) + hheight);


                if (xs >= 0 && xs < w && ys >= 0 && ys < h){
                    temp.setPixel(x, y, image->getPixel(xs, ys));
                }

            }
        }

        *image = temp;
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


