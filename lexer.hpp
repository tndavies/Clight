#pragma once

#include <string>

#define RewindInputStream() (m_ReadIdx--)
#define EndOfInputStream	0x00

enum class LexMode {
	Default,
	String,
	Number
};

enum class TokenType {
	Invalid,
	Keyword,
	Identifier,
	Operator,
	Seperator,
	String_Literal,
	Number_Literal
};

struct Token {
	TokenType type;
	size_t idx;
	size_t len;

	// Tokens convert to true if they are valid,
	// and false if they are invalid.
	operator bool() const { return (type != TokenType::Invalid); }
};

class Lexer {

private:
	const std::string m_Blob;
	size_t m_ReadIdx;
	LexMode m_State;

	void YieldToken(std::string& buffer, size_t origin, TokenType type, Token& token);
	bool ConsumeInputStream(uint8_t& c);
	bool IsNumericSymbol(std::uint8_t c);
	TokenType InferTokenType(const std::string& token);

	bool IsDelimiter(uint8_t c);
	bool IsOperator(std::uint8_t c);
	bool IsVirtualDelimiter(std::uint8_t c);

public:
	Lexer(const char* blob)
		: m_ReadIdx(0), m_Blob(blob), m_State(LexMode::Default) 
	{}

	[[nodiscard]] Token NextToken();
};