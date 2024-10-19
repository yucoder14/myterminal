#include "terminal.h"

#include <vector>
#include <string>

#include <wx/wx.h>
#include <wx/timer.h>

class MyFrame : public wxFrame {
public:
	MyFrame(const wxString& title);


private:
	Terminal *term;
};


