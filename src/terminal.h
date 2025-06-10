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
	enum PtyDataType type; 
	char keycode;
	vector<char> ansicode;
};

struct Cell {
	char keycode; 
	bool lineBreak;
};	

class Terminal : public wxWindow {
public:
	Terminal(wxWindow *parent, wxWindowID id, const wxPoint &pos, const wxSize &size);
	wxTimer *renderTimer;
	static constexpr int RenderTimerId = 1114;

	int SpawnShell(int *ptyMaster, int *shellPid, const char * shellPath, char * argv[]);
	void ReadFromPty(int ptyMaster, deque<PtyData> *rawData);
	void PopulateGrid(PtyData *rawData, vector<vector<char>> *grid, int *cursorX, int *cursorY);
//	void PopulateGrid(deque<PtyData> *rawData, vector<vector<char>> *grid, int *cursorX, int *cursorY);
	void Parse(PtyData ansi, vector<vector<char>>* grid, int *cursorX, int *cursorY);

	void Render(wxPaintEvent& event);
	void OnKeyEvent(wxKeyEvent& event);
	void Timer(wxTimerEvent& event);
	void ReSize(wxSizeEvent& event);

private:
	// widow information 
	int windowHeight, windowWidth;
	int gridHeight, gridWidth;
	
	// shell related stuff
	int ptyMaster;
	int shellPid;
	int shellStatus;

	// Used for indexing through the vectos to add/delete characters
	int mainCursorX, mainCursorY;
	int altCursorX, altCursorY;
	int rowScroll = 0;

	// font related stuff
	int fontSize;
	int fontX, fontY;
	int fontHeight, fontWidth;

	bool altScreen = false;

	deque<PtyData> rawData;
	vector<vector<char>> mainGrid;
	vector<vector<char>> altGrid;

	char buf[65536];
	bool space = true;
};


