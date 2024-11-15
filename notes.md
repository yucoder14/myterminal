GUI Toolkits to use for c++
===========================
wxWidgets because its free and cross-platform, and, hopefully, robust and powerful enough to make a terminal emulator. From what I have seen, it is more than capable. 
Example command for compiling: c++ -o hello hello.cpp `wx-config --cxxflags --libs`

Learning plan (sorta)
=====================
# Terminal Emulator Basics
- [x] what are terminal emulators? 
- [x] how do terminal emulators work? 

# Learn CMake / wxWidgets Basics
https://www.youtube.com/watch?v=MfuBS9n5_aY&list=PL0qQTroQZs5vVmTuBew-nTx9DIu6rRl2c&index=1&ab_channel=Luke%27sDevTutorials
- [ ] why do I need it? To make the building process easier 
- [ ] How do I write a Cmake file and execute it?
- [ ] How do I set up a project in order for CMake to work?

# Scrolling 
- [x] How can I do dynamic scrolling with growing number of things to draw? → I handled this by using two dimensional vector 

# Create a Custom Canvas with a Caret
wxPaintDC - used to draw shapes and text 
- [x] How do I draw letters on the canvas?
- [x] wxUniChar was an important element of the puzzle. 
- [x] How do I set the font?  
- [x] How do I create a canvas
- [ ] Rich font? 
- [ ] How can I render a cursor? (look at the caret sample)

# Handle keyboard inputs 
wxKeyEvent - for keyboard inputs 
- [x] How do I get pressed keys? 
- [x] How do I draw them on the canvas? 
- [x] How do I un-draw things that have been drawn by previous keystrokes? → as of now, the whole window gets redrawn periodically. So I just pop things off the vector to "undraw"j

# Unicode stuff
- [ ] How would I draw a unicode character? 

# ANSI stuff
- [ ] What is sent back from the shell when I send arrow keys? 
- [ ] What should 'clear' do?
- [ ] How do I color the foreground & background
- [ ] How do I do different text styles?

# Multi-threading GUI application
- [x] Do I need this?
- [x] How do I send/receive characters/signals between the TE and the shell? 
wxThread


Terminal Emulator Basics
========================
Terminal emulators are software emulations of teletypes.
Teletypes were used in the old days as a method to send keystrokes to the computer, which the computer interpreted and responded accordingly.
TTY's were essentially just a way of displaying what was being typed and what the computer produced. 
There are mainly two processes involved when emulating a terminal: terminal emulator and a shell to talk to the OS. 
Terminal emulators sole job is to interpret data from the shell and display it on screen. 
The shell provides interface to the OS. 
The two processes communicate through something called a pseudo-terminal, i.e., pty. 
PTY is a data structure similar to a bidirectional pipe, where data can be sent/received from both ends.

The two sides are master and slave. (Asynchronous Serial communication)
Master side is where the terminal emulator renders text. 
Master file descriptor will be used as the main communication channel(?) - SUS
- Can I consider the Master file descriptor as the pty?
Slave side is where the shell and open other terminal based apps operate. 
When users input text, text is rendered on TE. When the user presses Enter, than series of texts are sent through the pty to the shell. 
The shell will execute the command (forking if necessary). Then, it will get some output, which will be sent through the pty to the TE to be displayed (ANSI-escape codes)

CMake
=====
A Modern way to build projects

How to Render Text
==================
~~As of now, it seems like I need to keep a buffer to keep track of all the characters that were entered.~~ ~~I do not have to keep track of input characters, for I just need to send them straight to the pty~~ I need to keep track of output characters
~~Not completely sure yet, but the events seems to be handled in an infinite loop, meaning one paint event will take care of any changes that are made to the buffer(?)~~ Events do get handled in an infinite loop, and paint event can be invoked by Refresh. By having a timer that calls Refresh() periodically, I was able to render new texts if available. However, it is not ideal(?) for it's calling the refresh method more than needed. 


Newest Updates
==============
- I can now constantly send/receive incoming/outgoing characters and draw them on the terminal
- I can now detect escaped and basic ansi escape codes. I have yet to parse all the sequences mentioned in the following github repo:
    - https://gist.github.com/fnky/458719343aabd01cfb17a3a4f7296797
- I can now parse ^[K somewhat correctly
- I can now handle non_ascii characters, but I cannot print them just yet...
- On drawing characters...
    - it appears that vim, top are not printed correctly when printing from bottom right to top left. Current solution is to just print all alternate screens from top left to bottom right 
    - main screen is drawn from bottom right to top left

Data Structure Ideas
====================
- Terminal window should be a grid of character cells
    - [x] should contain the size of the window 
    - [x] a long vector to store all the characters; this way I can redraw all the letters when there is a window resize event or text resize event
    - [x] will be vector of 'Cell' class which contains various information about the what the cell contains 
    - [x] to conserve space, it's probably best to have a Cell type where it instructs to print multiple spaces/lines, instead of having blank cells that take up meaningless space
- To correctly parse ANSI-codes, there needs to be an temporary buffer to capture the incoming ansi-codes
    - data structure to contain different types of ansi-code instruction; will be just a collection of flags that will dictate oncoming non-ansi code characters  
- output buffer will be used for the prompt and the output 

Notes
=====
when I send left/right arrow keys and type, it will print the typed key and the preceding characters; It also appears that some ANSI codes are sent back...
when I send up/down arrow keys, it will print the previous/next 'command' in the history 
I have no idea how I'll render uni-code, but that's a problem for later
^[?1049h / ^[1049l are ansi codes to let the terminal know when to enable/disable alternative buffer
^[?1034h toggle interpretation of "meta" keys. This sets the eighth bit of keyboard input
^[?1h / ^[?1h - toggles the application cursor keys mode, which changes what control sequences are sent by the server

Problems
========
CR in middle of input (when the input length is bigger than the allotted terminal window, shell prints a CR) causes bugs 
Cannot correctly handle ← and → keys
