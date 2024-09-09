#include <wx/wx.h>

class myFrame : public wxFrame
{
	public:
		myFrame(const wxString& title);

		void OnQuit(wxCommandEvent& event);		
		void OnStart(wxCommandEvent& event); 

		wxMenuBar *menubar;
		wxMenu *file;
		wxPanel *panel;
		wxStaticText *testtext;
};
