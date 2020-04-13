#include "Augmentor.h"

namespace augmentorLib {
    Augmentor::Augmentor(char *filename) {
        this->output = Image(filename);
    }

    Augmentor& Augmentor::save(const std::string& fileName, int quality) {
        output.save(fileName, quality);

        return *this;
    }


    Augmentor& Augmentor::resize(int newHeight, int newWidth) {
        output.resize(newHeight, newWidth);

        return *this;
    }

    Augmentor& Augmentor::invert() {
        // Invert image
        for(size_t y = 0; y < output.getHeight(); y++) {
            for(size_t x = 0; x < output.getWidth(); x++) {
                std::vector<uint8_t> pixels = output.getPixel(x, y);

                for(uint8_t &p: pixels){
                    p = ~p;
                }

                output.setPixel(x,y,pixels);
            }
        }

        return *this;
    }
}

