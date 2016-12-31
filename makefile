mainmake: trader.cpp solsystem.cpp
	g++ trader.cpp solsystem.cpp -o trader -l curses
