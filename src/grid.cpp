#include "backend.h"

using namespace GRID;

/*** Getters **********************************************************/

Cell *GRID::Grid::GetGridElement(int row, int col) {
	return &grid.at(row).at(col);
}	

int GRID::Grid::GetCursorX() {
	return cursorX; 
}	

int GRID::Grid::GetCursorY() {
	return cursorY;
}	

int GRID::Grid::GetRowTop() {
	return rowTop;
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

/*** Checkers *********************************************************/

bool GRID::Grid::isNull(int row, int col) {
	return grid.at(row).at(col).keycode == '\0';
}	

/*** Setters **********************************************************/

void GRID::Grid::SetGridElement(char data) {
	grid.at(cursorY).at(cursorX).keycode = data;	
}	

void GRID::Grid::ClearCell(int row, int col) {
	grid.at(row).at(col).keycode = '\0';
}	

void GRID::Grid::SetLineBreak() {
	grid.at(cursorY).at(cursorX).lineBreak = true; 
}	

/*** Cursor Manipulations *********************************************/

void GRID::Grid::ZeroCursorX() {
	cursorX = 0;
}	

void GRID::Grid::ZeroCursorY() {
	cursorY = 0;
}	

void GRID::Grid::IncCursorX(int inc) {
	cursorX += inc;
}	

void GRID::Grid::IncCursorY(int inc) {
	cursorY += inc;
}	

void GRID::Grid::DecCursorX(int dec) {
	cursorX -= dec;
}	

void GRID::Grid::DecCursorY(int dec) {
	cursorY -= dec;
}	

void GRID::Grid::MoveCursor(int row, int col) {
	cursorX = col;
	cursorY = row + rowTop;
}	

/*** Miscellaneous ****************************************************/

void GRID::Grid::IncRowTop(int numRows) {
	rowTop += numRows;
}	

void GRID::Grid::AddNewLine(bool addTop) {
	vector<Cell> newline;
	newline.resize(GetNumCols());
	grid.push_back(newline);
	if (addTop) {
		rowTop++;
	}	
}	

// TODO: Implement this
void GRID::Grid::ResizeGrid(int rows, int cols) {
	grid.resize(rows, vector<Cell>(cols));
}	
