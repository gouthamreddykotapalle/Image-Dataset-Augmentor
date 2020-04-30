.PHONY: debug, clean

prod: main.cpp Augmentor.cpp jpeg.cpp Operation.cpp
	g++ -O -std=c++17 -Wall -Wextra -Wpedantic -Werror -o prod main.cpp Augmentor.cpp jpeg.cpp Operation.cpp -ljpeg


test: unit_test.cpp Augmentor.cpp jpeg.cpp Operation.cpp
	g++ -O -std=c++17 -Wall -Wextra -Wpedantic -Werror -o test unit_test.cpp Augmentor.cpp jpeg.cpp Operation.cpp -ljpeg -lgtest

debug: main.cpp Augmentor.cpp jpeg.cpp Operation.cpp
	g++ -g -O -std=c++17 -Wall -Wextra -Wpedantic -Werror -o debug *.cpp -ljpeg

clean:
	rm -f test
