#pragma once
#include <colour.hpp>

struct Token {
	size_t idx;
	size_t len;
	Colour col;
};

class Tokenizer {

public:
	void parse(std::string text);

	auto begin() { return m_Tokens.begin(); }
	auto end() { return m_Tokens.end(); }

	static void assign(Token& token);

private:
	std::vector<Token> m_Tokens;
};