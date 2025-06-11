#include "terminal.h"

/*** Constructor ******************************************************/

Terminal::Terminal(
		wxWindow *parent, wxWindowID id, const wxPoint &pos, const wxSize &size)
: wxWindow(parent, id, pos, size) {
	this->SetBackgroundStyle(wxBG_STYLE_PAINT);

	// Set font
	fontSize = 15;
	mainCursorX = 0;
	mainCursorY = 0;
	altCursorX = 0;
	altCursorY = 0;

	SetFont(wxFont(
		fontSize,
		wxFONTFAMILY_TELETYPE,
		wxFONTSTYLE_NORMAL,
		wxFONTWEIGHT_NORMAL
	));

	wxSize dim = GetFont().GetPixelSize();

	fontHeight = dim.GetHeight();
	fontWidth = dim.GetWidth();

	// Spawn Shell
	const char *shellPath = "/bin/bash";
	const char * argv[] = {NULL};
	int forkStatus = SpawnShell(
			&ptyMaster, &shellPid, shellPath, const_cast<char **>(argv));

	if (forkStatus == -1) {
		Close(true);
	}

	// Set even handlers
	this->Bind(wxEVT_PAINT, &Terminal::Render, this);
	this->Bind(wxEVT_CHAR, &Terminal::OnKeyEvent, this);
	this->Bind(wxEVT_SIZE, &Terminal::ReSize, this);

	// Start renderTimer 
	renderTimer = new wxTimer(this, RenderTimerId);
	this->Bind(wxEVT_TIMER, &Terminal::Timer, this);
	renderTimer->Start(1); // I don't like this
}

/*** Terminal Related Stuff *******************************************/

int Terminal::SpawnShell(
		int *ptyMaster, int *shellPid, const char *shellPath, char * argv[]) {
	*shellPid = forkpty(ptyMaster, nullptr, nullptr, nullptr);

	switch (*shellPid) {
		case -1:
			//cout << "Cannot fork" << endl;
			return -1;
			break;
		case 0:
			execvp(shellPath, argv);
			return 1;
	}

	return 1;
}

void Terminal::SetGrid(
		vector<vector<Cell>> *grid, int *cursorX, int *cursorY) {
	if (altScreen) {		
		grid = &altGrid;	
	} else {	
		grid = &mainGrid;	
	}	
}	

void Terminal::ReadFromPty(int ptyMaster, deque<PtyData> *rawData) {
	int bytesRead = read(ptyMaster, buf, sizeof(buf));

	vector<char> tmp;

	bool ESC = false;
	bool CSI = false;

	for (int i = 0; i < bytesRead; i++) {
		PtyData data;
		char b = buf[i];
		PTY::GetPtyData(&data, b, &tmp, &ESC, &CSI);
		if (!ESC && !CSI) {
			(*rawData).push_back(data);
		}
	}
}

void Terminal::AddNewLine(vector<vector<Cell>> *grid) {
	vector<Cell> newline;
	newline.resize(gridWidth);
	grid->push_back(newline);
	rowScroll++;
}	

void Terminal::PopulateGrid(
		PtyData *current, vector<vector<Cell>> *grid, int *cursorX, int *cursorY) {
	switch (current->type) {
		case PRINTABLE:
			(*grid)[*cursorY][*cursorX].keycode = current->keycode; 
			(*cursorX)++;
			if (*cursorX == grid->begin()->size()) {
				(*cursorY)++;
				*cursorX = 0; 
				AddNewLine(grid);
				(*grid)[*cursorY][*cursorX].lineBreak = true;
			}	
			break;
		case BACKSPACE:
			(*cursorX)--;
			break;
		case TAB:
			break;
		case BELL:
			break;
		case CARRAIGE:
			(*cursorX) = 0;
			break;
		case NEWLINE:
			{ 
				(*cursorY)++; 
				if (*cursorY == grid->size()) {
					AddNewLine(grid);
				}	
			}
			break;
		case ESCAPE:
			break;
		case ANSI:
			Parse(*current, grid, cursorX, cursorY);
			break;
	}
}

void Terminal::Parse(
		PtyData ansi, vector<vector<Cell>>* grid, int *cursorX, int *cursorY) {
	string str(ansi.ansicode.begin(), ansi.ansicode.end());

	// very basic
	if (str=="K") {
		for (auto colIt = grid->at(*cursorY).begin() + *cursorX; 
				colIt != grid->at(*cursorY).end(); ++colIt) { 
			(*grid)[*cursorY][*cursorX].keycode = '\0';
		}	
	} else if (str == "H") {
		// only temporary to simulate clear behavior... 
		*cursorX = 0;
		*cursorY = 0;
	} else if (str == "J") {
		// only temporary to simulate clear behavior... --> this is fucked
		cout << grid->size() << endl;
//		for (int i = 0; i < gridHeight; i++) {
//			vector<Cell> newline;
//			newline.resize(gridWidth);
//			grid->push_back(newline);
//			rowScroll += 1;
//			*cursorY += 1;
//		}	
	} else if (str == "?1049h") {
		altScreen = true;
	} else if (str == "?1049l") {
		altScreen = false;
		*cursorX = 0;
		*cursorY = 0;
		grid->clear();
		grid->resize(gridHeight, vector<Cell>(gridWidth));
	}
}

