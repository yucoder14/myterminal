#include "backend.h"

void GRID::RenderGrid::EraseScreen(int startingIndex, int endingIndex) {
	int row, col;
	for (int i = startingIndex; i < endingIndex; i++) {
		row = i / renderGridWidth + renderGrid->GetRowTop(); 	
		col = i % renderGridWidth;
		renderGrid->ClearCell(row, col);
	}	
}	

void GRID::RenderGrid::ParseAnsiCode(PtyData *ansi) {
	string str(ansi->ansicode.begin(), ansi->ansicode.end());
	if (str == "C") {
		renderGrid->IncCursorX();
		renderCursorX++;		
	} else if (str=="K") {
		for (auto colIt = renderGrid->GetRowAtCursor()->begin() + renderGrid->GetCursorX(); 
				colIt != renderGrid->GetRowAtCursor()->end(); ++colIt) { 
			colIt->keycode = '\0';
		}	
	} else if (str == "H") {
		// currently this is doing nothing...
		MoveRenderCursor(0, 0);
	} else if (str == "J" || str == "0J") { 
		// erase from cursor until end of screen	
		EraseScreen(renderCursorY * renderGridWidth + renderCursorX,
			renderGridHeight * renderGridWidth);
	} else if (str == "1J") { 
		// erase from cursor to beginning of screen
		EraseScreen(0, renderCursorY * renderGridWidth + renderCursorX);
	} else if (str == "2J") {
		// erase entire screen
		EraseScreen(0, renderGridHeight * renderGridWidth);
	} else if (str == "3J") { 
		// erase from cursor to beginning of screen
	} else if (str == "?1049h") {
		// now the renderGrid should point to altGrid
		ToggleAltGrid(); 
	} else if (str == "?1049l") {
		// clear the altGrid 
		renderGrid->ClearGrid(); // this crashes?  
		// now the renderGrid should point to mainGrid
		ToggleAltGrid(); 
//		grid->clear();
//		grid->resize(gridHeight, vector<Cell>(gridWidth));
	}
}	
