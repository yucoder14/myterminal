#include "backend.h" 

using namespace GRID;

/*** Getters **********************************************************/
int GRID::RenderGrid::GetRenderGridHeight() {
	return renderGridHeight; 
}	

int GRID::RenderGrid::GetRenderGridiWidth() {
	return renderGridWidth;
}	

Cell *GRID::RenderGrid::GetRenderGridElement(int row, int col) {
	int newRow = row + renderGrid->GetRowTop();

	return renderGrid->GetGridElement(newRow, col);	
}	

void GRID::RenderGrid::GetRenderGridDimensions(int *height, int *width) {
	*height = renderGrid->GetNumRows();	
	*width = renderGrid->GetNumCols();
}	

/*** Setters **********************************************************/

void GRID::RenderGrid::MoveRenderCursor(int row, int col) {
	renderCursorX = col; 
	renderCursorY = row;

	renderGrid->MoveCursor(row, col);	
}	

void GRID::RenderGrid::SetRenderGridElement(PtyData *data) {
//	cout << "\x1b[H\x1b[J" << "cursor: " << renderCursorX << " " << renderCursorY << endl;
	switch (data->type) {
		case PRINTABLE:
			SetPrintable(data);
			break;
		case BACKSPACE:
			SetBackspace();
			break;
		case TAB:
			SetTab();
			break;
		case BELL:
			break;
		case CARRAIGE:
			SetCarraige();
			break;
		case NEWLINE:
			SetNewline();
			break;
		case ESCAPE:
//			ParseEscapeCode(data);
			break;
		case ANSI:
			SetAnsi(data);
			break;
	}
}	

void GRID::RenderGrid::SetPrintable(PtyData *data) {
	renderGrid->SetGridElement(data->keycode);
	renderGrid->IncCursorX(1);
	renderCursorX = (renderCursorX + 1) % renderGridWidth;

	if (renderGrid->GetCursorX() == renderGrid->GetNumCols()) {
		renderGrid->ZeroCursorX();
		SetNewline();
		renderGrid->SetLineBreak();
	}	
}	

void GRID::RenderGrid::SetBackspace() {
	renderGrid->DecCursorX(1);
	renderCursorX--;
}	

void GRID::RenderGrid::SetTab() {
	// tab space is 4 for now
	renderGrid->IncCursorX(4);
}	

void GRID::RenderGrid::SetCarraige() {
	renderGrid->ZeroCursorX();
	renderCursorX = 0;
}	

void GRID::RenderGrid::SetNewline() {
	renderGrid->IncCursorY(1); 
	renderCursorY = min(renderGridHeight - 1, ++renderCursorY);
	if (renderGrid->GetCursorY() == 
			renderGrid->GetNumRows()) {
		renderGrid->AddNewLine(true);
	}
}	

/*** Miscellaneous ****************************************************/

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
