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


            int left_offset = x - size.width/2;
            int down_offset = y - size.height/2;

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

            for(int i=left_offset, i1=0; i<left_offset+size.width; i++, i1++){
                for(int j=down_offset, j1=0; j<down_offset+size.height; j++, j1++){
                    temp.setPixel(i1, j1, image->getPixel(i,j));
                }
            }

            //return image;
            }
        else{
// TODO: For random centers
//            auto left_shift = Operation<Image>::uniform_random_number(0, w - size.width);
//            auto down_shift = Operation<Image>::uniform_random_number(0, h - size.height);
        }


//        image->crop(left_shift, down_shift, center);
        *image = temp;
        return image;
    }

    template<typename Image>
    Image *ZoomOperation<Image>::perform(Image *image) {std::cout<<"ssds";
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
                image_size{h, w}, true, 1
        );

        image = operation.perform(image);

        return image;
    }

    template<typename Image>
    Image *RotateOperation<Image>::perform(Image *image) {std::cout<<"ssds";
        if (!Operation<Image>::operate_this_time()) {
            return image;
        }

        double rotate_degree = Operation<Image>::uniform_random_number(range.min_rotate, range.max_rotate);

        int w = image->getWidth();
        int h = image->getHeight();
        Image temp(w, h);

        int hwidth = w / 2;
        int hheight = h / 2;
        double angle = rotate_degree * 3.14156 / 180.0; //TODO: Add PI Value

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


