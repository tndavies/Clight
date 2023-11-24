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

enum class CharClass {
	Delimiter,
	Quotation,
	Numeral,
	Symbol
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

	bool NextToken(Token& token);

private:
	CharClass ClassifyCharacter(uint8_t c, bool in_sLiteral, bool in_nLiteral);
	void MakeToken(std::string& buffer, size_t blob_index, Token& token);
	bool ConsumeInputStream(uint8_t& c);

private:
	std::string m_Blob; // make const!
	size_t m_ReadIdx = 0;
};