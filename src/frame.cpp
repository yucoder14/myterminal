#include "frame.h"
#include "constants.h"

MyFrame::MyFrame(const wxString& title)
: wxFrame(nullptr, wxID_ANY, title, wxDefaultPosition, wxSize(800,800)) {
	wxBoxSizer *sizer = new wxBoxSizer(wxVERTICAL);

	term = new Terminal(this, wxID_ANY, wxDefaultPosition, wxSize(500,300));

	sizer->Add(term, 1, wxEXPAND | wxALL);
	SetSizerAndFit(sizer);

	CreateStatusBar();
}

