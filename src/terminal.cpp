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
	font_size = 15;
	main_cursor_x = 0;
	main_cursor_y = 0;
	alt_cursor_x = 0;
	alt_cursor_y = 0;

	SetFont(wxFont(
		font_size,
		wxFONTFAMILY_TELETYPE,
		wxFONTSTYLE_NORMAL,
		wxFONTWEIGHT_NORMAL
	));

	wxSize dim = GetFont().GetPixelSize();

	font_height = dim.GetHeight();
	font_width = dim.GetWidth();

	// Spawn Shell
	const char *shell_path = "/bin/bash";
	const char * argv[] = {NULL};
	int fork_status = SpawnShell(&pty_master, &shell_pid, shell_path, const_cast<char **>(argv));

	if (fork_status == -1) {
		Close(true);
	}

	this->Bind(wxEVT_PAINT, &Terminal::Render, this);
	this->Bind(wxEVT_CHAR, &Terminal::OnKeyEvent, this);
	this->Bind(wxEVT_SIZE, &Terminal::ReSize, this);

	renderTimer = new wxTimer(this, RenderTimerId);
	this->Bind(wxEVT_TIMER, &Terminal::Timer, this);
	renderTimer->Start(1); // I don't like this
}

int Terminal::SpawnShell(int *pty_master, int *shell_pid, const char *shell_path, char * argv[]) {
	*shell_pid = forkpty(pty_master, nullptr, nullptr, nullptr);

	switch (*shell_pid) {
		case -1:
			//cout << "Cannot fork" << endl;
			return -1;
			break;
		case 0:
			execvp(shell_path, argv);
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

		if (alt_screen) {
			grid = &alt_grid;
		} else {
			grid = &main_grid;
		}

		/* 
			There is no need to calculate the offset value if I draw from bottom right to top left
		*/
		for (auto row_it = grid->begin() + row_scroll; row_it != grid->end(); ++row_it) {
			for (auto col_it = row_it->begin(); col_it != row_it->end(); ++col_it) {
				int i, j, x, y;

				i = std::distance(grid->begin(), row_it) - row_scroll;
				j = std::distance(row_it->begin(), col_it);
				x = j * font_width;
				y = i * font_height;

				wxUniChar b((int) *col_it);
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

	int to_erase = 0;

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
			strncpy(out, arrow_up, size);
			break;
		case WXK_DOWN:
			size+=3;
			strncpy(out, arrow_down, size);
			break;
		case WXK_RIGHT:
			size+=3;
			strncpy(out, arrow_right, size);
			break;
		case WXK_LEFT:
			size+=3;
			strncpy(out, arrow_left, size);
			break;
		default:
			out[0] = keycode;
			size++;
			break;
	}

	write(pty_master, &out, (size_t) size);
}

void Terminal::Timer(wxTimerEvent& event) {
	int status;
	if (waitpid(shell_pid, &status, WNOHANG) != shell_pid) {
		fd_set reading;
		struct timeval timeout;

		FD_ZERO(&reading);
		FD_SET(pty_master, &reading);
		memset(&timeout, 0, sizeof(timeout));

		int rc = select(pty_master + 1, &reading, nullptr, nullptr, &timeout);

		if (rc > 0) {
			if (FD_ISSET(pty_master, &reading)) {
				ReadFromPty(pty_master, &raw_data);
				// I should iterate through the raw data here
				while (!raw_data.empty()) {
					PtyData cur = raw_data.at(0);
					if (alt_screen) {
						PopulateGrid(&cur, &alt_grid, &alt_cursor_x, &alt_cursor_y);
					} else {
						PopulateGrid(&cur, &main_grid, &main_cursor_x, &main_cursor_y);
					}
					raw_data.pop_front();
				}
				Refresh();
			}
		}
	} else {
		renderTimer->Stop();
	}
}

void Terminal::ReSize(wxSizeEvent& event) {
	GetSize(&window_width, &window_height);

	struct winsize w;

	int new_height = window_height / font_height - 1;
	int new_width = window_width / font_width - 1;

	w.ws_row = new_height;
	w.ws_col = new_width;
	grid_height = new_height;
	grid_width = new_width;

	main_grid.resize(grid_height, vector<char>(grid_width));
	alt_grid.resize(grid_height, vector<char>(grid_width));

	ioctl(pty_master, TIOCSWINSZ, &w);
}

void Terminal::ReadFromPty(int pty_master, deque<PtyData> *raw_data) {
	int bytes_read = read(pty_master, buf, sizeof(buf));

	vector<char> tmp;

	bool ESC = false;
	bool CSI = false;

	for (int i = 0; i < bytes_read; i++) {
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
			(*raw_data).push_back(datum);
		}
	}
}

/*
	Here, I should not iterate through the raw data. I should only take one PtyData and parse it
*/
void Terminal::PopulateGrid(PtyData *current, vector<vector<char>> *grid, int *cursor_x, int *cursor_y) {
	if (*cursor_y >= grid->size()) {
		vector<char> newline;
		newline.resize(grid_width);
		grid->push_back(newline);
		row_scroll++;
	}

	switch (current->type) {
		case PRINTABLE:
			(*grid)[*cursor_y][*cursor_x] = current->keycode; 
			(*cursor_x)++;
			if (*cursor_x == grid->begin()->size()) {
				*cursor_x = 0; 
				(*cursor_y)++;
			}	

			break;
		case BACKSPACE:
			(*cursor_x)--;
			(*grid)[*cursor_y][*cursor_x] = '\0';
			break;
		case TAB:
			break;
		case BELL:
			break;
		case CARRAIGE:
			(*cursor_x) = 0;
			break;
		case NEWLINE:
			(*cursor_y)++;
			break;
		case ESCAPE:
			break;
		case ANSI:
			Parse(*current, grid, cursor_x, cursor_y);
			break;
	}
}

void Terminal::Parse(PtyData ansi, vector<vector<char>>* grid, int *cursor_x, int *cursor_y) {
	string str(ansi.ansicode.begin(), ansi.ansicode.end());

	// very basic
	if (str=="K") {
		grid->at(*cursor_y).resize(grid_width);
	} else if (str == "H") {
		// only temporary to simulate clear behavior...
		*cursor_x = 0;
		*cursor_y = 0;
	} else if (str == "J") {
		// only temporary to simulate clear behavior...
		grid->clear();
		grid->resize(grid_height, vector<char>(grid_width));
	} else if (str == "?1049h") {
		alt_screen = true;
	} else if (str == "?1049l") {
		alt_screen = false;
		*cursor_x = 0;
		*cursor_y = 0;
		grid->clear();
		grid->resize(grid_height, vector<char>(grid_width));
	}
}
