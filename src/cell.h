typedef struct Cell Cell; 
struct Cell {
	char keycode; 
	bool lineBreak = false;
};	

// TODO: refactor code to abstract away grid
// as a separate class...
//class Grid {
//	...
//private:
//	vector<vector<Cell>> grid;	 
//	int grid_height, grid_width; 
//	int cursorX, cursorY;
//	int rowScroll;
//};	
