#include "backend.h"

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
		case 'C': // move caret # to the right 
			arg = (parsedAnsi.args.empty()) ? 1 : parsedAnsi.args[0];
			renderGrid->IncCaretX(arg);
			renderCaretX += arg;		
			break;
		case 'H': // move caret to row #, col #
			row = (parsedAnsi.args.empty()) ? 0 : parsedAnsi.args[0] - 1;
			col = (parsedAnsi.args.empty()) ? 0 : parsedAnsi.args[1] - 1;
			MoveRenderCaret(row, col);
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

/*** Helper Functions *************************************************/

void GRID::RenderGrid::EraseScreen(int arg) {
	int row, col, startingIndex, endingIndex;

	switch (arg) {
		case 0: // erase from caret until end of screen
			startingIndex = renderCaretY * renderGridWidth + renderCaretX;
			endingIndex = renderGridHeight * renderGridWidth;
			break;
		case 1: // erase from caret to beginning of screen
			startingIndex = 0;	
			endingIndex = renderCaretY * renderGridWidth + renderCaretX;
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
	int startingIndex, endingIndex, row;
	row = renderCaretY + renderGrid->GetRowTop();

	switch (arg) {
		case 0: // erase from caret to end of line
			startingIndex = renderCaretX;
			endingIndex = renderGridWidth;
			break;
		case 1: // erase start of line to the caret
			startingIndex = 0;
			endingIndex = renderCaretX;
			break;
		case 2: // erase entire line
			startingIndex = 0;
			endingIndex = renderGridWidth;
			break;
		default: // should not reach this case
			break;
	}	

	for (int i = startingIndex; i < endingIndex; i++) {
		renderGrid->ClearCell(row, i);
	}	
}	

void GRID::RenderGrid::SetPrivateMode(int arg) {
	switch(arg) { 
		case 1049: // set alternate screen 
			renderGrid = &altGrid;
			renderCaretXPrev = renderCaretX;
			renderCaretYPrev = renderCaretY;
			break;
		default: 
			break;
	}	
}	

void GRID::RenderGrid::UnsetPrivateMode(int arg) {
	switch(arg) { 
		case 1049: // unset alternate screen 
			renderGrid = &mainGrid;
			renderCaretX = renderCaretXPrev;
			renderCaretY = renderCaretYPrev;
			break;
		default: 
			break;
	}	
}	

void GRID::RenderGrid::DeleteCharacters(int arg) {
	int row = renderCaretY;
	int col = renderCaretX; 

	while (!renderGrid->isNull(row, col)) {
		col++;
	}	

	for (int i = col - 1; i > col - arg - 1; i--) {
		cout << "deleting cells: " << row << " " << i << endl;
		renderGrid->ClearCell(row, i);
	}	
}	

