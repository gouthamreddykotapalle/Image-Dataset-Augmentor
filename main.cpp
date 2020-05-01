#include <iostream>
#include <vector>

#include "Augmentor.h"

using namespace std;
using namespace augmentorLib;
typedef chrono::high_resolution_clock  clocking;

int main( int argc, char* argv[] )
{

    if ( argc < 3 ) {
        std::cout << "Please specify both input and output paths\n";
        return 1;
    }
    try {
        Augmentor augmentor(argv[1],argv[2]);
        clocking::time_point start = clocking::now();
        augmentor
        .rotate(0, 90, 0.5)
        .rotate(2, 30, 0.27)
        .flip(HORIZONTAL, 0.8)
        .flip(VERTICAL, 0.3)
        .crop(700, 700, true, 1)
        .invert(0.1)
        .blur<11>(50, 0.2)
        .random_erase({50,50}, {100, 100}, 0.5)
        .sample(10);
        clocking::time_point end = clocking::now();
        clocking::duration dur = end - start;
        int timetaken = chrono::duration_cast<chrono::seconds>(dur).count();
        cout << "Time taken in seconds is = " << timetaken << endl;
        return 0;
    }
    catch( const std::exception& e ) {
        std::cout << "Error: " << e.what() << std::endl;
        return 1;
    }
}