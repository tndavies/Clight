#pragma once
#include <string>
#include <chrono>

#define RewindInputStream() (m_ReadIdx--)
#define EndOfInputStream	0x00

enum class LexMode {
	Default,
	String_Literal,
	Number_Literal,
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
	size_t origin;
	size_t len;

	// Tokens convert to true if they are valid,
	// and false if they are invalid.
	operator bool() const { return (type != TokenType::Invalid); }
};

class Highlighter {
public:
	Highlighter(const char* blob, bool defer_lex = false)
		: m_ReadIdx(0), m_Blob(blob), m_Mode(LexMode::Default) {}

	/* Finds the next token within the blob */
	[[nodiscard]] Token NextToken();

	/* 
		Finds all tokens within the blob, and puts them into a list; the 
		list is accessible via the 'getToken' method. 
	*/
	void Parse();

	/* Various Lexing Paths */
	void DefaultMode(Token& token, const std::uint8_t c, std::uint8_t& string_terminator);
	void StringLiteralMode(Token& token, const std::uint8_t c, std::uint8_t& string_terminator, std::size_t& backslash_count);
	void NumberLiteralMode(Token& token, const std::uint8_t c, std::uint8_t& string_terminator);
	void SingleCommentMode(Token& token, const std::uint8_t c);
	void MultiCommentMode(Token& token, const std::uint8_t c);

	/* Various Utility Functions */
	bool ConsumeFromInputStream(std::uint8_t& c);
	bool IsNumericSymbol(const std::uint8_t c);
	bool AppearsIn(const std::vector<const char*>& dict, const std::string& string);
	void InferTokenType(Token& token, TokenType type = TokenType::Invalid);
	bool IsMathSymbol(const std::uint8_t c);
	auto getBlobSize() const { return m_Blob.length(); }
	const std::vector<Token>& getTokens() const { return m_Tokens; }

private:
	std::vector<Token> m_Tokens;
	const std::string m_Blob;

	std::size_t m_ReadIdx;
	LexMode m_Mode;
};