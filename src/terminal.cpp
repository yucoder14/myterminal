#include "terminal.h"
#include "constants.h"

#include <iostream>
#include <string>
#include <cctype>
#include <algorithm>

#include <util.h>
#include <unistd.h>
#include <string.h>

#include <wx/dcbuffer.h>
#include <wx/unichar.h>

#include <sys/select.h>
#include <sys/ioctl.h>


Terminal::Terminal(wxWindow *parent, wxWindowID id, const wxPoint &pos, const wxSize &size)
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
	int forkStatus = SpawnShell(&ptyMaster, &shellPid, shellPath, const_cast<char **>(argv));

	if (forkStatus == -1) {
		Close(true);
	}

	this->Bind(wxEVT_PAINT, &Terminal::Render, this);
	this->Bind(wxEVT_CHAR, &Terminal::OnKeyEvent, this);
	this->Bind(wxEVT_SIZE, &Terminal::ReSize, this);

	renderTimer = new wxTimer(this, RenderTimerId);
	this->Bind(wxEVT_TIMER, &Terminal::Timer, this);
	renderTimer->Start(1); // I don't like this
}

int Terminal::SpawnShell(int *ptyMaster, int *shellPid, const char *shellPath, char * argv[]) {
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

void Terminal::Render(wxPaintEvent& WXUNUSED(event)) {
	wxPaintDC dc(this);

	dc.Clear();
	wxGraphicsContext *gc = wxGraphicsContext::Create(dc);

	if (gc) {
		vector<vector<char>>* grid;

		if (altScreen) {
			grid = &altGrid;
		} else {
			grid = &mainGrid;
		}

		/* 
			There is no need to calculate the offset value if I draw from bottom right to top left
		*/
		cout << gridHeight << " " << gridWidth << endl;
		for (auto rowIt = grid->begin() + rowScroll; rowIt != grid->end(); ++rowIt) {
			for (auto colIt = rowIt->begin(); colIt != rowIt->end(); ++colIt) {
				int i, j, x, y;

				i = std::distance(grid->begin(), rowIt) - rowScroll;
				j = std::distance(rowIt->begin(), colIt);
				x = j * fontWidth;
				y = i * fontHeight;

				wxUniChar b((int) *colIt);
				dc.DrawText(b, x, y);
			}	
		}	

		// calculate offset value to print to the last available
		delete gc;
	}
}

void Terminal::OnKeyEvent(wxKeyEvent& event) {
	int keycode = event.GetKeyCode();
	wxLogStatus("Key Event %d", event.GetKeyCode());

	int size = 0;
	char out[10];

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
			if (FD_ISSET(ptyMaster, &reading)) {
				ReadFromPty(ptyMaster, &rawData);
				// I should iterate through the raw data here
				while (!rawData.empty()) {
					PtyData cur = rawData.at(0);
					if (altScreen) {
						PopulateGrid(&cur, &altGrid, &altCursorX, &altCursorY);
					} else {
						PopulateGrid(&cur, &mainGrid, &mainCursorX, &mainCursorY);
					}
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

	vector<vector<char>> *grid;

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
//	grid->clear();
	grid->resize(gridHeight, vector<char>(gridWidth));

	ioctl(ptyMaster, TIOCSWINSZ, &w);
}

void Terminal::ReadFromPty(int ptyMaster, deque<PtyData> *rawData) {
	int bytesRead = read(ptyMaster, buf, sizeof(buf));

	vector<char> tmp;

	bool ESC = false;
	bool CSI = false;

	for (int i = 0; i < bytesRead; i++) {
		PtyData datum;
		char b = buf[i];
		switch (b) {
			case 7:	// bell
				datum.type = BELL;
				break;
			case 8:  // backspace
				//cout << "BACKSPACE" << endl;
				datum.type = BACKSPACE;
				break;
			case 9: // tabspace
				//cout << "TAB" << endl;
				datum.type = TAB;
				break;
			case 10: // newline
				//cout << "NL" << endl;
				datum.type = NEWLINE;
				break;
			case 13: // carriage return
				//cout << "CR" << endl;
				datum.type = CARRAIGE;
				break;
			case 27: // escape
				ESC = true;
				break;
			default:
				if (ESC) {
					if (b == '[') {
						CSI = true;;
						ESC = false;
					} else {
						ESC = false;
						//cout << "ESCAPE: " <<  b << endl;
						datum.type = ESCAPE;
						datum.ansicode.push_back(b);
					}
				} else if (CSI) {
					tmp.push_back(b);
					if (isalpha(b)) {
						CSI = false;
						datum.type = ANSI;
						datum.ansicode.swap(tmp);
						string str(datum.ansicode.begin(), datum.ansicode.end());
						//cout << "ANSI CODE: " << str << endl;
					}
				} else {
					//cout << b << ", " << (unsigned int)int(b) << endl;
					datum.type = PRINTABLE;
					datum.keycode = b;
				}
				break;
		}

		if (!ESC && !CSI) {
			(*rawData).push_back(datum);
		}
	}
}

/*
	Here, I should not iterate through the raw data. I should only take one PtyData and parse it
*/
void Terminal::PopulateGrid(PtyData *current, vector<vector<char>> *grid, int *cursorX, int *cursorY) {
	if (*cursorY >= grid->size()) {
		vector<char> newline;
		newline.resize(gridWidth);
		grid->push_back(newline);
		rowScroll++;
	}

	switch (current->type) {
		case PRINTABLE:
			(*grid)[*cursorY][*cursorX] = current->keycode; 
			(*cursorX)++;
			if (*cursorX == grid->begin()->size()) {
				*cursorX = 0; 
				(*cursorY)++;
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
			(*cursorY)++;
			break;
		case ESCAPE:
			break;
		case ANSI:
			Parse(*current, grid, cursorX, cursorY);
			break;
	}
}

void Terminal::Parse(PtyData ansi, vector<vector<char>>* grid, int *cursorX, int *cursorY) {
	string str(ansi.ansicode.begin(), ansi.ansicode.end());

	// very basic
	if (str=="K") {
		for (auto colIt = grid->at(*cursorY).begin() + *cursorX; 
				colIt != grid->at(*cursorY).end(); ++colIt) { 
			(*grid)[*cursorY][*cursorX] = '\0';
		}	
	} else if (str == "H") {
		// only temporary to simulate clear behavior...
		*cursorX = 0;
		*cursorY = 0;
	} else if (str == "J") {
		// only temporary to simulate clear behavior...
		grid->clear();
		grid->resize(gridHeight, vector<char>(gridWidth));
	} else if (str == "?1049h") {
		altScreen = true;
	} else if (str == "?1049l") {
		altScreen = false;
		*cursorX = 0;
		*cursorY = 0;
		grid->clear();
		grid->resize(gridHeight, vector<char>(gridWidth));
	}
}
