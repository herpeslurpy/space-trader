mainmake: trader.cpp solsystem.cpp
	g++ trader.cpp solsystem.cpp -o space-trader -l curses
