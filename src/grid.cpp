#include "backend.h"

using namespace GRID;

/*** Getters **********************************************************/

Cell *GRID::Grid::GetGridElement(int row, int col) {
	return &grid.at(row).at(col);
}	

int GRID::Grid::GetCaretX() {
	return caretX; 
}	

int GRID::Grid::GetCaretY() {
	return caretY;
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

vector<Cell> *GRID::Grid::GetRowAtCaret() {
	return &grid.at(caretY);
}	

/*** Checkers *********************************************************/

bool GRID::Grid::isNull(int row, int col) {
	return grid.at(row).at(col).keycode == '\0';
}	

/*** Setters **********************************************************/

void GRID::Grid::SetGridElement(char data) {
	grid.at(caretY).at(caretX).keycode = data;	
}	

void GRID::Grid::ClearCell(int row, int col) {
	grid.at(row).at(col).lineBreak = false;
	grid.at(row).at(col).keycode = '\0';
}	

void GRID::Grid::SetLineBreak() {
	grid.at(caretY).at(caretX).lineBreak = true; 
}	

/*** Caret Manipulations *********************************************/

void GRID::Grid::ZeroCaretX() {
	caretX = 0;
}	

void GRID::Grid::ZeroCaretY() {
	caretY = 0;
}	

void GRID::Grid::IncCaretX(int inc) {
	caretX += inc;
}	

void GRID::Grid::IncCaretY(int inc) {
	caretY += inc;
}	

void GRID::Grid::DecCaretX(int dec) {
	caretX -= dec;
}	

void GRID::Grid::DecCaretY(int dec) {
	caretY -= dec;
}	

void GRID::Grid::MoveCaret(int row, int col) {
	caretX = col;
	caretY = row + rowTop;
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
