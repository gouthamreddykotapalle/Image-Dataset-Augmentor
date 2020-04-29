.PHONY: debug, clean

test: main.cpp Augmentor.cpp jpeg.cpp Operation.cpp
	g++ -O -std=c++17 -Wall -Wextra -Wpedantic -Werror -o test *.cpp -ljpeg

debug: main.cpp Augmentor.cpp jpeg.cpp Operation.cpp
	g++ -g -O -std=c++17 -Wall -Wextra -Wpedantic -Werror -o test *.cpp -ljpeg

clean:
	rm -f test
