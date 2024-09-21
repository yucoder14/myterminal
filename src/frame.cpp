#include "frame.h"

#include <util.h>
#include <unistd.h>
#include <wx/dcbuffer.h>

using namespace std;

MyFrame::MyFrame(const wxString& title) 
: wxFrame(nullptr, wxID_ANY, title, wxDefaultPosition, wxSize(800,800)) {
	panel = new wxPanel(this, -1);

	this->char_x = 0;
	this->char_y = 0;

	this->shell_pid = forkpty(&this->pty_master, nullptr, nullptr, nullptr);
	switch (this->shell_pid) {
		case -1: 
			cout << "Cannot fork" << endl;
			Close(true);
			break;	
		case 0: 
			char * argv[] = {NULL};
			execvp("/bin/bash", argv);
	}

	this->SetOwnBackgroundColour(wxColour(0,0,0));	

	this->Bind(wxEVT_PAINT, &MyFrame::Render, this);
	panel->Bind(wxEVT_CHAR, &MyFrame::OnKeyEvent, this);

	renderTimer = new wxTimer(this, RenderTimerId);
	this->Bind(wxEVT_TIMER, [this](wxTimerEvent&) {
		int status;
		if (waitpid(this->shell_pid, &status, WNOHANG) == this->shell_pid) {
			this->Destroy();
		}	
			
		Refresh();	
	});	
	renderTimer->Start(5); // I don't like this 
	CreateStatusBar();
}	

void MyFrame::Render(wxPaintEvent& WXUNUSED(event)) {
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

	write(this->pty_master, &keycode, (size_t) 1);	
	if (keycode == 13) {
		this->char_y+=this->char_height;
		this->char_x=0;
	}	
}

