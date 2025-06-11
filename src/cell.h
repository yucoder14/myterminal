/*
	For now, the Cell only contains information regarding the keycode and 
	whether the current cell is a continuation of the previous line, but it 
	will get more complicated as I add more features (Are emojis too much to ask?)

	The Terminal should not directly "see" indiviual cells!
*/
typedef struct Cell Cell; 

struct Cell {
	char keycode; 
	bool lineBreak = false;
};	


/* 
	The Grid consists of many Cells. As of now, it is a two dimensional vector
	of Cells. It made the most sense to have it be represented as a two dimensional
	array with all the cursor movements. 

	Would it be possible to implement it using one dimensional vector? Maybe? but
	I think it would unnecessarily complicate the cursor movements...

	Grid is what handles cursor movements, scrolling, and other tasks related to 
	rendering. Grid should set up its parameters just right, so that the Terminal
	can just "read off" of Grid and just draw characters
*/
// TODO: refactor code to abstract away grid
// as a separate class...
//class Grid {
//public:
//	some constructor?
//	SetGridElement(...)
//	ResizeGrid(...)
//private:
//	vector<vector<Cell>> grid;	 
//	int grid_height, grid_width; 
//	int cursorX, cursorY;
//	int rowScroll;
//};	
