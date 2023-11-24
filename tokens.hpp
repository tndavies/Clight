#pragma once
#include <colour.hpp>

enum class TokenClass {
	Keyword,
	Identifier,
	Seperator,
	Operator,
	String_Literal,
	Numeric_Literal
};

enum class InputStreamClass {
	Whitespace,
	Delimiter,
	Operator,
	Symbol	// ie: letter, digit etc.
};

struct Token {
	TokenClass type;
	size_t idx;
	size_t len;

	std::string ident;
};

class Lexer {

public:
	Lexer(const char* blob)
		: m_Blob(blob) {}

	bool next_token(Token& token);

private:
	InputStreamClass ClassifyCharacter(uint8_t c);
	void MakeToken(std::string& buffer, size_t blob_index, Token& token);
	bool ConsumeInputStream(uint8_t& c);

private:
	std::string m_Blob; // make const!
	size_t m_ReadIdx = 0;
};