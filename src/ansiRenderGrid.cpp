#include "backend.h"

void GRID::RenderGrid::EraseScreen(int arg) {
	int row, col, startingIndex, endingIndex;

	switch (arg) {
		case 0: // erase from cursor until end of screen
			startingIndex = renderCursorY * renderGridWidth + renderCursorX;
			endingIndex = renderGridHeight * renderGridWidth;
			break;
		case 1: // erase from cursor to beginning of screen
			startingIndex = 0;	
			endingIndex = renderCursorY * renderGridWidth + renderCursorX;
			break;
		case 2: // erase entire screen
			startingIndex = 0;	
			endingIndex = renderGridHeight * renderGridWidth;
			break;
		case 3:	// erase saved lines
			return;
			break;
		default: // should not reach this case... 
			return;
			break;
	}	

	for (int i = startingIndex; i < endingIndex; i++) {
		row = i / renderGridWidth + renderGrid->GetRowTop(); 	
		col = i % renderGridWidth;
		renderGrid->ClearCell(row, col);
	}	
}	

void GRID::RenderGrid::EraseLine(int arg) {
	int startingIndex, endingIndex;

	switch (arg) {
		case 0:
			startingIndex = renderCursorX;
			endingIndex = renderGridWidth;
			break;
		case 1:
			startingIndex = 0;
			endingIndex = renderCursorX;
			break;
		case 2:
			startingIndex = 0;
			endingIndex = renderGridWidth;
			break;
		default: // should not reach this case
			break;
	}	

	for (int i = startingIndex; i < endingIndex; i++) {
		renderGrid->ClearCell(renderCursorY, i);
	}	
}	

void GRID::RenderGrid::ParseAnsiCode(PtyData *ansi) {
	string str(ansi->ansicode.begin(), ansi->ansicode.end());
	PANSI parsedAnsi = ParseAnsiString(str); 
	int arg, row, col;

	if (parsedAnsi.privateMode) {
		switch (parsedAnsi.mode) {	
			case 'h':
				cout << parsedAnsi.args[0] << endl;
				//PrivateModeH(arg);
				break;
			case 'l': 
				cout << parsedAnsi.args[0] << endl;
				//PrivateModeL(arg);
				break;
		}	

		return;
	}	

	switch (parsedAnsi.mode) {
		case 'C': // move cursor to the right 
			renderGrid->IncCursorX();
			renderCursorX++;		
			break;
		case 'H': // move cursor to row #, col #
			row = (parsedAnsi.args.empty()) ? 0 : parsedAnsi.args[0];
			col = (parsedAnsi.args.empty()) ? 0 : parsedAnsi.args[1];
			MoveRenderCursor(row, col);
			break;
		case 'J': // erase display
			arg = (parsedAnsi.args.empty()) ? 0 :  parsedAnsi.args[0];
			EraseScreen(arg);
			break;
		case 'K': // erase line 
			arg = (parsedAnsi.args.empty()) ? 0 :  parsedAnsi.args[0];
			EraseLine(arg);
			break;
	}	

	if (str == "?1049h") {
		// now the renderGrid should point to altGrid
		ToggleAltGrid(); 
	} else if (str == "?1049l") {
		// clear the altGrid 
		renderGrid->ClearGrid(); // this crashes?  
		// now the renderGrid should point to mainGrid
		ToggleAltGrid(); 
	}
}	
