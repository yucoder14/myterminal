# for now just compile them all at the same time --> should things a object files and link them...
myterminal: frame.h main.h frame.cpp main.cpp terminal.cpp terminal.h 
	c++ main.cpp frame.cpp terminal.cpp ptyData.cpp ansi.cpp ansiRenderGrid.cpp grid.cpp renderGrid.cpp  `wx-config --cxxflags --libs` -std=c++17

