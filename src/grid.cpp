#include "backend.h"

using namespace GRID;

Cell *GRID::Grid::GetGridElement(int cursorX, int cursorY) {
	return &grid.at(cursorY).at(cursorY);
}	


void GRID::Grid::AddNewLine() {
	 
}	

void GRID::Grid::ResizeGrid(int rows, int cols) {
	 
}	
