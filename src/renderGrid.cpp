#include "backend.h" 

using namespace GRID;

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

void GRID::RenderGrid::MoveRenderCursor(int row, int col) {
	renderCursorX = col; 
	renderCursorY = row;

	renderGrid->MoveCursor(row, col);	
}	

void GRID::RenderGrid::SetRenderGridElement(PtyData *data) {
//	cout << "\x1b[H\x1b[J" << "cursor: " << renderCursorX << " " << renderCursorY << endl;
	switch (data->type) {
		case PRINTABLE:
			renderGrid->SetGridElement(data->keycode);
			renderGrid->IncCursorX(1);
			renderCursorX = (renderCursorX + 1) % renderGridWidth;

			if (renderGrid->GetCursorX() == renderGrid->GetNumCols()) {
				renderGrid->ZeroCursorX();
				renderGrid->IncCursorY(1); 
				renderCursorY = min(renderGridHeight - 1, ++renderCursorY);
				if (renderGrid->GetCursorY() == 
						renderGrid->GetNumRows()) {
					renderGrid->AddNewLine(true);
				}
				renderGrid->SetLineBreak();
			}	
			break;
		case BACKSPACE:
			renderGrid->DecCursorX(1);
			renderCursorX--;
			break;
		case TAB:
			break;
		case BELL:
			break;
		case CARRAIGE:
			renderGrid->ZeroCursorX();
			renderCursorX = 0;
			break;
		case NEWLINE:
			renderGrid->IncCursorY(1); 
			renderCursorY = min(renderGridHeight - 1, ++renderCursorY);
			if (renderGrid->GetCursorY() == 
					renderGrid->GetNumRows()) {
				renderGrid->AddNewLine(true);
			}
			break;
		case ESCAPE:
//			ParseEscapeCode(data);
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
