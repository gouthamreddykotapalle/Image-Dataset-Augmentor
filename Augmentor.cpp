#include "Augmentor.h"
#include <filesystem>
#include <chrono>
namespace fs = std::filesystem;
typedef std::chrono::high_resolution_clock  clocking;

namespace augmentorLib {
    Augmentor::Augmentor(const std::string& path) {
        //this->img = Image(filename);
        this->path = path;
    }

    Augmentor& Augmentor::save(const std::string& fileName, Image* image, int quality) {
        auto operation = std::make_unique<SaveOperation<Image>>(fileName, quality);
//        operations.push_back(std::move(operation));
        operation->perform(image);
        return *this;
    }

    Augmentor& Augmentor::pipeline(const std::string& directory_path) {
        //auto operation = std::make_unique<SaveOperation<Image>>(directory_path);
        for (const auto & entry : fs::directory_iterator(directory_path))
        {
            //for each image in input dir, create an object
            std::string temp = entry.path();
            std::size_t found = temp.find(".jpg");
            if( found!=std::string::npos) {
                std::cout << entry.path() << std::endl;
                this->imgs.push_back(temp);
            }
        }

        return *this;
    }


    Augmentor& Augmentor::resize(int newHeight, int newWidth, double prob) {
        auto operation = std::make_unique<ResizeOperation<Image>>(newHeight, newWidth, prob);
        operations.push_back(std::move(operation));
        return *this;
    }

    Augmentor& Augmentor::invert(double prob) {
        auto operation = std::make_unique<InvertOperation<Image>>(prob);
        operations.push_back(std::move(operation));
        return *this;
    }

    void Augmentor::sample(size_t size) {
        clocking::time_point beginning =  clocking::now();
        clocking::duration d =  clocking::now()-beginning;

        std::uniform_int_distribution<int> distribution(0,imgs.size()-1);
        auto seed = (unsigned)d.count();
        std::default_random_engine generator(seed);

        for(size_t i=0;i<size;i++) {
            //auto image = &img;
            int j = distribution(generator);
            this->output_array.push_back(imgs[j]);
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
                auto returned_image = operation->perform(image);
                if (returned_image == nullptr) {
                    break;
                }
                image = returned_image;
            }
            std::string new_img_path = item.substr(0, item.size()-4);
            std::cout<<new_img_path + "_" + std::to_string(j) + ".jpg"<<"\n";
            //TODO:: change to output path

            this->save(new_img_path + "_" + std::to_string(j) + ".jpg", image);
            j++;
        }
    }
}

