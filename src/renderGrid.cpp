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

void GRID::RenderGrid::GetRenderGridCaret(int *row, int *col) {
	*row = renderCaretY; 
	*col = renderCaretX;
}	

/*** Setters **********************************************************/

void GRID::RenderGrid::MoveRenderCaret(int row, int col) {
	renderCaretX = col; 
	renderCaretY = row;

	renderGrid->MoveCaret(row, col);	
}	

void GRID::RenderGrid::SetRenderGridElement(PtyData *data) {
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
	renderGrid->IncCaretX(1);
	renderCaretX = (renderCaretX + 1) % renderGridWidth;

	if (renderGrid->GetCaretX() == renderGrid->GetNumCols()) {
		renderGrid->ZeroCaretX();
		SetNewline();
		renderGrid->SetLineBreak();
	}	
}	

void GRID::RenderGrid::SetBackspace() {
	renderGrid->DecCaretX(1);
	renderCaretX--;
}	

void GRID::RenderGrid::SetTab() {
	// tab space is 4 for now
	renderGrid->IncCaretX(4);
}	

void GRID::RenderGrid::SetCarraige() {
	renderGrid->ZeroCaretX();
	renderCaretX = 0;
}	

void GRID::RenderGrid::SetNewline() {
	renderGrid->IncCaretY(1); 
	renderCaretY = min(renderGridHeight - 1, ++renderCaretY);
	if (renderGrid->GetCaretY() == 
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
