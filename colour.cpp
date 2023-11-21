#include <colour.hpp>

void Palette::Init() {
	m_Colours.push_back("[m");		// Default (terminal's text colour)
	m_Colours.push_back("[30m"); 	// Black
	m_Colours.push_back("[31m");	// Red
	m_Colours.push_back("[32m"); 	// Green
	m_Colours.push_back("[33m"); 	// Yellow
	m_Colours.push_back("[34m"); 	// Blue
	m_Colours.push_back("[35m"); 	// Magenta
	m_Colours.push_back("[36m"); 	// Cyan
	m_Colours.push_back("[37m");  	// White
}

std::string Palette::get(Colour col) {
	return std::string(m_Colours[(size_t)col]);
}

std::vector<const char*> Palette::m_Colours;