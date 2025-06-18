#include "frame.h"

MyFrame::MyFrame(const wxString& title)
: wxFrame(nullptr, wxID_ANY, title, wxDefaultPosition, wxSize(600,400)) {
	wxBoxSizer *sizer = new wxBoxSizer(wxVERTICAL);

	term = new Terminal(this, wxID_ANY, wxDefaultPosition, wxSize(600,408));

	sizer->Add(term, 1, wxEXPAND | wxALL);
	SetSizerAndFit(sizer);

//	CreateStatusBar();
}

