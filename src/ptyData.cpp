#include "ptyData.h"

void GetPtyData(PtyData *data, char b, vector<char> *tmp, bool *ESC, bool *CSI) {
	switch (b) {
		case 7:	// bell
			data->type = BELL;
			break;
		case 8:  // backspace
			data->type = BACKSPACE;
			break;
		case 9: // tabspace
			data->type = TAB;
			break;
		case 10: // newline
			data->type = NEWLINE;
			break;
		case 13: // carriage return
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
					data->type = ESCAPE;
					data->ansicode.push_back(b);
				}
			} else if (*CSI) {
				tmp->push_back(b);
				// ansi code terminators
				if (isalpha(b) || b == '@') {
					*CSI = false;
					data->type = ANSI;
					data->ansicode.swap(*tmp);
					string str(data->ansicode.begin(), data->ansicode.end());
				}
			} else {
				data->type = PRINTABLE;
				data->keycode = b;
			}
			break;
	}
}	
