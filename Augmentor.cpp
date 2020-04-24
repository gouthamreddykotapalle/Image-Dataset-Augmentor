#include "Augmentor.h"
#include <filesystem>
#include <chrono>
namespace fs = std::filesystem;
typedef std::chrono::high_resolution_clock  clocking;

namespace augmentorLib {
    Augmentor::Augmentor(const std::string& path) {
        //this->img = Image(filename);
        Augmentor::pipeline(path);
    }

   void Augmentor::save(const std::string& fileName, Image* image, int quality) {
        image->save(fileName, quality);
    }

    Augmentor& Augmentor::pipeline(const std::string& directory_path) {
        this->dir_path = directory_path;

        //auto operation = std::make_unique<SaveOperation<Image>>(directory_path);
        for (const auto & entry : fs::directory_iterator(directory_path))
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

    Augmentor &Augmentor::resize(int lower_height, int lower_width, int upper_height, int upper_width, double prob) {
        auto operation = std::make_unique<ResizeOperation<Image>>(
                image_size{lower_height, lower_width}, image_size{upper_height, upper_width}, prob
                );
        operations.push_back(std::move(operation));
        return *this;;
    }

    Augmentor &Augmentor::resize(int height, int width, double prob) {
        auto operation = std::make_unique<ResizeOperation<Image>>(
                image_size{height, width}, image_size{height, width}, prob
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

        std::cout<<"output size= "<<output_array.size()<<std::endl;
        for (unsigned i = 0; i < output_array.size(); i++) {
            std::cout<<"output[" << i << "]=" << output_array[i] << std::endl;
        }

        int j=0;
        for(const std::string& item:output_array) {
            Image img = Image(item);//creating a temp img object
            auto image = &img;
            for (auto &operation : operations) {
                image = operation->perform(image);
            }
            std::string new_img_path = item.substr(0, item.size()-4);
            std::cout<<new_img_path + "_" + std::to_string(j) + ".jpg"<<"\n";
            //TODO:: change to output dir_path

            this->save(new_img_path +  "_" + std::to_string(j) + ".jpg", image);
            j++;
        }
    }
}

