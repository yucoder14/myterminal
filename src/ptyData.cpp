#include "ptyData.h"

void GetPtyData(PtyData *data, char b, vector<char> *tmp, bool *ESC, bool *CSI) {
//	cout << "RAW: " << int(b) << "->" <<  char(b) << endl;
	switch (b) {
		case BELL_CHAR:	
			data->type = BELL;
			break;
		case BACKSPACE_CHAR:  
			data->type = BACKSPACE;
			//cout << "BACKSPACE" << endl;
			break;
		case TAB_CHAR: 
			data->type = TAB;
			//cout << "TAB" << endl;
			break;
		case NEWLINE_CHAR: 
			data->type = NEWLINE;
			//cout << "NEWLINE" << endl;
			break;
		case CARRAIGE_CHAR:
			data->type = CARRAIGE;
			//cout << "CARRAIGE" << endl;
			break;
		case ESCAPE_CHAR: 
			*ESC = true;
			break;
		default:
			if (*ESC) {
				if (b == '[') {
					*CSI = true;
					*ESC = false;
				} else {
					*ESC = false;
					data->type = ESCAPE;
					data->ansicode.push_back(b);
					//cout << "ESCAPE: " << b << endl;
				}
			} else if (*CSI) {
				tmp->push_back(b);
				// ansi code terminators
				if (isalpha(static_cast<unsigned char>(b)) || b == '@') {
					*CSI = false;
					data->type = ANSI;
					data->ansicode.swap(*tmp);
					tmp->clear();
//					string str(data->ansicode.begin(), data->ansicode.end());
//					cout << "ANSI: " << str << endl;
				}
			} else {
				data->type = PRINTABLE;
				data->keycode = b;
//					cout << "PRINTABLE: " << char(b) << endl;
			}
			break;
	}
}	
