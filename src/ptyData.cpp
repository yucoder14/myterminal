#include "ptyData.h"

void PTY::GetPtyData(PtyData *data, char b, vector<char> *tmp, bool *ESC, bool *CSI) {
	switch (b) {
		case 7:	// bell
			data->type = BELL;
			break;
		case 8:  // backspace
			//cout << "BACKSPACE" << endl;
			data->type = BACKSPACE;
			break;
		case 9: // tabspace
			//cout << "TAB" << endl;
			data->type = TAB;
			break;
		case 10: // newline
			//cout << "NL" << endl;
			data->type = NEWLINE;
			break;
		case 13: // carriage return
			//cout << "CR" << endl;
			data->type = CARRAIGE;
			break;
		case 27: // escape
			*ESC = true;
			break;
		default:
			if (*ESC) {
				if (b == '[') {
					*CSI = true;;
					*ESC = false;
				} else {
					*ESC = false;
					//cout << "ESCAPE: " <<  b << endl;
					data->type = ESCAPE;
					data->ansicode.push_back(b);
				}
			} else if (*CSI) {
				tmp->push_back(b);
				if (isalpha(b)) {
					*CSI = false;
					data->type = ANSI;
					data->ansicode.swap(*tmp);
					string str(data->ansicode.begin(), data->ansicode.end());
					//cout << "ANSI CODE: " << str << endl;
				}
			} else {
				//cout << b << ", " << (unsigned int)int(b) << endl;
				data->type = PRINTABLE;
				data->keycode = b;
			}
			break;
	}
}	
