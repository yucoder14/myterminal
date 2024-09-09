#include "main.h"
#include "frame.h"

IMPLEMENT_APP(MyApp)

bool MyApp::OnInit()
{
	myFrame *myframe = new myFrame(wxT("Impossible"));
	myframe->Show(true);

	return true;
}	
