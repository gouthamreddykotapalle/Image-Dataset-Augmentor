#include <iostream>
#include <vector>

#include "Augmentor.h"
#include "Operation.h"

using namespace std;
using namespace augmentorLib;

int main( int argc, char* argv[] )
{
    int N = 10;
    std::cout << "Going to call the Stdout operations (50% success rate) " << N << " times\n";
    auto ops = StdoutOperation<int>("hello world", 0.5);
    for (int i = 0; i < N; i++) {
        int* pt = nullptr;
        ops.perform(pt);
    }

//    auto filter = gaussian_blur_filter_1D<5>(1);
//    for (int i = 0; i < 5; i++) {
//        std::cout << filter[i] << " ";
//    }
//    std::cout << std::endl;
//    return 0;

    if ( argc < 2 ) {
        std::cout << "No directory path specified\n";
        return 1;
    }
    try {
        Augmentor augmentor(argv[1]);

        //Resize example and Invert color example
//        augmentor.resize(400, 400, 600, 600).invert(0.1)
//        .blur<11>(50);
//        augmentor.sample(1);
//        augmentor.blur<11>(50);
//        augmentor.sample(1);
//        augmentor.blur(50, 11ul);
//        augmentor.sample(1);
        augmentor.random_erase(image_size{10, 10}, image_size{1000, 1000}).crop(300,300, true);
        augmentor.sample(1);
        return 0;
    }
    catch( const std::exception& e ) {
        std::cout << "Error: " << e.what() << std::endl;
        return 1;
    }
}