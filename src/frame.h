#include <vector>
#include <string>

#include <wx/wx.h>
#include <wx/timer.h>


enum CellType {
	PRINTABLE = 0,
	WHITESPACE
};

class Cell {
	enum CellType type; 

	int vertical_shift;
	int horizontal_shift;
	
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

private:
	int pty_master;
	int shell_pid;
	int shell_status;

	int char_x, char_y;
	int char_height, char_width;

	std::vector<Cell> grid;
	char output_buf[65536]; 
	int output_line_count;
};


