#include <string>
#include <vector>
#include <tuple>
#include <deque>
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
		char keycode = 0; 
		bool lineBreak = false;
	};	

	
	/* 
		Grid is the class that handles all the grid related manipulations
		cursor of the Grid is not to be confused or to be treated equally as 
		the cursor of ResizeGrid. The Grid cursor is only to be used when 
		parsing PtyData and populating the actual grid. It should not be 
		modified during any other times. 

		Do I worry about line breaks here? One approach is to have unbroken 
		lines of texts/symbols stored in the Grid and handle line breaking 
		during rendering -> this approach does not require me to resize and 
		join broken lines whenever the window is resized. Under this approach, 
		when I'm trying to render symbols on screen, i can iterate through lines 
		and handle line breaking then. A con of this approach is that the 
		conversion between screen coordinates (i.e. renderCursor) and grid coordinate
		becomes less straight forward, but i might not need it anyways... 
		- it is still doable if i can keep track of how many line breaks there
		  are and render from bottom to top 

		Another approach is to resize the grid and handle line breaking on 
		the Grid. Under this approach, the conversion between renderCursor and 
		grid's cursor is more straight forward; I just need to account for how
		much scrolling happened. A con of this approach is that i would have to 
		resize Grid everytime the windwow is resized (assuming that resizing does
		not happen often, this approach is more favorable...); in order to resize
		the window, I have to go through every row of the Grid, even if it may not
		end up on the screen...	
		- but considering that the cha
	*/
	class Grid {
	public: 
		Grid(int rows, int cols) {
			grid.resize(rows, vector<Cell>(cols));

			screenHeight = rows;
			screenWidth = cols;
			cursorX = 0;
			cursorY = 0;
			rowScroll = 0;
		}	

		Cell *GetGridElement(int row, int col);
		int GetCursorX();
		int GetCursorY();
		int GetRowScroll();
		int GetNumRows();
		int GetNumCols();
		vector<Cell> *GetRowAtCursor();

		void SetGridElement(char data);
		void SetLineBreak();
		
		void ZeroCursorX();
		void ZeroCursorY();
		void IncCursorX();
		void IncCursorY();
		void DecCursorX();
		void DecCursorY();

		void IncRowScroll(int numRows);
		
		void AddNewLine(bool addScroll);
		void ResizeGrid(int rows, int cols);
		void ClearGrid();
	private:
		vector<vector<Cell>> grid;	 
		int cursorX, cursorY;
		int screenHeight, screenWidth;
		int rowScroll;
	};	

	/* 
		Render grid is the interface between the two grids (main and alt) 
		and the Terminal; it's weird to call it an interface, but the idea
		is that this class will get data ("instructions?") from the Terminal 
		and propagate it to the appropriate grid, which will then do necessary
		parsing/manipulations
	  	the renderCursor is what is used to get the elements from Grid's to then 
		draw it on the screen 
	*/
	class RenderGrid {
	public:
		RenderGrid(int rows, int cols) {
			mainGrid.ResizeGrid(rows, cols);
			altGrid.ResizeGrid(rows, cols);
			renderGridHeight = rows;
			renderGridWidth = cols;
			renderGrid = &mainGrid;
		}	

		int GetRenderGridHeight();
		int GetRenderGridiWidth();
		Cell *GetRenderGridElement(int renderCursorX, int renderCursorY);
		void GetRenderGridDimensions(int *height, int *width);

		void SetRenderGridElement(PtyData *data);

		void MoveRenderCursor(int row, int col);
		void ToggleAltGrid();
		void ParseAnsiCode(PtyData *ansi);

		void FormatRawData(deque<PtyData> *rawData); 
		void ResizeRenderGrid(int rows, int cols);

	private:
//		Cursor cursor; --> to indicate where the cursor 
		Grid mainGrid = Grid(0, 0);
		Grid altGrid = Grid(0, 0);
		Grid *renderGrid;

		// these values should be screen coordinates
		int renderCursorX, renderCursorY;
		
		// these values should be values derived from window sizes
		int renderGridHeight, renderGridWidth;

	};	
}	
