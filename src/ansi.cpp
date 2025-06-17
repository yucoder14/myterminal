#include "ansi.h"

PANSI ParseAnsiString(string str) { 
	PANSI res;
	regex setRegex("^(=).+");
	regex privateRegex("^\\?.+");
	regex modeRegex("[a-zA-Z]$");
	regex argRegex("([0-9]+)"); 

	if (regex_match(str, setRegex)) {
		res.setMode = true;
	}	
	if (regex_match(str, privateRegex)) {
		res.privateMode = true;
	}	

	smatch mode; 
	regex_search(str, mode, modeRegex);
	res.mode = string(mode[0])[0];

	smatch args;
	string::const_iterator searchStart (str.cbegin());
	while (regex_search(searchStart, str.cend(), args, argRegex)) {
		res.args.push_back(stoi(string(args[0])));
		searchStart = args.suffix().first;
	}	

	return res;
}	
