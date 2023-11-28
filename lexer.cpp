#include <lexer.hpp>
#include <iostream>

/* todo list of features

	1) Handle character literals, ie: 'A', '\n' etc.
	2) Handle newline feeds properly
	3) single/multiline comments
	4) keywords
*/

Token Lexer::NextToken() 
{
	Token token = { .type = TokenType::Invalid };
	
	std::size_t token_origin = m_ReadIdx;
	bool InputStreamWasRead = NULL;
	std::string buffer;
	uint8_t c = NULL;

	while (token.type == TokenType::Invalid &&
		(InputStreamWasRead = ConsumeInputStream(c))) 
	{
		// handle string literals.
		if (c == '\"') {
			if (m_State == LexMode::Default) {
				if (buffer.length()) {
					auto type = InferTokenType(buffer);
					YieldToken(buffer, token_origin, type, token);
				}
				
				m_State = LexMode::String;
				++token_origin; // ignore " in emitted token.
			}
			else {
				if (m_State == LexMode::Number && c != ' ')
					RewindInputStream();

				YieldToken(buffer, token_origin, TokenType::String_Literal, token);
				m_State = LexMode::Default;
			}

			continue;
		}

		// handle number literals.
		if (std::isdigit(c) && m_State == LexMode::Default) {
			if (buffer.length()) {
				auto type = InferTokenType(buffer);
				YieldToken(buffer, token_origin, type, token);
				RewindInputStream();
			}

			m_State = LexMode::Number;
		}
		else if (!std::isdigit(c) && !IsNumericSymbol(c) 
			&& m_State == LexMode::Number) 
		{
			YieldToken(buffer, token_origin, TokenType::Number_Literal, token);
			if (!IsVirtualDelimiter(c)) RewindInputStream();

			m_State = LexMode::Default;

			continue;
		}

		// build token.
		if(IsDelimiter(c)) {
				if (IsVirtualDelimiter(c) && buffer.length()) { 
					// flush buffer
					auto type = InferTokenType(buffer);
					YieldToken(buffer, token_origin, type, token);
				}
				else if (IsVirtualDelimiter(c) && !buffer.length()) { 
					// process virtual delimiter
					token = NextToken();
				}
				else if (c != ' ' && buffer.length()) { 
					// flush buffer
					auto type = InferTokenType(buffer);
					YieldToken(buffer, token_origin, type, token);
					RewindInputStream();
				}
				else if (c != ' ' && !buffer.length()) { 
					// process physical delimiter
					buffer += c;
					YieldToken(buffer, token_origin, TokenType::Seperator, token);
				}
		} 
		else buffer += c;

	}

	return token;
}

TokenType Lexer::InferTokenType(const std::string& token) {
	if (token.length() == 1 && IsOperator(token[0]))
		return TokenType::Operator;
	else
		return TokenType::Identifier;
}

void Lexer::YieldToken(std::string& buffer, size_t origin, TokenType type, Token& token) {
	token.idx = origin;
	token.len = buffer.length();
	token.type = type;

	buffer.clear();
}

bool Lexer::ConsumeInputStream(uint8_t& c) {
	bool InputStreamRemaining = m_ReadIdx <= m_Blob.length();
	
	c = (InputStreamRemaining) ? m_Blob[m_ReadIdx] : EndOfInputStream;
	m_ReadIdx++;

	return InputStreamRemaining;
}

bool Lexer::IsDelimiter(uint8_t c) {
	if (c == EndOfInputStream)
		return true;

	switch (m_State) {
	case LexMode::Default:
		return (std::ispunct(c) || c == ' ');
		break;

	case LexMode::Number:
		return !(std::isdigit(c) || IsNumericSymbol(c));
		break;

	case LexMode::String:
		return false;
		break;
	}
}

bool Lexer::IsOperator(std::uint8_t c) {
	return (c == '+' || c == '-' || c == '*' || c == '/' ||
		c == '^' || c == '%');
}

bool Lexer::IsVirtualDelimiter(std::uint8_t c) {
	return (c == ' ' || c == EndOfInputStream);
}

bool Lexer::IsNumericSymbol(std::uint8_t c) {
	return c == '.' || c == 'e';
}