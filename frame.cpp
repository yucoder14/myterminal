#include "frame.h"
#include <util.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <string>
#include <wx/dcbuffer.h>
#include <wx/wfstream.h>
using namespace std;
MyFrame::MyFrame(const wxString& title) 
: wxFrame(nullptr, wxID_ANY, title, wxDefaultPosition, wxSize(800,800)) {
	panel = new wxPanel(this, -1);

	char_x = 0;
	char_y = 0;

	this->SetOwnBackgroundColour(wxColour(0,0,0));	

	this->Bind(wxEVT_PAINT, &MyFrame::Render, this);
	panel->Bind(wxEVT_CHAR, &MyFrame::OnKeyEvent, this);

	
	char name[40];
	shell_pid = forkpty(&pty_master, name, nullptr, nullptr);

	switch (shell_pid) {
		case -1: 
			std::cout << "Cannot fork" << std::endl;
			Close(true);
			break;	
		case 0: 
			char * argv[] = {NULL};
			execvp("/opt/homebrew/bin/bash", argv);
	}
	std::cout << name << std::endl;

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

	if (!char_height) {
		char_height = dim.GetHeight();
	}	

	if (!char_width) {
		char_width = dim.GetWidth();
	}	

	// find a way to read all the available characters without indefinitely blocking into a temporary buffer
	int flags = fcntl(pty_master, F_GETFL, 0);
	fcntl(pty_master, F_SETFL, flags | O_NONBLOCK);

	int i = 0; 
	int line_count = 0;
	while(read(pty_master, &output_buf[i], (size_t) 1) != -1) {
		//dc.DrawText(d, char_x, char_y);
		//char_x+=char_width;
		//cout << output_buf[i] << ", " << int(output_buf[i]) << endl;
		if (output_buf[i] == '\n') {
			line_count++;
		} 
		i++;
	}
	output_buf[i] = '\0';
	if (line_count > 0) {
		i = 0;
	}	
	dc.DrawText(output_buf, char_x, char_y);
	char_x += char_width * i;
	char_y += char_height * line_count;
}	

void MyFrame::OnKeyEvent(wxKeyEvent& event) {
	int keycode = event.GetKeyCode(); 
	wxLogStatus("Key Event %d", event.GetKeyCode());

	write(pty_master, &keycode, (size_t) 1);	
	if (keycode == 13) {
		char_y+=char_height;
		char_x=0;
	}	

	Refresh();
}

