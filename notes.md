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

# Create a Custom Canvas with a Caret
wxPaintDC - used to draw shapes and text 
- [x] How do I draw letters on the canvas?
- [x] How do I set the font?  
- [ ] How do I create a canvas
- [ ] Rich font? 
- [ ] How can I render a cursor? (look at the caret sample)

# Handle keyboard inputs 
wxKeyEvent - for keyboard inputs 
- [x] How do I get pressed keys? 
- [x] How do I draw them on the canvas? 

# Multi-threading GUI application
- [ ] Do I need this?
- [ ] How do I send/receive characters/signals between the TE and the shell? 
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

Maybe look more into Canonical vs. Non-Canonical mode.
https://stackoverflow.com/questions/358342/canonical-vs-non-canonical-terminal-input

In short, what I need to figure out is how to send and display characters to the shell properly. 
Considering that shell will be in canonical mode, I should first send keyboard inputs to the shell, which will then take care of line discipline stuff for me and then display what is returned from the shell. (I can be totally wrong about this) - SUS

CMake
=====
A Modern way to build projects

How to Render Text
==================
As of now, it seems like I need to keep a buffer to keep track of all the characters that were entered. 

I first have to know how the events are programmed to be handled within the wxWidgets framework.
Then, I will have a better idea of what the fuck I'm suppose to do. 

Not completely sure yet, but the events seems to be handled in an infinite loop, meaning one paint event will take care of any changes that are made to the buffer(?) - NOTSUS but not fine either


Newest Updates
==============
- I got to a point where I can read and write stuff to the shell, but it's printing extra junk (probably some ANSI-code escape sequence that's related to "bracketed paste mode"? 
- also I have to spam the return key multiple times in order for me to get the result of a command; just sending a newline character does not work as intended
    - Is it something related to the way I'm handling I/O? is fcntl not enough?
    - can "read" read newline/EOF
- The cursor movement is also very unreliable. 

Terminal window should be a grid of character cells (custom class)
To correctly parse ANSI-codes, there needs to be an output buffer
output buffer will be used for the prompt and the output 

What I do not understand
========================
TE prints weird characters [?2004l and [?2004h when reading from the pty 
I have to invoke multiple key events in order for the renderer to render everything
- this is related to how wxWidgets handles events; 
- from the way I currently have it, the window is only refreshed if keys are pressed or when the window is refocused. 
- SOLUTION: find a way to generate a custom event regarding output
    - will be ideal if I can refresh only one keys and where there are outputs to read from pty
    - a separate thread that focuses solely on reading from pty (might be redundant)    
    - ~~Look into how to properly handle idle event (will this going to work? idk)~~ 
    - look into how to constantly check the pty for available bytes to read 

