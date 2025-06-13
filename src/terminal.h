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
#include "backend.h"
using namespace PTY;
using namespace GRID;

/*
	In general, the Terminal class's main job is connect the pty with 
	the screen. 	

	There are two main components to the terminal: the shell and the grid.
	Terminal's main job is to just relay information between the shell and 
	the grid. Any movement of cursors or scrolling is only a consequences of 
	the information relay. In other words, it is grid's job to decide what
	to do with the data upon receiving them from the Terminal. --> not finalized,
	but it sounds like a reasonable design choice.
*/
class Terminal : public wxWindow {
public:
	Terminal(wxWindow *parent, wxWindowID id, const wxPoint &pos, const wxSize &size);
	wxTimer *renderTimer;
	static constexpr int RenderTimerId = 1114;

	// The Terminal has three jobs: spawning shells, reading from the pty and writing to the screen. 
	void ReadFromPty(int ptyMaster, deque<PtyData> *rawData);
	int SpawnShell(int *ptyMaster, int *shellPid, const char * shellPath, char * argv[]);

	// this grid related stuff should not be here
	void SetGrid(vector<vector<Cell>> *grid, int *cursorX, int *cursorY);
//	void SetGrid(Grid *grid); 
	void AddNewLine(vector<vector<Cell>> *grid);
//	void AddNewLine(Grid *grid); --> this should be a grid method, not a terminal method!
	void PopulateGrid(PtyData *rawData, vector<vector<Cell>> *grid, int *cursorX, int *cursorY);
//	void PopulateGrid(PtyData *rawData, Grid *grid);
	void Parse(PtyData ansi, vector<vector<Cell>>* grid, int *cursorX, int *cursorY); 
//	void Parse(PtyData ansi, Grid *grid); --> hopefully, this gets abstracted away 
//	into its own class (?), knowing how many ansi code there are

	void Render(wxPaintEvent& event);
	void OnKeyEvent(wxKeyEvent& event);
	void Timer(wxTimerEvent& event);
	void ReSize(wxSizeEvent& event);

private:
	// widow information 
	int windowHeight, windowWidth;
	
	// shell related stuff
	int ptyMaster;
	int shellPid;
	int shellStatus;

//	RenderGrid grid;
//	Grid mainGrid;
	vector<vector<Cell>> mainGrid;
	int mainCursorX, mainCursorY;
	int gridHeight, gridWidth;
	int rowScroll = 0;

	// this is wrong! alt grid should not share the 
	// height, width and scroll value with the mainGrid!
//	Grid altGrid;
	vector<vector<Cell>> altGrid;
	int altCursorX, altCursorY;

	// font related stuff
	int fontSize;
	int fontX, fontY;
	int fontHeight, fontWidth;

	bool altScreen = false;

	deque<PtyData> rawData;

	char buf[65536];
};


