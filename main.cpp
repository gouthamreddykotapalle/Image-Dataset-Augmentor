#include <iostream>
#include <vector>

#include "Augmentor.h"
#include "Operation.h"

using namespace std;
using namespace augmentorLib;
typedef chrono::high_resolution_clock  clocking;

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
        clocking::time_point start = clocking::now();
        augmentor
//        .rotate(45,90,1)
//        .rotate(2,25,1)
//        .flip(HORIZONTAL, 1)
//        .flip(VERTICAL, 1)
//        .crop(300, 300, true)
//        .resize(120,120,1)
//        .rapid_blur(5)
        .zoom(2,2,1)
        .sample(1);
        clocking::time_point end = clocking::now();
        clocking::duration dur = end - start;
        int timetaken = chrono::duration_cast<chrono::seconds>(dur).count();
        cout << "Time taken in seconds is = " << timetaken << endl;
//        Augmentor augmentor(argv[1],argv[2]);
//        augmentor.rapid_blur(5).sample(1);
        return 0;
    }
    catch( const std::exception& e ) {
        std::cout << "Error: " << e.what() << std::endl;
        return 1;
    }
}