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
