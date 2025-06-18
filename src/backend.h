#include <string>
#include <vector>
#include <tuple>
#include <deque>
#include <iostream>

#include "ptyData.h"
#include "ansi.h"

using namespace std;

/* 
	The Grid consists of many Cells, which contain various information about each
	cell in the grid.

	Grid is what handles cursor movements, scrolling, and other tasks related to 
	rendering. Grid should set up its parameters just right, so that the Terminal
	can just "read off" of Grid and just draw characters
*/
namespace GRID {
	typedef struct Cell Cell; 

	struct Cell {
		char keycode = 0; 
		bool lineBreak = false;
	};	
	
	class Grid {
	public: 
		Grid(int rows, int cols) {
			grid.resize(rows, vector<Cell>(cols));
			screenHeight = rows;
			screenWidth = cols;
			cursorX = 0;
			cursorY = 0;
			rowTop = 0;
		}	

		Cell *GetGridElement(int row, int col);
		int GetCursorX();
		int GetCursorY();
		int GetRowTop();
		int GetNumRows();
		int GetNumCols();
		vector<Cell> *GetRowAtCursor();

		bool isNull(int row, int col);

		void SetGridElement(char data);
		void ClearCell(int row, int col);
		void SetLineBreak();
		
		void ZeroCursorX();
		void ZeroCursorY();
		void IncCursorX(int inc);
		void IncCursorY(int inc);
		void DecCursorX(int dec);
		void DecCursorY(int dec);
		void MoveCursor(int row, int col);

		void IncRowTop(int numRows);
		
		void AddNewLine(bool addTop);
		void ResizeGrid(int rows, int cols);
		void ClearGrid();
	private:
		vector<vector<Cell>> grid;	 
		int cursorX, cursorY;
		int screenHeight, screenWidth;
		int rowTop;
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

		// functions related to ansi codes 
		void EraseScreen(int arg);
		void EraseLine(int arg);
		void PrivateModeSet(int arg);
		void PrivateModeUnset(int arg);
		void DeleteCharacters(int arg);
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
		int renderCursorXPrev, renderCursorYPrev;
		
		// these values should be values derived from window sizes
		int renderGridHeight, renderGridWidth;

	};	
}	
