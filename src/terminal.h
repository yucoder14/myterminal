#pragma once 

#include <vector>
#include <string>

#include <wx/wx.h>
#include <wx/timer.h>

using namespace std;

enum PtyDataType {
	PRINTABLE,
	BACKSPACE,
	BELL,
	CARRAIGE,
	NEWLINE,
	ESCAPE,
	ANSI
};

class PtyData {
public:
	enum PtyDataType type; 
	char keycode;
	vector<char> ansicode;
};

class Terminal : public wxScrolled<wxWindow> {
public:
	Terminal(wxWindow *parent, wxWindowID id, const wxPoint &pos, const wxSize &size);
	wxTimer *renderTimer;
	static constexpr int RenderTimerId = 1114;

	int SpawnShell(int *pty_master, int *shell_pid, const char * shell_path, char * argv[]);
	void ReadFromPty();

	void Render(wxPaintEvent& event);
	void OnKeyEvent(wxKeyEvent& event);
	void Timer(wxTimerEvent& event);
	void ReSize(wxSizeEvent& event);

private:
	// for placing deletion protection 
	bool place_guard = true;     // for checking if guard needs to be placed

	// widow information 
	int window_height, window_width;
	
	// shell related stuff
	int pty_master;
	int shell_pid;
	int shell_status;

	// cursor position; used to draw text in correct position
	int cursor_x, cursor_y;

	// font related stuff
	int font_size;
	int font_x, font_y;
	int font_height, font_width;

	vector<PtyData> raw_data;
	vector<vector<char>> archive;

};


