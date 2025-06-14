#pragma once 

#include <vector>
#include <deque> 
#include <string>

#include <wx/wx.h>
#include <wx/timer.h>

using namespace std;

enum PtyDataType {
	PRINTABLE,
	BACKSPACE,
	TAB,
	BELL,
	CARRAIGE,
	NEWLINE,
	ESCAPE,
	ANSI
};

struct PtyData {
public:
	enum PtyDataType type; 
	char keycode;
	vector<char> ansicode;
};

class Terminal : public wxWindow {
public:
	Terminal(wxWindow *parent, wxWindowID id, const wxPoint &pos, const wxSize &size);
	wxTimer *renderTimer;
	static constexpr int RenderTimerId = 1114;

	int SpawnShell(int *pty_master, int *shell_pid, const char * shell_path, char * argv[]);
	void ReadFromPty(int pty_master, deque<PtyData> *raw_data);
	void PopulateGrid(PtyData *raw_data, vector<vector<char>> *grid, int *cursor_x, int *cursor_y);
//	void PopulateGrid(deque<PtyData> *raw_data, vector<vector<char>> *grid, int *cursor_x, int *cursor_y);
	void Parse(PtyData ansi, vector<vector<char>>* grid, int *cursor_x, int *cursor_y);

	void Render(wxPaintEvent& event);
	void OnKeyEvent(wxKeyEvent& event);
	void Timer(wxTimerEvent& event);
	void ReSize(wxSizeEvent& event);

private:
	// widow information 
	int window_height, window_width;
	int grid_height, grid_width;
	
	// shell related stuff
	int pty_master;
	int shell_pid;
	int shell_status;

	// Used for indexing through the vectos to add/delete characters
	int main_cursor_x, main_cursor_y;
	int alt_cursor_x, alt_cursor_y;
	int row_scroll = 0;

	// font related stuff
	int font_size;
	int font_x, font_y;
	int font_height, font_width;

	bool alt_screen = false;

	deque<PtyData> raw_data;
	vector<vector<char>> main_grid;
	vector<vector<char>> alt_grid;

	char buf[65536];
	bool space = true;
};


