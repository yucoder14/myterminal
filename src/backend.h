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

	Grid is what handles caret movements, scrolling, and other tasks related to 
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
			caretX = 0;
			caretY = 0;
			rowTop = 0;
		}	

		Cell *GetGridElement(int row, int col);
		int GetCaretX();
		int GetCaretY();
		int GetRowTop();
		int GetNumRows();
		int GetNumCols();
		vector<Cell> *GetRowAtCaret();

		bool isNull(int row, int col);

		void SetGridElement(char data);
		void ClearCell(int row, int col);
		void SetLineBreak();
		
		void ZeroCaretX();
		void ZeroCaretY();
		void IncCaretX(int inc);
		void IncCaretY(int inc);
		void DecCaretX(int dec);
		void DecCaretY(int dec);
		void MoveCaret(int row, int col);

		void IncRowTop(int numRows);
		void AddNewLine(bool addTop);
		void ResizeGrid(int rows, int cols);
	private:
		vector<vector<Cell>> grid;	 
		int caretX, caretY;
		int screenHeight, screenWidth;
		int rowTop;
	};	

	/* 
		Render grid is the interface between the two grids (main and alt) 
		and the Terminal; it's weird to call it an interface, but the idea
		is that this class will get data ("instructions?") from the Terminal 
		and propagate it to the appropriate grid, which will then do necessary
		parsing/manipulations
	  	the renderCaret is what is used to get the elements from Grid's to then 
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
		Cell *GetRenderGridElement(int renderCaretX, int renderCaretY);
		void GetRenderGridDimensions(int *height, int *width);
		void GetRenderGridCaret(int *row, int *col);

		void MoveRenderCaret(int row, int col);
		void SetRenderGridElement(PtyData *data);
		void SetPrintable(PtyData *data);
		void SetBackspace();
		void SetTab();
		void SetCarraige();
		void SetNewline();
		void SetEscape();

		void EraseScreen(int arg);
		void EraseLine(int arg);
		void SetPrivateMode(int arg);
		void UnsetPrivateMode(int arg);
		void DeleteCharacters(int arg);
		void SetAnsi(PtyData *ansi);

		void FormatRawData(deque<PtyData> *rawData); 
		void ResizeRenderGrid(int rows, int cols);

	private:
//		Caret caret; --> to indicate where the caret 
		Grid mainGrid = Grid(0, 0);
		Grid altGrid = Grid(0, 0);
		Grid *renderGrid;

		// these values should be screen coordinates
		int renderCaretX, renderCaretY;
		int renderCaretXPrev, renderCaretYPrev;
		
		// these values should be values derived from window sizes
		int renderGridHeight, renderGridWidth;

	};	
}	
