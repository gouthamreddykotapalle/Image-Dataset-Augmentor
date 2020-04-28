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


    if ( argc < 3 ) {
        std::cout << "Please specify both input and output paths\n";
        return 1;
    }
    try {
        Augmentor augmentor(argv[1],argv[2]);
        augmentor
        .random_erase(image_size{100, 100}, image_size{300, 100}, 0.5)
        .flip("Horizontal", .25)
        .rotate(0, 360, 0.40)
        .crop(300, 300, true, 0.10)
        .invert(0.30)
        .zoom(1.2, 1.4, 1)
        .blur<11>(10,0.5)
        .sample(10);
        return 0;
    }
    catch( const std::exception& e ) {
        std::cout << "Error: " << e.what() << std::endl;
        return 1;
    }
}