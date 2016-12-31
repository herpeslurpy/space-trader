# Space Trader
A simple game with a text-based UI, made with the curses library for C++.
Sort of inspired by games like Escape Velocity and Endless Sky.

## Building
The makefile included should work just fine, simply run ```make```.
If not, manual compilation should look something like:
```
g++ trader.cpp solsystem.cpp -o <name> -lcurses
```
where `<name>` is whatever you'd prefer to call the executable.

You'll need all the included files, G++, and the C/C++ Curses library.
Currently it's also entirely compatible with the newer ncurses library,
if you'd rather use that. If you want to, in trader.cpp, replace
```
#include <curses.h>
```
with
```
#include <ncurses.h>
```
and when compiling, replace
```
-lcurses
```
with
```
-lncurses
```

If you want to use the makefile, make the same change within it.

I've tested compiling with a few C++ versions, back to at least C++98,
so any modern Linux machine should be good to go.

Also, this was written with terminal dimensions of 80x24 in mind,
so adjust those if necessary. A bigger terminal won't be too wonky,
but I can't guarantee anything smaller will be usable.

## Contributing
This project is licensed under the MIT license. For now this is a very small personal project that probably won't be receiving much, if any attention in two weeks from now. If it ever grows beyond that, I'll open it up.
