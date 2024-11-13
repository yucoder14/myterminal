#include "terminal.h"
#include "constants.h"

#include <iostream>
#include <string>
#include <cctype>

#include <util.h>
#include <unistd.h>
#include <string.h>

#include <wx/dcbuffer.h>

#include <sys/select.h>
#include <sys/ioctl.h>


Terminal::Terminal(wxWindow *parent, wxWindowID id, const wxPoint &pos, const wxSize &size)
: wxScrolled<wxWindow>(parent, id, pos, size) {
	this->SetBackgroundStyle(wxBG_STYLE_PAINT);

	// Get window size
	GetSize(&window_width, &window_height);

	// Set font
	font_size = 16;
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

	for (int i = 0; i < window_height / font_height; i++) {
		vector<char> line1;
		vector<char> line;
		alt_grid.push_back(line1);
		main_grid.push_back(line);
	}

	// Spawn Shell
	const char *shell_path = "/bin/bash";
	char * argv[] = {NULL};
	int fork_status = SpawnShell(&pty_master, &shell_pid, shell_path, argv);

	if (fork_status == -1) {
		Close(true);
	}

	this->Bind(wxEVT_PAINT, &Terminal::Render, this);
	this->Bind(wxEVT_CHAR, &Terminal::OnKeyEvent, this);
	this->Bind(wxEVT_SIZE, &Terminal::ReSize, this);

	renderTimer = new wxTimer(this, RenderTimerId);
	this->Bind(wxEVT_TIMER, &Terminal::Timer, this);
	renderTimer->Start(10); // I don't like this
}

int Terminal::SpawnShell(int *pty_master, int *shell_pid, const char *shell_path, char * argv[]) {
	*shell_pid = forkpty(pty_master, nullptr, nullptr, nullptr);

	switch (*shell_pid) {
		case -1:
			cout << "Cannot fork" << endl;
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
			cout << "**************** alt ****************" << endl;
		} else {
			grid = &main_grid;
			cout << "**************** main ****************" << endl;
		}

		// calculate offset value to print to the last available 
		int offset = grid->size() - int(window_height / font_height) + 2;
		if (offset < 0) {
			offset = 0;
		}	
		int win_x = 0, win_y = 0;
		for (auto i = grid->begin() + offset; i != grid->end(); i++) {
			for (auto j = i->begin(); j != i->end(); j++) {
				if (win_x * font_width > window_width - font_width) {
					win_x = 0;
					win_y++;
				}
				int x = win_x * font_width;
				int y = win_y * font_height;
				dc.DrawText(*j, x, y);
				win_x++;
			}
			win_x=0;
			win_y++;
		}
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
			cout << "Ctrl-C pressed" << endl;
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
				if (alt_screen) {
					PopulateGrid(&raw_data, &alt_grid, &alt_cursor_x, &alt_cursor_y);
				} else {
					PopulateGrid(&raw_data, &main_grid, &main_cursor_x, &main_cursor_y);
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

	int new_height = window_height / font_height;
	int new_width = window_width / font_width;

	w.ws_row = new_height;
	w.ws_col = new_width;

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
				cout << "BACKSPACE" << endl;
				datum.type = BACKSPACE;
				break;
			case 9: // tabspace
				cout << "TAB" << endl;
				datum.type = TAB;
				break;
			case 10: // newline
				cout << "NL" << endl;
				datum.type = NEWLINE;
				break;
			case 13: // carriage return
				cout << "CR" << endl;
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
						cout << "ESCAPE: " <<  b << endl;
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
						cout << "ANSI CODE: " << str << endl;

					}
				} else {
					cout << b << ", " << int(b) << endl;
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

void Terminal::PopulateGrid(deque<PtyData> *raw_data, vector<vector<char>> *grid, int *cursor_x, int *cursor_y ) {
	while (!raw_data->empty()) {
		if (*cursor_y > grid->size() - 1) {
			vector<char> newline;
			grid->push_back(newline);
		}

		PtyData current = raw_data->at(0);

		switch (current.type) {
			case PRINTABLE:
				grid->at(*cursor_y).insert(grid->at(*cursor_y).begin() + *cursor_x, current.keycode);
				(*cursor_x)++;
				break;
			case BACKSPACE:
				(*cursor_x)--;
				break;
			case TAB:
				for (int i = 0; i < 4; i++) {
					grid->at(*cursor_y).insert(grid->at(*cursor_y).begin() + *cursor_x, ' ');
					(*cursor_x)++;
				}	
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
				Parse(current, grid, cursor_x, cursor_y);
				break;
		}

		raw_data->pop_front();
	}
}

void Terminal::Parse(PtyData ansi, vector<vector<char>>* grid, int *cursor_x, int *cursor_y) {
	string str(ansi.ansicode.begin(), ansi.ansicode.end());

	// very basic
	if (str=="K") {
		int size = grid->at(*cursor_y).size();
		while (*cursor_x < size) {
			grid->at(*cursor_y).pop_back();
			size--;
		}
	} else if (str == "?1049h") {
		alt_screen = true;
	} else if (str == "?1049l") {
		alt_screen = false;
		grid->clear();
	}
}
