#pragma once
#include <string>
#include <chrono>

#define RewindInputStream() (m_ReadIdx--)
#define EndOfInputStream	0x00

enum class LexMode {
	Default,
	String_Literal,
	Number_Literal,
	Preprocessor_Directive,
	SingleLineComment,
	MultiLineComment
};

enum class TokenType {
	Invalid,
	Keyword,
	Datatype,
	Identifier,
	Operator,
	Seperator,
	String_Literal,
	Char_Literal,
	Number_Literal,
	Preprocessor,
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
	std::chrono::microseconds m_ElapsedTime;
	std::vector<Token> m_Tokens;
	const std::string m_Blob;
	std::size_t m_ReadIdx;
	LexMode m_Mode;

	void DefaultLexingPath(Token& token, const std::uint8_t c, const std::size_t origin, std::size_t& len, 
		std::uint8_t& sliteral_terminator);

	void LexStringLiteral(Token& token, const std::uint8_t c, const std::size_t origin, std::size_t& len,
		std::uint8_t& sliteral_terminator, std::size_t& backslash_count);

	void LexNumberLiteral(Token& token, const std::uint8_t c, const std::size_t origin, std::size_t& len,
		std::uint8_t& sliteral_terminator);

	void LexPreprocessorDirective(Token& token, const std::uint8_t c, const std::size_t origin, std::size_t& len,
		std::uint8_t& sliteral_terminator);

	bool ConsumeFromInputStream(std::uint8_t& c);
	
	bool IsNumericSymbol(std::uint8_t c);
	
	TokenType MatchKeyword(const std::size_t origin, const std::size_t len);
	
	void YieldToken(const std::size_t origin, const std::size_t len,
		Token& token, TokenType type = TokenType::Invalid);

	bool IsMathSymbol(const std::uint8_t c);

	[[nodiscard]] Token NextToken();
public:
	Lexer(const char* blob, bool defer_lex = false);
	auto getElapsedTime() const { return m_ElapsedTime; }
	auto getBlobSize() const { return m_Blob.length(); }
	const std::vector<Token>& getTokens() const { return m_Tokens; }
	void LexBlob();
};