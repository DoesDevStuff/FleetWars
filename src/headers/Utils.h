/*
 * Utils.h
 *
 *  Created on: 23 Feb 2024
 *      Author: Charlie
 */

#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <iostream>

#ifdef _WIN32
#include <windows.h>
#else
#include <cstdlib>
#endif

namespace Utils {
	enum GAME_DIFFICULTY {
		easy,
		medium,
		hard
	};

	enum SHIP_TYPE {
		aircraft_carrier,
		battleship,
		destroyer,
		patrolboat,
		submarine
	};

	enum SHIP_ORIENTATION {
		horizontal,
		vertical
	};

	/* https://stackoverflow.com/a/6487534
	 * https://learn.microsoft.com/en-us/windows/console/console-functions
	 */

	inline void clearScreen() {
	#ifdef _WIN32
	    // Windows: Use Windows Console API
	    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	    COORD coordScreen = {0, 0};
	    DWORD cCharsWritten;
	    CONSOLE_SCREEN_BUFFER_INFO csbi;
	    DWORD dwConSize;

	    GetConsoleScreenBufferInfo(hConsole, &csbi);
	    dwConSize = csbi.dwSize.X * csbi.dwSize.Y;
	    FillConsoleOutputCharacter(hConsole, ' ', dwConSize, coordScreen, &cCharsWritten);
	    GetConsoleScreenBufferInfo(hConsole, &csbi);
	    FillConsoleOutputAttribute(hConsole, csbi.wAttributes, dwConSize, coordScreen, &cCharsWritten);
	    SetConsoleCursorPosition(hConsole, coordScreen);
	#else
	    // POSIX systems including Linux, macOS: Use ANSI escape codes
	    std::cout << "\033[2J\033[1;1H";
	#endif
	}

	inline void printCenter(std::string s, int i) {
	    std::string indent((i * 2 - s.length()) / 2, ' ');
	    std::cout << indent << s;
	}

	inline void pause() {
	    do {
	        std::cout << "Press the Enter key to continue." << '\n';
	    } while (std::cin.get() != '\n');
	}
};



#endif /* UTILS_H */
