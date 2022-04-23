compile:
	#gcc -fopenmp image-effect.cpp -o my-effect `pkg-config opencv4 --libs opencv4`
	#cmake -B./build -S./src
	cmake .
	#cmake ./build
	make -f Makefile
