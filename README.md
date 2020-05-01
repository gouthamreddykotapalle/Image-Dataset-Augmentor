# Image-Augmentation-Library
Library Design using C++ Final Project

Using jpeglib.h which is a commonly used jpeg library/package - http://libjpeg.sourceforge.net/

Using the library - https://github.com/md81544/libjpeg_cpp/ (https://www.martyndavis.com/?tag=libjpeg)


<br>
This is a wrapper around the standard libjpeg/jpeglib.h written in STL around the jpeglib.h data structures that use c-style representations.

<br>

We will be using this Wrapper instead of the standard classes that Jpeglib.h offers since it is way more simpler and writing our own wrapper might be tedious.

Before starting, please install libjpeg using (Comes in-built if you are using Unix-like distributions) - 
<br>
brew install libjpeg or brew install libjpeg-dev


Steps to use ImageAugmentor library:

1. git clone https://github.com/Gouthamkreddy1234/Image-Augmentation-Library.git
2. brew install libjpeg
3. Copy the code form the cloned directory into your project directory
4. Add the Augmentor.cpp jpeg.cpp Operation.cpp to your makefile (follow below example assuming main.cpp is your main project file)

    prod: main.cpp Augmentor.cpp jpeg.cpp Operation.cpp
      g++ -O -std=c++17 -Wall -Wextra -Wpedantic -Werror -o prod main.cpp Augmentor.cpp jpeg.cpp Operation.cpp -ljpeg

    test: unit_test.cpp Augmentor.cpp jpeg.cpp Operation.cpp
      g++ -O -std=c++17 -Wall -Wextra -Wpedantic -Werror -o test unit_test.cpp Augmentor.cpp jpeg.cpp Operation.cpp -ljpeg -lgtest

    debug: main.cpp Augmentor.cpp jpeg.cpp Operation.cpp
      g++ -g -O -std=c++17 -Wall -Wextra -Wpedantic -Werror -o debug *.cpp -ljpeg

5. Command - make prod
    IMPORTANT: Use the prod target when you build the code, test target builds only the unit test code

6. #include "Augmentor.h" in your main.cpp

7. Usage example:

    Augmentor augmentor(argv[1],argv[2]); //input and output directory path
    augmentor
    .rotate(45,90,0.5) // 45-90 degree of rotation randomness
    .flip(HORIZONTAL, 0.5) // 0.5 probability of flip operation being applied to an image
    .crop(300, 300, true) // (x, y) size of cropped image
    .resize(120,120,1) // (x, y) size of resized image
    .invert(1) // invert with probability 1
    .sample(1000); // Output 1000 images

8. This will output 1000 augmented images to the provided destination directory (argv[2])

9. Refer for documentation - http://image-augmentor.s3-website-us-east-1.amazonaws.com

