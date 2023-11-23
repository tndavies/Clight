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

struct Token {
	TokenClass type;
	size_t idx;
	size_t len;

	std::string ident;
};

class Lexer {

public:
	Lexer(const char* blob);

	bool next(Token& token);

private:
	bool IsOperator(uint8_t c);
	bool IsDelimiter(uint8_t c);
	bool IsWhiteSpace(uint8_t c);
	void YieldToken(std::string& buffer, size_t blob_index, Token& token);

private:
	std::string m_Blob;
	size_t m_Index = 0;
};