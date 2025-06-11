#pragma once 

#include <vector>
#include <deque> 
#include <string>
#include <iostream>
#include <string>
#include <cctype>
#include <algorithm>

#include <util.h>
#include <unistd.h>
#include <string.h>

#include <wx/dcbuffer.h>
#include <wx/unichar.h>
#include <wx/wx.h>
#include <wx/timer.h>

#include <sys/select.h>
#include <sys/ioctl.h>

using namespace std;

#include "constants.h"
#include "ptyData.h"
using namespace PTY;
#include "cell.h"

class Terminal : public wxWindow {
public:
	Terminal(wxWindow *parent, wxWindowID id, const wxPoint &pos, const wxSize &size);
	wxTimer *renderTimer;
	static constexpr int RenderTimerId = 1114;

	int SpawnShell(int *ptyMaster, int *shellPid, const char * shellPath, char * argv[]);
	void AddNewLine(vector<vector<Cell>> *grid);
	void SetGrid(vector<vector<Cell>> *grid, int *cursorX, int *cursorY);

	void ReadFromPty(int ptyMaster, deque<PtyData> *rawData);
	void PopulateGrid(PtyData *rawData, vector<vector<Cell>> *grid, int *cursorX, int *cursorY);
	void Parse(PtyData ansi, vector<vector<Cell>>* grid, int *cursorX, int *cursorY);

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
	vector<vector<Cell>> mainGrid;
	vector<vector<Cell>> altGrid;

	char buf[65536];
	bool space = true;
};


