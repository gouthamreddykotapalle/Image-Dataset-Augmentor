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
        Augmentor augmentor(argv[1]);

        //Resize example
        augmentor.resize(400, 400).save("./output_resized.jpg");
        augmentor.run();
        //Invert color example
        augmentor.invert(0.1).save("./output.jpg");
        augmentor.run();
        return 0;
    }
    catch( const std::exception& e ) {
        std::cout << "Error: " << e.what() << std::endl;
        return 1;
    }
}