/*** Rendering and Event Handling ************************************/

void Terminal::Render(wxPaintEvent& WXUNUSED(event)) {
	wxPaintDC dc(this);

	dc.Clear();
	wxGraphicsContext *gc = wxGraphicsContext::Create(dc);

	if (gc) {
		vector<vector<Cell>>* grid;

		if (altScreen) {
			grid = &altGrid;
		} else {
			grid = &mainGrid;
		}

		for (auto rowIt = grid->begin() + rowScroll; rowIt != grid->end(); ++rowIt) {
			for (auto colIt = rowIt->begin(); colIt != rowIt->end(); ++colIt) {
				int i, j, x, y;

				i = std::distance(grid->begin(), rowIt) - rowScroll;
				j = std::distance(rowIt->begin(), colIt);
				x = j * fontWidth;
				y = i * fontHeight;

				wxUniChar b((int) colIt->keycode);
				dc.DrawText(b, x, y);
			}	
		}	

		delete gc;
	}
}

void Terminal::OnKeyEvent(wxKeyEvent& event) {
	int keycode = event.GetKeyCode();
	wxLogStatus("Key Event %d", event.GetKeyCode());

	int size = 0;
	char out[10]; // this is prone to buffer overflow --> change to vector

	int toErase = 0;

	switch (keycode) {
		case WXK_CONTROL_C:
			out[0] = WXK_CONTROL_C;
			size++;
			break;
		case WXK_RETURN:
			out[0] = WXK_RETURN;
			size++;
			break;
		case WXK_UP:
			size+=3;
			strncpy(out, arrowUp, size);
			break;
		case WXK_DOWN:
			size+=3;
			strncpy(out, arrowDown, size);
			break;
		case WXK_RIGHT:
			size+=3;
			strncpy(out, arrowRight, size);
			break;
		case WXK_LEFT:
			size+=3;
			strncpy(out, arrowLeft, size);
			break;
		default:
			out[0] = keycode;
			size++;
			break;
	}

	write(ptyMaster, &out, (size_t) size);
}

void Terminal::Timer(wxTimerEvent& event) {
	int status;
	if (waitpid(shellPid, &status, WNOHANG) != shellPid) {
		fd_set reading;
		struct timeval timeout;

		FD_ZERO(&reading);
		FD_SET(ptyMaster, &reading);
		memset(&timeout, 0, sizeof(timeout));

		int rc = select(ptyMaster + 1, &reading, nullptr, nullptr, &timeout);
		if (rc > 0) {
			vector<vector<Cell>> *grid;
			int *cursorX, *cursorY;

			if (altScreen) {
				grid = &altGrid;
				cursorX = &altCursorX;
				cursorY = &altCursorY;
			} else {	
				grid = &mainGrid;
				cursorX = &mainCursorX;
				cursorY = &mainCursorY;
			}	

			if (FD_ISSET(ptyMaster, &reading)) {
				ReadFromPty(ptyMaster, &rawData);
				while (!rawData.empty()) {
					PtyData cur = rawData.at(0);
					PopulateGrid(&cur, grid, cursorX, cursorY);
					rawData.pop_front();
				}
				Refresh();
			}
		}
	} else {
		renderTimer->Stop();
	}
}

void Terminal::ReSize(wxSizeEvent& event) {
	GetSize(&windowWidth, &windowHeight);

	struct winsize w;

	int newHeight = windowHeight / fontHeight;
	int newWidth = windowWidth / fontWidth;

	w.ws_row = newHeight;
	w.ws_col = newWidth;
	gridHeight = newHeight;
	gridWidth = newWidth;

	vector<vector<Cell>> *grid;

	if (altScreen) {
		grid = &altGrid;
	} else {	
		grid = &mainGrid;
	}	
	
	// TODO: correctly resize the screen, it is not enough to just 
	// resize the grid; i must temporarily save the previous grid; 
	// resize the grid and correctly handle line wrapping...; as of 
	// now there is no way for me to distinguish between line breaks
	// and new lines 
	int numRows = grid->size();
//	grid->clear();
	grid->resize(gridHeight, vector<Cell>(gridWidth));

	ioctl(ptyMaster, TIOCSWINSZ, &w);
}
