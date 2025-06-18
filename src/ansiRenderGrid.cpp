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
		case 0: // erase from cursor to end of line
			startingIndex = renderCursorX;
			endingIndex = renderGridWidth;
			break;
		case 1: // erase start of line to the cursor
			startingIndex = 0;
			endingIndex = renderCursorX;
			break;
		case 2: // erase entire line
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

void GRID::RenderGrid::SetPrivateMode(int arg) {
	switch(arg) { 
		case 1049: // set alternate screen 
			renderGrid = &altGrid;
			renderCursorXPrev = renderCursorX;
			renderCursorYPrev = renderCursorY;
			break;
		default: 
			break;
	}	
}	

void GRID::RenderGrid::UnsetPrivateMode(int arg) {
	switch(arg) { 
		case 1049: // unset alternate screen 
			renderGrid = &mainGrid;
			renderCursorX = renderCursorXPrev;
			renderCursorY = renderCursorYPrev;
			break;
		default: 
			break;
	}	
}	

void GRID::RenderGrid::DeleteCharacters(int arg) {
	int row = renderCursorY;
	int col = renderGridWidth - 1; 

	while (renderGrid->isNull(row, col)) {
		col -= 1;
	}	

	for (int i = 0; i < arg; i++) {
		renderGrid->ClearCell(row, col);
		col -= 1;	
	}	
}	

void GRID::RenderGrid::SetAnsi(PtyData *ansi) {
	string str(ansi->ansicode.begin(), ansi->ansicode.end());
	PANSI parsedAnsi = ParseAnsiString(str); 
	int arg, row, col;

	if (parsedAnsi.privateMode) {
		switch (parsedAnsi.mode) {	
			case 'h':
				arg = parsedAnsi.args[0];
				SetPrivateMode(arg);
				break;
			case 'l': 
				arg = parsedAnsi.args[0];
				UnsetPrivateMode(arg);
				break;
		}	

		return;
	}	

	switch (parsedAnsi.mode) {
		case 'C': // move cursor # to the right 
			arg = (parsedAnsi.args.empty()) ? 1 : parsedAnsi.args[0];
			renderGrid->IncCursorX(arg);
			renderCursorX += arg;		
			break;
		case 'H': // move cursor to row #, col #
			row = (parsedAnsi.args.empty()) ? 0 : parsedAnsi.args[0] - 1;
			col = (parsedAnsi.args.empty()) ? 0 : parsedAnsi.args[1] - 1;
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
		case 'P': // delete # characters from right to left
			arg = (parsedAnsi.args.empty()) ? 0 :  parsedAnsi.args[0];
			DeleteCharacters(arg);
			break;
	}	
}	
