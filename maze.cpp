#include <iostream>
#include <fstream>
#include <vector>
#include <time.h>
#include <string>

// monochrome bitmap library: (has to be on compiler include path)
// https://github.com/pertbanking/bitmap-monochrome
#include "bitmap.h"

using namespace std;

// helper function for allocating memory of given type and length
template <typename T>
T* givenew(unsigned length) {
    T* val = new (std::nothrow) T[length];
    if (val == nullptr || length < 1) {
        std::cout << "\nmemory allocation failed, exiting\n";
        exit(0);
    }
    return val;
}

// overload of givenew<>() with a fill value
template <typename T>
T* givenew(unsigned length, T fill){
	T* val = givenew<T>(length);
	for (unsigned i = 0; i < length; i++){
		val[i] = fill;
	}
	return val;
}

// helper function for copying array
template <typename T>
T* copy(T* original, unsigned length) {
	T* result = givenew<T>(length);
    for (unsigned i = 0; i < length; i++) {
        result[i] = original[i];
    }
	return result;
}

// helper function for counting occurrences of a value in vector
template <typename T>
int count_occurrences(vector<T> v, T val){
	int count = 0;
	for (T i : v){
		if (i == val){
			count++;
		}
	}
	return count;
}

class maze{
	private:
		unsigned dimension_num, node_num, line_num;
		
		// stores size of each dimension of maze
		int* sizes;
		
		// stores each line state
		vector<bool> lines;
		
		// stores number of total lines for each direction
		// makes line_to_index() easier/faster
		int* dir_sizes;
		
		// variables used during generation process
		vector<bool> connected_nodes;
		vector<bool> possible_bases;
		unsigned possible_base_num = 0;
		
		// checks that position is inbounds
		// assumes that pos0's length = dimension_num
		bool inbounds(int* pos0){
			for (int i = 0; i < dimension_num; i++){
				if (pos0[i] < 0 || pos0[i] >= sizes[i]){
					return false;
				}
			}
			return true;
		}
		
		// checks that two positions are neighboors
		// assumes that both pos's lengths = dimension_num
		bool has_line(int* pos0, int* pos1){
			if (!inbounds(pos0) || !inbounds(pos1)){
				return false;
			}
			
			bool found_difference = false;
			for (int i = 0; i < dimension_num; i ++){
				if (pos0[i] != pos1[i]){
					if (found_difference){
						return false;
					}
					if (abs(pos0[i] - pos1[i]) == 1){
						found_difference = true;
					} else{
						return false;
					}
				}
			}
			return found_difference;
		}
		
		// get index of given node position
		// used to index vectors connected_nodes and possible_bases
		int node_to_index(int* pos0){
			int sum = 0;
			unsigned m = 1;
			for (int i = 0; i < dimension_num; i++){
				sum += pos0[i] * m;
				m = m * sizes[i];
			}
			return sum;
		}
		
		//reverse of node_to_index()
		int* index_to_node(int index){
			unsigned m = 1;
			for (int i = 0; i < dimension_num; i++){
				m = m * sizes[i];
			}
			
			int* result = givenew<int>(dimension_num, 0);
			for (int i = dimension_num - 1; i >= 0; i--){
				m = m / sizes[i];
				result[i] = index / m;
				index = index % m;
			}
			return result;
		}
		
		// get index for base position and line direction
		int line_to_index(int* pos0, int direction){
			int sum = 0;
			for (int i = 0; i < direction; i++){
				sum += dir_sizes[i];
			}
			
			int m = 1;
			for (int i = 0; i < dimension_num; i++){
				sum += m * pos0[i];
				
				int s = sizes[i];
				if (i == direction){
					s--;
				}
				m = m * s;
			}
			
			return sum;
		}
		
