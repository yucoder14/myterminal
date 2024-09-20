#include "main.h"
#include "frame.h"

IMPLEMENT_APP(MyApp)

bool MyApp::OnInit() {
	MyFrame *frame = new MyFrame(wxT("Test"));
	frame->Show(true);

	return true;
}	
