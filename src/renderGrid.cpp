#include "backend.h" 

using namespace GRID;
int GRID::RenderGrid::GetRenderGridHeight() {
	return renderGridHeight; 
}	

int GRID::RenderGrid::GetRenderGridiWidth() {
	return renderGridWidth;
}	

Cell *GRID::RenderGrid::GetRenderGridElement(int row, int col) {
	int newRow = row + renderGrid->GetRowScroll();

	return renderGrid->GetGridElement(newRow, col);	
}	

void GRID::RenderGrid::GetRenderGridDimensions(int *height, int *width) {
	*height = renderGrid->GetNumRows();	
	*width = renderGrid->GetNumCols();
}	

void GRID::RenderGrid::MoveRenderCursor(int row, int col) {
	renderCursorX = col; 
	renderCursorY = row;
}	

void GRID::RenderGrid::ToggleAltGrid() {	 
	if (renderGrid == &mainGrid) {
		renderGrid = &altGrid;
	} else {	
		renderGrid = &mainGrid;
	}	
}	

void GRID::RenderGrid::ParseAnsiCode(PtyData *ansi) {
	string str(ansi->ansicode.begin(), ansi->ansicode.end());

	// very basic ... i feel like theses should be function pointers at some point down the line
	// maybe like a hash table of function pointers ? 
	if (str=="K") {
		for (auto colIt = renderGrid->GetRowAtCursor()->begin() + renderGrid->GetCursorX(); 
				colIt != renderGrid->GetRowAtCursor()->end(); ++colIt) { 
			colIt->keycode = '\0';
		}	
	} else if (str == "H") {
		MoveRenderCursor(0, 0);
	} else if (str == "J") {
		int numRowsToAdd = (
				renderGrid->GetCursorY() - renderGrid->GetRowScroll()
				) % renderGridHeight;
		renderGrid->IncRowScroll(numRowsToAdd);
		for (int i = 0; i < numRowsToAdd; i++) {
			renderGrid->AddNewLine(false);
		}	
	} else if (str == "?1049h") {
		// now the renderGrid should point to altGrid
		ToggleAltGrid(); 
	} else if (str == "?1049l") {
		// clear the altGrid 
		renderGrid->ClearGrid();  
		// now the renderGrid should point to mainGrid
		ToggleAltGrid(); 
//		grid->clear();
//		grid->resize(gridHeight, vector<Cell>(gridWidth));
	}
}	

void GRID::RenderGrid::SetRenderGridElement(PtyData *data) {
	switch (data->type) {
		case PRINTABLE:
			renderGrid->SetGridElement(data->keycode);
			renderGrid->IncCursorX();
			if (renderGrid->GetCursorX() == renderGrid->GetNumCols()) {
				renderGrid->ZeroCursorX();
				renderGrid->IncCursorY(); 
				if (renderGrid->GetCursorY() == 
						renderGrid->GetNumRows()) {
					renderGrid->AddNewLine(true);
				}
				renderGrid->SetLineBreak();
			}	
			break;
		case BACKSPACE:
			renderGrid->DecCursorX();
			break;
		case TAB:
			break;
		case BELL:
			break;
		case CARRAIGE:
			renderGrid->ZeroCursorX();
			break;
		case NEWLINE:
			renderGrid->IncCursorY(); 
			if (renderGrid->GetCursorY() == 
					renderGrid->GetNumRows()) {
				renderGrid->AddNewLine(true);
			}
			break;
		case ESCAPE:
			break;
		case ANSI:
			ParseAnsiCode(data);
			break;
	}
	
}	

void GRID::RenderGrid::FormatRawData(deque<PtyData> *rawData) {
	while (!rawData->empty()) {
		PtyData data = rawData->at(0);
		SetRenderGridElement(&data);
		rawData->pop_front();
	}
}	

void GRID::RenderGrid::ResizeRenderGrid(int rows, int cols) {
	renderGridHeight = rows; 
	renderGridWidth = cols;

	mainGrid.ResizeGrid(rows, cols); 
	altGrid.ResizeGrid(rows, cols); 
}	
