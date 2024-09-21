#include <wx/wx.h>
#include <wx/timer.h>

class MyFrame : public wxFrame {
public:
	MyFrame(const wxString& title);

	wxPanel *panel;
	wxTimer *renderTimer;
	static constexpr int RenderTimerId = 1114;

	int pty_master;
	int shell_pid;

	int char_x, char_y;
	int char_height, char_width;

	char output_buf[65536]; 
	int output_line_count;

	void Render(wxPaintEvent& event);
	void OnKeyEvent(wxKeyEvent& event);
};

