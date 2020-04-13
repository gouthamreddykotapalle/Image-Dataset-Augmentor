#include "Augmentor.h"

namespace augmentorLib {
    Augmentor::Augmentor(char *filename) {
        this->img = Image(filename);
    }

    Augmentor& Augmentor::save(const std::string& fileName, int quality) {
        img.save(fileName, quality);

        return *this;
    }


    Augmentor& Augmentor::resize(int newHeight, int newWidth) {
        img.resize(newHeight, newWidth);

        return *this;
    }

    Augmentor& Augmentor::invert() {
        // Invert image
        for(size_t y = 0; y < img.getHeight(); y++) {
            for(size_t x = 0; x < img.getWidth(); x++) {
                std::vector<uint8_t> pixels = img.getPixel(x, y);

                for(uint8_t &p: pixels){
                    p = ~p;
                }

                img.setPixel(x,y,pixels);
            }
        }

        return *this;
    }
}

