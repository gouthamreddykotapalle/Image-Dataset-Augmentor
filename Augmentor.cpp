#include "Augmentor.h"
#include <filesystem>
#include <chrono>
namespace fs = std::filesystem;
typedef std::chrono::high_resolution_clock  clocking;

namespace augmentorLib {
    Augmentor::Augmentor(const std::string& in_path, const std::string& out_path) {
        //this->img = Image(filename);
        this->dir_path = in_path;
        this->out_path = out_path;
        Augmentor::pipeline();
    }

   void Augmentor::save(const std::string& fileName, Image* image, int quality) {
        image->save(fileName, quality);
    }

    Augmentor& Augmentor::pipeline() {
        //auto operation = std::make_unique<SaveOperation<Image>>(directory_path);
        for (const auto & entry : fs::directory_iterator(this->dir_path))
        {
            //for each image in input dir, create an object
            std::string temp = entry.path();
            std::size_t found = temp.find(".jpg");
            if( found!=std::string::npos) {
                std::cout << entry.path() << std::endl;
                this->image_paths.push_back(temp);
            }
        }

        return *this;
    }


    Augmentor& Augmentor::resize(image_size lower, image_size upper, double prob) {
        auto operation = std::make_unique<ResizeOperation<Image>>(lower, upper, prob);
        operations.push_back(std::move(operation));
        return *this;
    }

    Augmentor &Augmentor::resize(image_size new_size, double prob) {
        auto operation = std::make_unique<ResizeOperation<Image>>(new_size, new_size, prob);
        operations.push_back(std::move(operation));
        return *this;;
    }

    Augmentor &Augmentor::resize(size_t lower_height, size_t lower_width, size_t upper_height, size_t upper_width, double prob) {
        auto operation = std::make_unique<ResizeOperation<Image>>(
                image_size{lower_height, lower_width}, image_size{upper_height, upper_width}, prob
                );
        operations.push_back(std::move(operation));
        return *this;;
    }

    Augmentor &Augmentor::resize(size_t height, size_t width, double prob) {
        auto operation = std::make_unique<ResizeOperation<Image>>(
                image_size{height, width}, image_size{height, width}, prob
                );
        operations.push_back(std::move(operation));
        return *this;;
    }

    Augmentor &Augmentor::crop(int height, int width, bool center, double prob) {
        auto operation = std::make_unique<CropOperation<Image>>(
                image_size{static_cast<size_t>(height), static_cast<size_t>(width)}, center, prob
        );
        operations.push_back(std::move(operation));
        return *this;;
    }

    Augmentor &Augmentor::zoom(double min_factor, double max_factor, double prob) {
        auto operation = std::make_unique<ZoomOperation<Image>>(
                zoom_factor{min_factor, max_factor}, prob
        );
        operations.push_back(std::move(operation));
        return *this;;
    }

    Augmentor &Augmentor::rotate(int min_degree, int max_degree, double prob) {
        auto operation = std::make_unique<RotateOperation<Image>>(
                rotate_range{min_degree, max_degree}, prob
        );
        operations.push_back(std::move(operation));
        return *this;;
    }

    Augmentor& Augmentor::invert(double prob) {
        auto operation = std::make_unique<InvertOperation<Image>>(prob);
        operations.push_back(std::move(operation));
        return *this;
    }

    void Augmentor::sample(size_t size) {
        clocking::time_point beginning =  clocking::now();
        clocking::duration d =  clocking::now()-beginning;

        std::uniform_int_distribution<int> distribution(0, image_paths.size() - 1);
        auto seed = (unsigned)d.count();
        std::default_random_engine generator(seed);

        for(size_t i=0;i<size;i++) {
            //auto image = &img;
            int j = distribution(generator);
            this->output_array.push_back(image_paths[j]);
        }

        //std::cout<<"output size= "<<output_array.size()<<std::endl;
//        for (unsigned long i = 0; i < output_array.size(); i++) {
//            //std::cout<<"output[" << i << "]=" << output_array[i] << std::endl;
//        }

        int j=0;
        for(const std::string& item:output_array) {
            Image img = Image(item);//creating a temp img object
            auto image = &img;
//            clocking::time_point start = clocking::now();
            for (auto &operation : operations) {
                image = operation->perform(image);
            }
//            clocking::time_point end = clocking::now();
//            clocking::duration dur = end - start;
//            int timetaken = std::chrono::duration_cast<std::chrono::milliseconds>(dur).count();
//            std::cout << "Time taken in mseconds is = " << timetaken << std::endl;
            //std::cout<<this->out_path + "output_" + std::to_string(j) + ".jpg"<<"\n";
            this->save(this->out_path +  "output_" + std::to_string(j) + ".jpg", image);
            j++;
        }
    }

    Augmentor &Augmentor::blur(double sigma, size_t kernel_size, double prob) {
        auto operation = std::make_unique<BlurOperation<Image>>(sigma, kernel_size, prob);
        operations.push_back(std::move(operation));
        return *this;
    }

    Augmentor &Augmentor::random_erase(image_size lower_mask_size, image_size upper_mask_size, double prob) {
        auto operation = std::make_unique<RandomEraseOperation<Image>>(lower_mask_size, upper_mask_size, prob);
        operations.push_back(std::move(operation));
        return *this;
    }

    Augmentor &Augmentor::random_erase(image_size mask_size, double prob) {
        auto operation = std::make_unique<RandomEraseOperation<Image>>(mask_size, mask_size, prob);
        operations.push_back(std::move(operation));
        return *this;
    }

    Augmentor &Augmentor::flip(const std::string& type, double prob) {
        auto operation = std::make_unique<FlipOperation<Image>>(type, prob);
        operations.push_back(std::move(operation));
        return *this;
    }
}

