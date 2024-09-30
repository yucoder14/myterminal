#include "frame.h"

#include <iostream>

#include <util.h>
#include <unistd.h>
#include <string.h>

#include <wx/dcbuffer.h>

using namespace std;

MyFrame::MyFrame(const wxString& title)
: wxFrame(nullptr, wxID_ANY, title, wxDefaultPosition, wxSize(800,800)) {
	panel = new wxPanel(this, -1);

	this->char_x = 0;
	this->char_y = 0;

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
	this->Bind(wxEVT_TIMER, [this](wxTimerEvent&) {
		int status;
		if (waitpid(this->shell_pid, &status, WNOHANG) != this->shell_pid) {
			// I should recieve text here
			// then render the text using Refresh
			Refresh();
		} else {
			cout << "blocking here?" << endl;
		}

	});
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
	// I don't think the drawing needs to be buffered as I'll draw all the text every function call. not ideal but (for now) feels inevitable considering that I need to somehow redraw all the text during window resize event
	wxBufferedPaintDC dc(this);

	dc.SetFont(wxFont(
				16,
				wxFONTFAMILY_TELETYPE,
				wxFONTSTYLE_NORMAL,
				wxFONTWEIGHT_NORMAL
			));

	wxSize dim = dc.GetFont().GetPixelSize();

	if (!this->char_height) {
		this->char_height = dim.GetHeight();
	}

	if (!this->char_width) {
		this->char_width = dim.GetWidth();
	}


	// probably this is causing the blocking...
	// the render function should only render text, NOT fetch then render text
	// hopefully this problem will be taken care of if I implement the new data structure
	int flags = fcntl(this->pty_master, F_GETFL, 0);
	fcntl(this->pty_master, F_SETFL, flags | O_NONBLOCK);

	int i = 0;
	int line_count = 0;
	while(read(this->pty_master, &this->output_buf[i], (size_t) 1) != -1) {
		if (this->output_buf[i] == '\n') {
			line_count++;
		}
		i++;
	}
	this->output_buf[i] = '\0';

	if (line_count > 0) {
		i = 0;
	}
	dc.DrawText(this->output_buf, this->char_x, this->char_y);
	this->char_x += this->char_width * i;
	this->char_y += this->char_height * line_count;
}

void MyFrame::OnKeyEvent(wxKeyEvent& event) {
	int keycode = event.GetKeyCode();
	wxLogStatus("Key Event %d", event.GetKeyCode());

	int size = 0;
	char out[10];
	// move this out of this function and make them a constant
	char arrow_up[] = "\033[A";
	char arrow_down[] = "\033[B";
	char arrow_right[] = "\033[C";
	char arrow_left[] = "\033[D";

	switch (keycode) {
		case WXK_CONTROL_C:
			cout << "Ctrl-C pressed" << endl;
			out[0] = WXK_CONTROL_C;
			size++;
			break;
		case WXK_RETURN:
			this->char_y+=this->char_height;
			this->char_x=0;
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

	write(this->pty_master, &out, (size_t) size);
}

