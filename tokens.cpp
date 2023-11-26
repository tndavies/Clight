#include <tokens.hpp>
#include <iostream>

Token Lexer::NextToken() 
{
	Token token = { .type = TokenClass::Invalid };
	
	std::size_t token_origin = m_ReadIdx;
	bool InputStreamWasRead = NULL;
	std::string buffer;
	uint8_t c = NULL;

	while (token.type == TokenClass::Invalid &&
		(InputStreamWasRead = ConsumeInputStream(c))) 
	{
		// handle string literals.
		if (c == '\"') {
			if (m_State == LexingMode::Default) {
				if (buffer.length())
					YieldToken(buffer, token_origin, token);
				
				m_State = LexingMode::String;
				++token_origin; // ignore " in emitted token.
			}
			else if(m_State == LexingMode::String) {
				m_State = LexingMode::Default;
				YieldToken(buffer, token_origin, token);
			}
			else if (m_State == LexingMode::Number) {
				YieldToken(buffer, token_origin, token);
				if (c != ' ') m_ReadIdx--;
				
				m_State = LexingMode::Default;
			}

			continue;
		}

		// handle number literals.
		if (std::isdigit(c) && m_State == LexingMode::Default) {
			m_State = LexingMode::Number;
		}
		else if (!std::isdigit(c) && !IsNumericSymbol(c) 
			&& m_State == LexingMode::Number) 
		{
			YieldToken(buffer, token_origin, token);
			if (c != ' ') m_ReadIdx--;

			m_State = LexingMode::Default;

			continue;
		}

		// build token.
		switch(ClassifyCharacter(c)) {
			case CharClass::Delimiter:
			{
				// whitespace
				if (c == ' ' && buffer.length()) 
					YieldToken(buffer, token_origin, token);

				else if (c == ' ' && !buffer.length()) 
					token = NextToken();

				// non-whitespace
				else if (c != ' ' && buffer.length()) {
					YieldToken(buffer, token_origin, token);
					m_ReadIdx--;
				}
				else if (c != ' ' && !buffer.length()) {
					buffer += c;
					YieldToken(buffer, token_origin, token);
				}
			}
			break;
			
			case CharClass::Symbol: 
				buffer += c;
			break;
		}
	}

	// flush token buffer once the input stream  
	// has been fully consumed.
	if (!InputStreamWasRead && buffer.length())
		YieldToken(buffer, token_origin, token);

	return token;
}

CharClass Lexer::ClassifyCharacter(uint8_t c) {	
	switch (m_State) {
		case LexingMode::Default:
			return (std::ispunct(c) || c == ' ') ? CharClass::Delimiter : CharClass::Symbol;
		break;

		case LexingMode::Number:
			if (std::isdigit(c) || IsNumericSymbol(c)) return CharClass::Symbol;
			else return CharClass::Delimiter;
		break;

		case LexingMode::String:
			return CharClass::Symbol;
		break;
	}
}

bool Lexer::IsNumericSymbol(std::uint8_t c) {
	return c == '.' || c == 'e';
}

void Lexer::YieldToken(std::string& buffer, size_t blob_index, Token& token) {
	token.idx = blob_index;
	token.len = buffer.length();
	token.ident = buffer;
	token.type = TokenClass::Identifier;
	buffer.clear();
}

bool Lexer::ConsumeInputStream(uint8_t& c) {
	bool InputStreamRemaining = m_ReadIdx < m_Blob.length();
	
	if (InputStreamRemaining)
		c = m_Blob[m_ReadIdx++];

	return InputStreamRemaining;
}