#include "Augmentor.h"

namespace augmentorLib {
    Augmentor::Augmentor(char *filename) {
        this->img = Image(filename);
    }

    Augmentor& Augmentor::save(const std::string& fileName, int quality, double prob) {
        auto operation = std::make_unique<SaveOperation<Image>>(fileName, quality, prob);
        operations.push_back(std::move(operation));
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

    void Augmentor::run() {
        auto image = &img;
        for (auto& operation : operations) {
            image = operation->perform(image);
            if (image == nullptr) {
                break;
            }
        }
    }
}

