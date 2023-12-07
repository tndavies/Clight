#pragma once

#include <string>

#define RewindInputStream() (m_ReadIdx--)
#define EndOfInputStream	0x00

enum class LexMode {
	Default,
	String,
	Numeric
};

enum class TokenType {
	Invalid,
	Keyword,
	Identifier,
	Operator,
	Seperator,
	String_Literal,
	Number_Literal,
	Comment
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
	LexMode m_Mode;

	void DefaultLexingPath(Token& token, const std::uint8_t c, const std::size_t origin, std::size_t& len, 
		std::uint8_t& StringLiteralPrefix, bool& lexing_comment);

	void StringLexingPath(Token& token, const std::uint8_t c, const std::size_t origin, std::size_t& len,
		std::uint8_t& StringLiteralPrefix, bool& lexing_comment);

	void NumericLexingPath(Token& token, const std::uint8_t c, const std::size_t origin, std::size_t& len,
		std::uint8_t& StringLiteralPrefix, bool& lexing_comment);

	bool ConsumeFromInputStream(std::uint8_t& c);
	
	bool IsNumericSymbol(std::uint8_t c);
	
	bool MatchKeyword(const std::size_t origin, const std::size_t len);
	
	void YieldToken(const std::size_t origin, const std::size_t len,
		Token& token, TokenType type = TokenType::Invalid);

public:
	Lexer(const char* blob)
		: m_ReadIdx(0), m_Blob(blob), m_Mode(LexMode::Default) 
	{}

	[[nodiscard]] Token NextToken();
};