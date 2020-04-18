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

    if ( argc < 2 ) {
        std::cout << "No jpeg file specified\n";
        return 1;
    }
    try {
        Augmentor img(argv[1]);

        //Resize example
        img.resize(400, 400).save("/Users/shubhamsinha/Desktop/Columbia/Design/output_resized.jpg");
        //Invert color example
        img.invert().save("/Users/shubhamsinha/Desktop/Columbia/Design/output.jpg");
        return 0;
    }
    catch( const std::exception& e ) {
        std::cout << "Error: " << e.what() << std::endl;
        return 1;
    }
}