		// get index for line between two positions
		int line_to_index(int* pos0, int* pos1){
			int direction = 0;
			int* line_base = nullptr;
			for (int i = 0; i < dimension_num; i++){
				if (pos0[i] != pos1[i]){
					direction = i;
					if (pos0[i] < pos1[i]){
						line_base = pos0;
					} else{
						line_base = pos1;
					}
				}
			}
			return line_to_index(line_base, direction);
		}
		
		// returns vector of all positions pos0 can connect to
		vector<int*> get_connectable_nodes(int* pos0){
			vector<int*> list (0);
			for (int dir = 0; dir < dimension_num; dir++){
			for (int sign = -1; sign <=1; sign += 2){
				int* pos1 = copy<int>(pos0, dimension_num);
				pos1[dir] += sign;
				if (inbounds(pos1)){
					int pos1_index = node_to_index(pos1);
				if (!connected_nodes[pos1_index]){
					list.push_back(pos1);
				} else{ delete[] pos1;}
				} else{ delete[] pos1;}
			}}
			return list;
		}
		
		// assumes given nodes are neighboors
		//		and pos0 is the root of connection
		// takes care of:
		//		setting bit in lines[]
		//		adding pos1 to connected_nodes
		//		updating possible_roots for pos1 and its neighboors
		// root refers to already connected node
		// end refers to unconnected node
		void connect_nodes(int* root, int* end){
			
			// set line between nodes to true
			int line_index = line_to_index(root, end);
			lines[line_index] = true;
			
			// mark end pos as connected in connected_nodes
			int end_index = node_to_index(end);
			connected_nodes[end_index] = true;
			
			// update if neighboors of end pos are now possible bases
			for (int dir = 0; dir < dimension_num; dir ++){
			for (int sign = -1; sign <= 1; sign += 2){
				int* n1 = copy<int>(end, dimension_num);
				n1[dir] += sign;
				if (inbounds(n1)){
					int n1_index = node_to_index(n1);
					if (possible_bases[n1_index]){
						possible_base_num--;
					}
					possible_bases[n1_index] = false;
				if (connected_nodes[n1_index]){
					
					// n1 is inbounds and connected,
					// so update if n1 is now a possible base
					// by checking if any of its neighboors
					// are inbounds and unconnected
					for (int ndir = 0; ndir < dimension_num; ndir++){
					for (int nsign = -1; nsign <= 1; nsign += 2){
						int* n2 = copy<int>(n1, dimension_num);
						n2[ndir] += nsign;
						if (inbounds(n2)){
							int n2_index = node_to_index(n2);
						if (!connected_nodes[n2_index]){
							if (!possible_bases[n1_index]){
								possible_base_num++;
							}
							possible_bases[n1_index] = true;
						}}
						delete[] n2;
					}}
				} else{
					// end pos has an inbounds and unconnected neighboor
					// so end pos is a possible base
					if (!possible_bases[end_index]){
						possible_base_num++;
						possible_bases[end_index] = true;
					}
				}
				}
				delete[] n1;
			}}
		}
	public:
	
		// assumes valid parameters
		// doesn't delete[] node_sizes, make sure to do it
		maze(int dimensions, int* node_sizes)
		{
			dimension_num = dimensions;
			sizes = copy<int>(node_sizes, dimension_num);
			
			node_num = 1;
			line_num = 0;
			for (int i = 0; i < dimension_num; i++){
				node_num = node_num * sizes[i];
				int lines_d = 1;
				for (int j = 0; j < dimension_num; j++){
					if (i == j){
						lines_d = lines_d * (sizes[j] - 1);
					} else{
						lines_d = lines_d * sizes[j];
					}
				}
				line_num += lines_d;
			}
			
			dir_sizes = givenew<int>(dimension_num);
			for (int i = 0; i < dimension_num; i++){
				dir_sizes[i] = (sizes[i] - 1) * (node_num / sizes[i]);
			}
			
			lines = vector<bool>(line_num);
			connected_nodes = vector<bool>(node_num, false);
			possible_bases = vector<bool>(node_num, false);
		}
		
