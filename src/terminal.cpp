#include "terminal.h"
#include "constants.h"

#include <iostream>
#include <deque>

#include <util.h>
#include <unistd.h>
#include <string.h>

#include <wx/dcbuffer.h>

using namespace std;

Terminal::Terminal(wxWindow *parent, wxWindowID id, const wxPoint &pos, const wxSize &size)
: wxWindow(parent, id, pos, size) {
	this->SetBackgroundStyle(wxBG_STYLE_PAINT);
	grid_length = 0;

	// Get window size
	GetSize(&window_width, &window_height);

	// Set font
	font_size = 16;
	cursor_x = 0;
	cursor_y = 0;

	// Spawn Shell
	const char *shell_path = "/bin/bash";
	char * argv[] = {NULL};
	int fork_status = SpawnShell(&pty_master, &shell_pid, shell_path, argv);

	if (fork_status == -1) {
		Close(true);
	}	

	SetOwnBackgroundColour(wxColour(0,0,0));

	this->Bind(wxEVT_PAINT, &Terminal::Render, this);
	this->Bind(wxEVT_CHAR, &Terminal::OnKeyEvent, this);

	renderTimer = new wxTimer(this, RenderTimerId);
	this->Bind(wxEVT_TIMER, &Terminal::Timer, this);
	renderTimer->Start(5); // I don't like this
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
	wxBufferedPaintDC dc(this);

	dc.SetFont(wxFont(
				font_size,
				wxFONTFAMILY_TELETYPE,
				wxFONTSTYLE_NORMAL,
				wxFONTWEIGHT_NORMAL
			));

	wxSize dim = dc.GetFont().GetPixelSize();

	if (!font_height) {
		font_height = dim.GetHeight();
	}

	if (!font_width) {
		font_width = dim.GetWidth();
	}

	int new_length = grid.size();

	int new_cells = new_length - grid_length;
		
	deque<Cell> tmp;

	for (auto i = grid.rbegin(); new_cells > 0; ++i) {
		tmp.push_front(*i); 
		new_cells--;
	}	

	for (auto i = tmp.begin(); i != tmp.end(); ++i ){
		switch ((*i).type) { 
			case GUARD:
				//do not account guard cell as a printable character
				break;
			case CARRAIGE_RETURN: 
				cursor_x = 0;
				break;
			case NEWLINE:
				cursor_y++;
				break;
			case PRINTABLE:
				// text wrapping 
				if (cursor_x * font_width > window_width - 2 * font_width) { 
					cursor_x = 0;
					cursor_y++;
				}
				int x = cursor_x * font_width;
				int y = cursor_y * font_height;
				dc.DrawText((*i).keycode, x, y);
				cursor_x++;
		}
		grid_length = new_length; 
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
			place_guard = true;
			out[0] = WXK_RETURN;
			size++;
			break;
		case WXK_UP:
			for (auto i = grid.rbegin(); (*i).type != GUARD; ++i) {
				to_erase++;
			}	
			cout << to_erase << endl;
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
		int flags = fcntl(pty_master, F_GETFL, 0);
		fcntl(pty_master, F_SETFL, flags | O_NONBLOCK);

		char b;
		while(read(pty_master, &b, (size_t) 1) != -1) {
			Cell cell; 
			switch (b) { 
				case 13: 
					cell.type = CARRAIGE_RETURN;
					break;
				case 10: 
					cell.type = NEWLINE;
					break;
				default:
					cell.type = PRINTABLE;
					cell.keycode = b;
					break;	
			}	
			grid.push_back(cell);
		}

		// should i remove the previous guard to save space?
		if (place_guard) {
			Cell guard; 
			guard.type = GUARD;
			grid.push_back(guard);
			place_guard = false;
		}	

		if (grid_length != grid.size())
			Refresh();
	} else {
		renderTimer->Stop();
		vector<Cell>().swap(grid);
	}
}	
