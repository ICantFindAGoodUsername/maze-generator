# maze-generator
C++ project for generating n-dimensional mazes  
Currently, the only form of saving a maze is by making a bitmap image if the maze is 2d
Future fix: the random selection of a node is not constant time because of my choice of data structures so algorithm is slow for larger mazes
## compiling
uses a [monochrome bitmap library](https://github.com/pertbanking/bitmap-monochrome)  
the library has to be on the compiler's include path
## using maze objects
maze object lifecycle:
1. call maze constructor, sets maze parameters (dimension number, length of each side)
2. call generate(), randomly generates maze
3. (if maze is 2d) call asPixelMatrix() if you want to save to bitmap image
4. call maze destructor, or end program to take care of object's garbage collection
## how it works
this is a possible output for a 2 dimensional, 3 by 3 maze:

![1](https://user-images.githubusercontent.com/75276147/135773161-682a12fc-dca0-4d10-81ca-0a238949fb7e.png)

this can be thought of as a collection of nodes. Since there are 2 dimensions, each node's position is represented by two numbers.
The blue shows the node at position (1, 0).

![2](https://user-images.githubusercontent.com/75276147/135773164-6af1f676-0043-4d10-bf42-d09e6daeb304.png)

Each node has a set of neighboors. The green nodes are all neighboors of (1, 0).

![3](https://user-images.githubusercontent.com/75276147/135773167-dccc06e8-ee01-4da7-b871-5d08699f95b9.png)

In this case, its neighboors are (0, 0), (1, 1), and (2, 0).  
Each node's neighboors can either be connected or unconnected to it. (0, 0) and (1, 1) are connected to (1, 0), but (2, 0) is not.

When maze generation starts, all nodes are first unconnected to each other.

![4](https://user-images.githubusercontent.com/75276147/135773177-451be653-55d2-44aa-8c6a-c85e4d656d13.png)

On the first iteration of maze generation, the origin (0, 0) randomly chooses one of its neighboors to connect to.

![5](https://user-images.githubusercontent.com/75276147/135773126-c8c067fe-1055-411b-8987-d4e7f76096d6.png)

A list is kept of all nodes that have been connected to so far. Right now (0, 0) and (1, 0) are on the list.  
Every iteration, a connected node is randomly chosen and connects to one of its unconnected neighboors.  
But a connected node can only be selected if one of its neighboors is unconnected. These nodes are 'possible bases'.  
No two already connected nodes will connect to each other, so no loops like this are formed:

![6](https://user-images.githubusercontent.com/75276147/135773077-4631e64c-dfe1-4615-8e8e-af7ca63abd5a.png)

The amount of iterations to be done is equal to (the total number of nodes - 1). All nodes will be connected when generation is finished.

Because of this, it doesn't really matter what start or end point you choose for a generated maze: any given node can travel to another given node.  
And there will only be one path between two nodes that doesn't intersect itself.
