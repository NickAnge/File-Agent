# File-Agent

File-Agent is a event-driven Windows service written in C++. This service checks "movement" of files in directories like (C:/,D:/ or other mounted directories).


It is at an early stage as it sends a simple message to the event viewer application of windows, when a simple creation, deletion , Update or read of a file is happening(CRUD).


