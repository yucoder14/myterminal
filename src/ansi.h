#include <regex> 
#include <string>
#include <vector>

using namespace std;

typedef struct PANSI PANSI;
struct PANSI { 
	bool setMode = false;  
	bool privateMode = false;
	char mode;
	vector<int> args;
};

PANSI ParseAnsiString(string str);
