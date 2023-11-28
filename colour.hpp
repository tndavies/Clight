#pragma once
#include <vector>
#include <string>

enum class Colour {
	Default = 0,
	Black,
	Red,
	Green,
	Yellow,
	Blue,
	Magenta,
	Cyan,
	White
};

class Palette {
public:
	static std::string get(Colour col) {
		switch (col) {
			case Colour::Black: 	return "\033[30m"; 	break;
			case Colour::Red: 		return "\033[31m"; 	break;
			case Colour::Green: 	return "\033[32m"; 	break;
			case Colour::Yellow: 	return "\033[33m"; 	break;
			case Colour::Blue: 		return "\033[34m"; 	break;
			case Colour::Magenta: 	return "\033[35m"; 	break;
			case Colour::Cyan: 		return "\033[36m"; 	break;
			case Colour::White: 	return "\033[37m"; 	break;

			default: return "\033[m";
		}
	}
};