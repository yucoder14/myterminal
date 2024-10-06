#include <vector>
#include <string>

#include <wx/wx.h>
#include <wx/timer.h>


enum CellType {
	PRINTABLE = 0,
	CARRAIGE_RETURN,
	NEWLINE
};

class Cell {
public:
	enum CellType type; 
	char keycode;
};

class MyFrame : public wxFrame {
public:
	MyFrame(const wxString& title);

	wxPanel *panel;
	wxTimer *renderTimer;
	static constexpr int RenderTimerId = 1114;

	int SpawnShell(int *pty_master, int *shell_pid, const char * shell_path, char * argv[]);
	void Render(wxPaintEvent& event);
	void OnKeyEvent(wxKeyEvent& event);
	void Timer(wxTimerEvent& event);

private:
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

	std::vector<Cell> grid;
	int grid_length;
	char output_buf[65536]; 
};


