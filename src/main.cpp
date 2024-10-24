#include "main.h"
#include "frame.h"

IMPLEMENT_APP(MyApp)

bool MyApp::OnInit() {
	MyApp &main_app = wxGetApp();

	MyFrame *frame = new MyFrame(wxT("Test"));

	frame->Show(true);

	return true;
}	
