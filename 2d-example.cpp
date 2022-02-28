/*
example usage of a 2d, 50x50 maze
saves generated maze to bitmap file
*/

#include "maze.cpp"
#include <iostream>

using namespace std;

const char* file_name = "50x50.bmp";

int main(){
	int sizes[] {50, 50};
	maze m(2, sizes);
	
	cout << "generating...";
	m.generate();
	cout << "\ndone generating";
	
	PixelMatrix pm = m.asPixelMatrix();
	Bitmap bm;
	bm.fromPixelMatrix(pm);
	bm.save(file_name);
	cout << "\nsaved to file";
}
