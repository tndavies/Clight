#pragma once

#include <string>

enum class TokenClass {
	Invalid,
	Keyword,
	Identifier,
	Seperator,
	Operator,
	String,
	Number
};

enum class CharClass {
	Ignore,
	Delimiter,
	Symbol
};

enum class LexingMode {
	Default,
	String,
	Number
};

struct Token {
	TokenClass type;
	size_t idx;
	size_t len;

	std::string ident;

	// Tokens convert to true if they are valid,
	// and false if they are invalid.
	operator bool() const { return (type != TokenClass::Invalid); }
};

class Lexer {

public:
	Lexer(const char* blob)
		: m_ReadIdx(0), m_Blob(blob),
	m_State(LexingMode::Default){}

	[[nodiscard]] Token NextToken();

private:
	CharClass ClassifyCharacter(uint8_t c);
	
	void YieldToken(std::string& buffer, size_t blob_index, Token& token);
	
	bool ConsumeInputStream(uint8_t& c);

	bool IsNumericSymbol(std::uint8_t c);

private:
	std::string m_Blob; // fix: we want to enusre you can't mutate this!!
	size_t m_ReadIdx;
	LexingMode m_State;
};