#include "frame.h"
#include "constants.h"

MyFrame::MyFrame(const wxString& title)
: wxFrame(nullptr, wxID_ANY, title, wxDefaultPosition, wxSize(800,800)) {
	term = new Terminal(this, wxID_ANY, wxDefaultPosition, wxSize(800,800));
	CreateStatusBar();
}

