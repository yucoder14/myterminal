# for now just compile them all at the same time --> should things a object files and link them...
myterminal: frame.h main.h frame.cpp main.cpp terminal.cpp terminal.h 
	c++ main.cpp frame.cpp terminal.cpp ptyData.cpp `wx-config --cxxflags --libs` -std=c++17

renderGrid: backend.h renderGrid.cpp
	c++ renderGrid.cpp grid.cpp ptyData.cpp -std=c++17 
