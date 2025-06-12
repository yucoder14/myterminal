#include <string>
#include <vector>
#include <iostream>

using namespace std;

/*
	All the PTY (psuedo terminal) related things reside here. 
	When terminal reads data from the shell, the data is "tokenized" 
	and stored in the struct PtyData. Then, the Terminal will relay 
	this information to the grid to figure out how it's suppose to 
	format the data, such that it is screen ready.
*/
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
