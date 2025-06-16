# Goals
- [x] figure out how to create a gui application with c++; using wxWidgets
- [x] spawn a shell; using forkpty, ~~select, fd_set, and fds~~ ended up using fcntl
- [x] figure out how to send/display keystrokes with wxWidgets
- [x] have a basic framework before I go crazy
- [x] Correctly send typed keys, and signals (right now it crashes when I do Ctrl-C)
- [x] Scrolling 
- [ ] Resizing Window
- [ ] Render Man page correctly
- [ ] handle arrow keys correctly
- [ ] figure out how to parse
- [ ] Set up CMake just for kicks
- [ ] Resizing text size
- [ ] Cursor
- [ ] Handle Unicode Characters
- [ ] Run vim properly

# Useful Readings
https://tldp.org/HOWTO/Text-Terminal-HOWTO-7.html#ss7.2
https://poor.dev/blog/terminal-anatomy/
https://en.wikibooks.org/wiki/Serial_Programming/termios#Introduction

# Prerequisites 
wxWidgets (brew install wxWidgets)
c++

# To compile 
run make in src directory
