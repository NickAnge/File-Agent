# File-Agent

File-Agent is a event-driven Windows service written in C++. This service checks "movement" of files in directories like (C:/,D:/ or other mounted directories).It is at an early stage as it sends a simple message to the event viewer application of windows, when a simple creation, deletion , Update or read of a file is happening(CRUD).


## Installation

1)Install Visual Studio\
2)Create a new Project->Windows Desktop Wizard->Next\
3)Name:CppWindowsService\
4)Clone source code for this repo\
5)Build project\

### Commands

-Install
CppWindowsService.exe -install

-Remove
CppWindowsService.exe -remove

-Start and Stop for SCM
