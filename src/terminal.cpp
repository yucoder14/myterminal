#include "terminal.h"

/*** Constructor ******************************************************/

Terminal::Terminal(
		wxWindow *parent, wxWindowID id, const wxPoint &pos, const wxSize &size)
: wxWindow(parent, id, pos, size) {
	this->SetBackgroundStyle(wxBG_STYLE_PAINT);
	GetSize(&windowWidth, &windowHeight);
	// Set font
	fontSize = 15;
	SetFont(wxFont(
		fontSize,
		wxFONTFAMILY_TELETYPE,
		wxFONTSTYLE_NORMAL,
		wxFONTWEIGHT_NORMAL
	));
	wxSize dim = GetFont().GetPixelSize();
	fontHeight = dim.GetHeight();
	cout << fontHeight << endl;
	fontWidth = dim.GetWidth();

	// initialize grids and cursors
	gridHeight = windowHeight / fontHeight;
	gridWidth = windowWidth / fontWidth;
	grid.ResizeRenderGrid(gridHeight, gridWidth);	

	// Spawn Shell
	const char *shellPath = "/bin/bash";
	const char * argv[] = {NULL};
	int forkStatus = SpawnShell(
			&ptyMaster, &shellPid, shellPath, const_cast<char **>(argv));

	if (forkStatus == -1) {
		Close(true);
	}

	// Set even handlers
	this->Bind(wxEVT_PAINT, &Terminal::Render, this);
	this->Bind(wxEVT_CHAR, &Terminal::OnKeyEvent, this);
	this->Bind(wxEVT_SIZE, &Terminal::ReSize, this);

	// Start renderTimer 
	renderTimer = new wxTimer(this, RenderTimerId);
	this->Bind(wxEVT_TIMER, &Terminal::Timer, this);
	renderTimer->Start(10); // I don't like this
}

/*** Terminal Related Stuff *******************************************/

int Terminal::SpawnShell(
	int *ptyMaster, int *shellPid, const char *shellPath, char * argv[]) {
	*shellPid = forkpty(ptyMaster, nullptr, nullptr, nullptr);

	switch (*shellPid) {
		case -1:
			//cout << "Cannot fork" << endl;
			return -1;
			break;
		case 0:
			execvp(shellPath, argv);
			return 1;
	}

	return 1;
}

void Terminal::ReadFromPty(int ptyMaster, deque<PtyData> *rawData) {
	int bytesRead = read(ptyMaster, buf, sizeof(buf));

	vector<char> tmp;

	bool ESC = false;
	bool CSI = false;

	for (int i = 0; i < bytesRead; i++) {
		PtyData data;
		char b = buf[i];
		GetPtyData(&data, b, &tmp, &ESC, &CSI);
		if (!ESC && !CSI) {
			(*rawData).push_back(data);
		}
	}
}


/*** Rendering *******************************************************/

void Terminal::Render(wxPaintEvent& WXUNUSED(event)) {
	wxPaintDC dc(this);

	dc.Clear();
	wxGraphicsContext *gc = wxGraphicsContext::Create(dc);

	if (gc) {
		for (int i = 0; i < grid.GetRenderGridHeight(); i++) { 
			for (auto j = 0; j < grid.GetRenderGridiWidth(); j++) {
//				grid.MoveRenderCursor(i, j);
				int x, y;
				
				x = j * fontWidth;
				y = i * fontHeight;
				Cell *cell = grid.GetRenderGridElement(i, j);

				// only draw if cell exists
				if (cell != nullptr) {
					int keycode = cell->keycode;
					wxUniChar b((int) keycode);
					dc.DrawText(b, x, y);
				}	
			}
		}

		delete gc;
	}
}

/*** Event Handling **************************************************/

void Terminal::OnKeyEvent(wxKeyEvent& event) {
	int keycode = event.GetKeyCode();
	wxLogStatus("Key Event %d", event.GetKeyCode());

	int size = 0;
	char out[10]; // this is prone to buffer overflow --> change to vector

	int toErase = 0;

	switch (keycode) {
		case WXK_CONTROL_C:
			out[0] = WXK_CONTROL_C;
			size++;
			break;
		case WXK_RETURN:
			out[0] = WXK_RETURN;
			size++;
			break;
		case WXK_UP:
			size+=3;
			strncpy(out, arrowUp, size);
			break;
		case WXK_DOWN:
			size+=3;
			strncpy(out, arrowDown, size);
			break;
		case WXK_RIGHT:
			size+=3;
			strncpy(out, arrowRight, size);
			break;
		case WXK_LEFT:
			size+=3;
			strncpy(out, arrowLeft, size);
			break;
		default:
			out[0] = keycode;
			size++;
			break;
	}

	write(ptyMaster, &out, (size_t) size);
}

void Terminal::Timer(wxTimerEvent& event) {
	int status;
	if (waitpid(shellPid, &status, WNOHANG) != shellPid) {
		fd_set reading;
		struct timeval timeout;

		FD_ZERO(&reading);
		FD_SET(ptyMaster, &reading);
		memset(&timeout, 0, sizeof(timeout));

		int rc = select(ptyMaster + 1, &reading, nullptr, nullptr, &timeout);
		if (rc > 0) {
			if (FD_ISSET(ptyMaster, &reading)) {
				ReadFromPty(ptyMaster, &rawData);
				grid.FormatRawData(&rawData);
				Refresh();
			}
		}
	} else {
		renderTimer->Stop();
	}
}

void Terminal::ReSize(wxSizeEvent& event) {
	GetSize(&windowWidth, &windowHeight);

	struct winsize w;

	int newHeight = windowHeight / fontHeight;
	int newWidth = windowWidth / fontWidth;

	w.ws_row = newHeight;
	w.ws_col = newWidth;

	grid.ResizeRenderGrid(newHeight, newWidth);

	ioctl(ptyMaster, TIOCSWINSZ, &w);
}
