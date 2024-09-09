# Goals
- [x] figure out how to create a gui application with c++; using wxWidgets
- [x] spawn a shell; using forkpty, select, fd_set, and fds. 
- [ ] figure out how to send/display keystrokes with wxWidgets
- [ ] figure out how to parse
- [ ] have a basic framework before I go crazy

# Readings...
https://tldp.org/HOWTO/Text-Terminal-HOWTO-7.html#ss7.2
https://poor.dev/blog/terminal-anatomy/
https://en.wikibooks.org/wiki/Serial_Programming/termios#Introduction


My current understanding of this project (Can be wrong)
=======================================================
## pty
- some data structure that acts like a bidirectional pipe that can receive and send data
- has two ends: master and slave. Master end is where user sends keystrokes to. Slave end is where the shell does things. 
- it is pty's job to send data from master end to the slave end where the keys will be interepreted and executed by shell.
- it is also pty's job to receive data from the slave end and send them to master end, where characters will be displayed (ANSI-escape codes)

