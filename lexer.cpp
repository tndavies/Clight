#include <lexer.hpp>
#include <iostream>

/* todo list of features
	3) single/multiline comments
	4) keywords
*/

bool IsOperator(const std::uint8_t c) {
	return c == '+' || c == '-' || c == '*' || c == '/' || c == '%' || c == '=';
}

void Lexer::DefaultLexingPath(Token& token, const std::uint8_t c, 
	const std::size_t origin, std::size_t& len, std::uint8_t& StringLiteralPrefix,
	bool& lexing_comment)
{
	if (std::isspace(c)) {
		if (len) YieldToken(origin, len, token);
		else token = NextToken();
	}
	else if (c == '\"' || c == '\'') {
		if (len) {
			YieldToken(origin, len, token);
			RewindInputStream();
		}
		else {
			m_Mode = LexMode::String;
			StringLiteralPrefix = c;
			len++;
		}
	}
	else if (c == '#') { 
		if (len) {
			YieldToken(origin, len, token);
			RewindInputStream();
		}
		else {
			m_Mode = LexMode::String;
			lexing_comment = true;
			len++;
		}
	}
	else if (std::ispunct(c)) {
		if (len) {
			YieldToken(origin, len, token);
			RewindInputStream();
		}
		else {
			auto type = IsOperator(c) ? TokenType::Operator : TokenType::Seperator;
			YieldToken(origin, 1, token, type);
		}
	}
	else if (std::isdigit(c) && !len) {
		m_Mode = LexMode::Numeric;
		len++;
	}
	else if (c == EndOfInputStream && len) {
		YieldToken(origin, len, token);
	}
	else {
		len++;
	}
}

void Lexer::StringLexingPath(Token& token, const std::uint8_t c,
	const std::size_t origin, std::size_t& len, std::uint8_t& StringLiteralPrefix,
	bool& lexing_comment)
{
	len++;

	if (!lexing_comment && c == StringLiteralPrefix || c == EndOfInputStream) {
		YieldToken(origin, len, token, TokenType::String_Literal);
		m_Mode = LexMode::Default;
	}
	else if (lexing_comment && c == '\n' || c == EndOfInputStream) {
		YieldToken(origin, len - 1, token, TokenType::Comment);
		m_Mode = LexMode::Default;
	}
}

void Lexer::NumericLexingPath(Token& token, const std::uint8_t c,
	const std::size_t origin, std::size_t& len, std::uint8_t& StringLiteralPrefix,
	bool& lexing_comment)
{
	if (!IsNumericSymbol(c)) {
		YieldToken(origin, len, token, TokenType::Number_Literal);
		m_Mode = LexMode::Default;
		RewindInputStream();
	}
	else {
		len++;
	}
}

Token Lexer::NextToken() {
	Token token = { .type = TokenType::Invalid };
	std::size_t origin = m_ReadIdx;
	std::size_t len = 0;

	bool lexing_comment = false;
	std::uint8_t StringLiteralPrefix = NULL;
	bool InputStreamWasRead = false;
	std::uint8_t c = NULL;

	while (token.type == TokenType::Invalid && 
		(InputStreamWasRead = ConsumeFromInputStream(c)) )
	{
		if (c == '\r') continue;

		switch (m_Mode) {
			case LexMode::Default:
				DefaultLexingPath(token, c, origin, len, StringLiteralPrefix, lexing_comment);
				break;

			case LexMode::String:
				StringLexingPath(token, c, origin, len, StringLiteralPrefix, lexing_comment);
				break;

			case LexMode::Numeric:
				NumericLexingPath(token, c, origin, len, StringLiteralPrefix, lexing_comment);
				break;
		}
	}

	return token;
}

TokenType Lexer::InferTokenType(const std::size_t origin, const std::size_t len) {
	return TokenType::Identifier;
}

void Lexer::YieldToken(const std::size_t origin, const std::size_t len,
	Token& token, TokenType type) 
{
	token.type = (type == TokenType::Invalid) ? InferTokenType(origin, len) : type;
	token.idx = origin;
	token.len = len;
}

bool Lexer::ConsumeFromInputStream(std::uint8_t& c) {
	if (m_ReadIdx < m_Blob.length()) {
		c = m_Blob[m_ReadIdx++];
		return true;
	}
	else if(m_ReadIdx == m_Blob.length()) {
		c = EndOfInputStream;
		m_ReadIdx++;
		return true;
	}
	else {
		return false;
	}
}

bool Lexer::IsNumericSymbol(std::uint8_t c) {
	return std::isdigit(c) || c == '.' || c == 'e' || c == '-';
}