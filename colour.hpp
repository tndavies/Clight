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
	static void Init();
	static std::string get(Colour col);

private:
	static std::vector<const char*> m_Colours;
};