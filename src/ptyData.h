#include <string>
#include <vector>
#include <iostream>

using namespace std;

namespace PTY {
	typedef struct PtyData PtyData;

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

	void GetPtyData(PtyData *data, char b, vector<char> *tmp, bool *ESC, bool *CSI);
}	
