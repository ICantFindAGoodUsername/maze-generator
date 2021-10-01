# maze-generator
C++ project for generating n-dimensional mazes

Currently can only save 2d mazes as bitmap images
uses a monochrome bitmap library:
https://github.com/pertbanking/bitmap-monochrome
the library has to be on the compiler's include path

maze object lifecycle:

  1: call maze constructor, sets maze parameters
  
  2: call generate(), randomly generates maze
  
  3: (optional) call asPixelMatrix() to make bitmap image, if maze is 2d
  
  4: call maze destructor, or end program to take care of object's garbage collection
