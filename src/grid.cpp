#include "backend.h"

using namespace GRID;

Cell *GRID::Grid::GetGridElement(int row, int col) {
	return &grid.at(row).at(col);
}	

int GRID::Grid::GetCursorX() {
	return cursorX; 
}	

int GRID::Grid::GetCursorY() {
	return cursorY;
}	

int GRID::Grid::GetRowScroll() {
	return rowScroll;
}	

int GRID::Grid::GetNumRows() {
	return grid.size();
}	

int GRID::Grid::GetNumCols() {
	return grid.at(0).size();
}	

vector<Cell> *GRID::Grid::GetRowAtCursor() {
	return &grid.at(cursorY);
}	

void GRID::Grid::SetGridElement(char data) {
	grid.at(cursorY).at(cursorX).keycode = data;	
}	

void GRID::Grid::SetLineBreak() {
	grid.at(cursorY).at(cursorX).lineBreak = true; 
}	

void GRID::Grid::ZeroCursorX() {
	cursorX = 0;
}	

void GRID::Grid::ZeroCursorY() {
	cursorY = 0;
}	

void GRID::Grid::IncCursorX() {
	cursorX ++;
}	

void GRID::Grid::IncCursorY() {
	cursorY ++;
}	

void GRID::Grid::DecCursorX() {
	cursorX --;
}	

void GRID::Grid::DecCursorY() {
	cursorY --;
}	

void GRID::Grid::AddNewLine() {
	IncCursorY(); 
	if (GetCursorY() == GetNumRows()) {
		vector<Cell> newline;
		newline.resize(GetNumCols());
		grid.push_back(newline);
		rowScroll++;
	}
}	

void GRID::Grid::ResizeGrid(int rows, int cols) {
	grid.resize(rows, vector<Cell>(cols));
}	

void GRID::Grid::ClearGrid() {
	grid.clear();
	grid.resize(screenHeight, vector<Cell>(screenWidth));
}	

