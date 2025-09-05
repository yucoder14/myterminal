#include <vector>
#include <string>
#include <iostream>

using namespace std;

constexpr char BELL_CHAR = 7;
constexpr char BACKSPACE_CHAR = 8;
constexpr char TAB_CHAR = 9;
constexpr char NEWLINE_CHAR = 10;
constexpr char CARRAIGE_CHAR = 13;
constexpr char ESCAPE_CHAR = 27;

/*
	All the PTY (psuedo terminal) related things reside here. 
	When terminal reads data from the shell, the data is "tokenized" 
	and stored in the struct PtyData. Then, the Terminal will relay 
	this information to the grid to figure out how it's suppose to 
	format the data, such that it is screen ready.
*/
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