		// randomly generates maze
		void generate(bool show_progress){
			
			srand(time(NULL));
			
			// set origin (0, 0, ...) as first node of tree
			possible_bases[0] = true;
			connected_nodes[0] = true;
			possible_base_num = 1;
			
			// loop of randomly connecting two nodes
			// until all nodes are connected
			for (int i = 0; i < node_num - 1; i++){
				
				// randomly choose root node
				int* root_node = nullptr;
				int r = rand() % possible_base_num;
				for (int j = 0; j < node_num; j++){
					if (possible_bases[j] == true){
						r--;
						if (r < 0){
							root_node = index_to_node(j);
							break;
						}
					}
				}
				
				// randomly choose end node
				vector<int*> p = get_connectable_nodes(root_node);
				int s = rand() % p.size();
				int* end_node = p[s];
				
				// connect root node to end node
				connect_nodes(root_node, end_node);
				
				// garbage collection of pointers used in this loop iteration
				delete[] root_node;
				for (int* pi : p){
					delete[] pi;
				}
			}
		}
		
		// only usable if 2 maze is dimensional
		PixelMatrix asPixelMatrix(){
			if (dimension_num != 2){
				cout << "\ntried to make pixel matrix with non 2d maze";
				PixelMatrix rval(1);
				return rval;
			}
			int width = sizes[0] * 2 + 1;
			int height = sizes[1] * 2 + 1;
			
			PixelMatrix result(width);
			
			for (int y = 0; y < height; y++){
				vector<Pixel> row(width, false);
				if (y == 0 || y == height - 1){
					for (int x = 0; x < width; x++){
						//maze border, filled space
						row[x] = true;
					}
				} else{
					for (int x = 0; x < width; x++){
						if (x == 0 || x == width - 1){
							//maze border, filled space
							row[x] = true;
						} else if( (x%2) &&  (y%2)){
							// node position, empty space
						} else if(!(x%2) && !(y%2)){
							// wall intersection, filled space
							row[x] = true;
						} else if( (x%2) && !(y%2)){
							int* node_base = givenew<int>(2);
							node_base[0] = (x - 1) / 2;
							node_base[1] = (y - 2) / 2;
							
							int line_index = line_to_index(node_base, 1);
							if (!lines[line_index]){
								row[x] = true;
							}
							
							delete[] node_base;
						} else{
							int* node_base = givenew<int>(2);
							node_base[0] = (x - 2) / 2;
							node_base[1] = (y - 1) / 2;
							
							int line_index = line_to_index(node_base, 0);
							if (!lines[line_index]){
								row[x] = true;
							}
							
							delete[] node_base;
						}
					}
				}
				result[y] = row;
			}
			return result;
		}
		
		~maze(){
			delete[] sizes;
			delete[] dir_sizes;
		}
};

int main(int argc, char** argv){
	int dimensions;
	cout << "maze dimensions: ";
	cin >> dimensions;
	if (dimensions < 1){ return 0;}
	
	int* sizes = givenew<int>(dimensions);
	cout << "sizes:\n";
	for (int i = 0; i < dimensions; i++){
		cout << "  ";
		cin >> sizes[i];
		if (sizes[i] < 1){ return 0;}
	}
	
	bool make_bitmap = false;
	string file_name;
	if (dimensions == 2){
		cout << "since there are 2 dimensions, "
			 << "do you want to make bmp image when done? y/n ";
		char ans = 0;
		cin >> ans;
		if (ans == 'y' || ans == 'Y'){
			make_bitmap = true;
			cout << "file name to save bitmap to: ";
			cin >> file_name;
		}
	}
	
	maze m(dimensions, sizes);
	
	m.generate(true);
	cout << "maze generation finished";
	
	if (make_bitmap){
		PixelMatrix pm = m.asPixelMatrix();
		Bitmap bm;
		bm.fromPixelMatrix(pm);
		bm.save(file_name);
		cout << " and saved to file";
	}
	
	delete[] sizes;
}