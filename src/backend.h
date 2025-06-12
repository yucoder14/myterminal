#include <string>
#include <vector>
#include <iostream>

using namespace std;

/*
	All the PTY (psuedo terminal) related things reside here. 
	When terminal reads data from the shell, the data is "tokenized" 
	and stored in the struct PtyData. Then, the Terminal will relay 
	this information to the grid to figure out how it's suppose to 
	format the data, such that it is screen ready.
*/
namespace PTY {
	typedef struct PtyData PtyData;

	enum PtyDataType {
		PRINTABLE,
		BACKSPACE,
		TAB,
		BELL,
		CARRAIGE,
		NEWLINE,
		ESCAPE,
		ANSI
	};

	struct PtyData {
		enum PtyDataType type; 
		char keycode;
		vector<char> ansicode;
	};
	void GetPtyData(PtyData *data, char b, vector<char> *tmp, bool *ESC, bool *CSI);
}

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
namespace GRID {
	using namespace PTY;
	typedef struct Cell Cell; 
	/*
		For now, the Cell only contains information regarding the keycode and 
		whether the current cell is a continuation of the previous line, but it 
		will get more complicated as I add more features (Are emojis too much to ask?)

		The Terminal should not directly "see" indiviual cells!
	*/
	struct Cell {
		char keycode; 
		bool lineBreak = false;
	};	

	class Grid {
	public:
		Grid();
		void ParseAnsiCode(PtyData *ansi);
		void SetGridElement(PtyData *data);
		void AddNewLine();
		void ResizeGrid(int rows, int cols);
	private:
		vector<vector<Cell>> grid;	 
		int gridHeight, gridWidth; 
		int cursorX, cursorY;
		int rowScroll;
	};	
}	
