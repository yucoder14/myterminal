#include "frame.h"
#include "constants.h"

#include <iostream>

#include <util.h>
#include <unistd.h>
#include <string.h>

#include <wx/dcbuffer.h>

using namespace std;

MyFrame::MyFrame(const wxString& title)
: wxFrame(nullptr, wxID_ANY, title, wxDefaultPosition, wxSize(800,800)) {
	panel = new wxPanel(this, -1);

	this->grid_length = 0;

	// Get window size
	GetSize(&this->window_width, &this->window_height);

	// Set font
	this->font_size = 16;
	this->cursor_x = 0;
	this->cursor_y = 0;

	// Spawn Shell
	const char *shell_path = "/bin/bash";
	char * argv[] = {NULL};
	int fork_status = SpawnShell(&this->pty_master, &this->shell_pid, shell_path, argv);

	if (fork_status == -1) {
		Close(true);
	}	

	this->SetOwnBackgroundColour(wxColour(0,0,0));

	this->Bind(wxEVT_PAINT, &MyFrame::Render, this);
	panel->Bind(wxEVT_CHAR, &MyFrame::OnKeyEvent, this);

	renderTimer = new wxTimer(this, RenderTimerId);
	this->Bind(wxEVT_TIMER, &MyFrame::Timer, this);
	renderTimer->Start(5); // I don't like this
	CreateStatusBar();
}

int MyFrame::SpawnShell(int *pty_master, int *shell_pid, const char *shell_path, char * argv[]) {
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

void MyFrame::Render(wxPaintEvent& WXUNUSED(event)) {
	wxBufferedPaintDC dc(this);

	dc.SetFont(wxFont(
				this->font_size,
				wxFONTFAMILY_TELETYPE,
				wxFONTSTYLE_NORMAL,
				wxFONTWEIGHT_NORMAL
			));

	wxSize dim = dc.GetFont().GetPixelSize();

	if (!this->font_height) {
		this->font_height = dim.GetHeight();
	}

	if (!this->font_width) {
		this->font_width = dim.GetWidth();
	}

	int new_length = this->grid.size();

	if (this->grid_length != new_length) { 
		int j = 1; // counter to skip over previously printed characters
		
		for (auto i = this->grid.begin(); i != grid.end(); ++i ){
			if (j > this->grid_length) {  
				switch ((*i).type) { 
					case GUARD:
						//do not account guard cell as a printable character
						j--;  
						break;
					case CARRAIGE_RETURN: 
						this->cursor_x = 0;
						break;
					case NEWLINE:
						this->cursor_y++;
						break;
					case PRINTABLE:
						// text wrapping 
						if (this->cursor_x * this->font_width > this->window_width - 2 * this->font_width) { 
							this->cursor_x = 0;
							this->cursor_y++;
						}
						int x = this->cursor_x * this->font_width;
						int y = this->cursor_y * this->font_height;
						dc.DrawText((*i).keycode, x, y);
						this->cursor_x++;
						break;
				}
			}
			j++;
		}	
		this->grid_length = new_length; 
	}
}

void MyFrame::OnKeyEvent(wxKeyEvent& event) {
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
			this->place_guard = true;
			out[0] = WXK_RETURN;
			size++;
			break;
		case WXK_UP:
			for (auto i = this->grid.rbegin(); (*i).type != GUARD; ++i) {
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

	write(this->pty_master, &out, (size_t) size);
}

void MyFrame::Timer(wxTimerEvent& event) {
	int status;
	if (waitpid(this->shell_pid, &status, WNOHANG) != this->shell_pid) {
		int flags = fcntl(this->pty_master, F_GETFL, 0);
		fcntl(this->pty_master, F_SETFL, flags | O_NONBLOCK);

		char b;
		while(read(this->pty_master, &b, (size_t) 1) != -1) {
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
			this->grid.push_back(cell);
		}

		// should i remove the previous guard to save space?
		if (this->place_guard) {
			cout << "Guard Placed" << endl;
			Cell guard; 
			guard.type = GUARD;
			this->grid.push_back(guard);
			this->place_guard = false;
		}	

		if (this->grid_length != this->grid.size())
			Refresh();
	} else {
		renderTimer->Stop();
		vector<Cell>().swap(this->grid);
	}
}	
