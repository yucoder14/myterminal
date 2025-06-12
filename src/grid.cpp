#include "backend.h"

GRID::Grid::Grid() {
	cursorX = 0;
	cursorY = 0;
}	

void GRID::Grid::ParseAnsiCode(PtyData *ansi) {
	 
}	

void GRID::Grid::SetGridElement(PtyData *data) {
	switch (data->type) {
		case PRINTABLE:
			grid[cursorY][cursorX].keycode = data->keycode; 
			cursorX++;
			if (cursorX == grid.begin()->size()) {
				cursorY++;
				cursorX = 0; 
				AddNewLine();
				grid[cursorY][cursorX].lineBreak = true;
			}	
			break;
		case BACKSPACE:
			cursorX--;
			break;
		case TAB:
			break;
		case BELL:
			break;
		case CARRAIGE:
			cursorX = 0;
			break;
		case NEWLINE:
			{ 
				cursorY++; 
				if (cursorY == grid.size()) {
					AddNewLine();
				}	
			}
			break;
		case ESCAPE:
			break;
		case ANSI:
			ParseAnsiCode(data); 
			break;
	}
	 
}	

void GRID::Grid::AddNewLine() {
	vector<Cell> newline;
	newline.resize(gridWidth);
	grid.push_back(newline);
	rowScroll++;
}	

void GRID::Grid::ResizeGrid(int rows, int cols) {
	 
}	
