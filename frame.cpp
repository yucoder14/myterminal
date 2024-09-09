#include "frame.h"

myFrame::myFrame(const wxString& title)
	: wxFrame(NULL, wxID_ANY, title, wxDefaultPosition, wxSize(600,350))
{
	panel = new wxPanel(this, -1);

	menubar = new wxMenuBar;
	file = new wxMenu;
	file->Append(wxID_EXIT, wxT("&Quit"));
	SetMenuBar(menubar);

	testtext = new wxStaticText(panel, -1, wxT("hello world"));

	Connect(wxID_EXIT, wxEVT_COMMAND_MENU_SELECTED,
		wxCommandEventHandler(myFrame::OnQuit));

	Centre();

}

void myFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
	Close(true);
}	

