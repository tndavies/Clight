#include <tokens.hpp>
#include <iostream>

// "foo  abc" 

bool Lexer::ConsumeInputStream(uint8_t& c)
{
	bool InputStreamRemaining = m_ReadIdx < m_Blob.length();
	if (InputStreamRemaining)
		c = m_Blob[m_ReadIdx++];

	return InputStreamRemaining;
}

bool Lexer::next_token(Token& token) 
{
	bool failed_to_find_token = false;
	size_t token_origin = m_ReadIdx;
	std::string buffer;

	uint8_t c = NULL;
	bool InputStreamWasRead = NULL;
	while (InputStreamWasRead = ConsumeInputStream(c))
	{
		switch (ClassifyCharacter(c)) 
		{
			case InputStreamClass::Whitespace:
			{
				if (buffer.length()) {
					MakeToken(buffer, token_origin, token);
					goto yield_token;
				}
				else
					return next_token(token);
			}
			break;

			case InputStreamClass::Delimiter:
			case InputStreamClass::Operator:
			{
				if (buffer.length())
					--m_ReadIdx; // stay on delimiter/operator for next token call.
				else
					buffer += c;

				MakeToken(buffer, token_origin, token);
				goto yield_token;
			}
			break;

			case InputStreamClass::Symbol:
			{
				buffer += c;
			}
			break;
		}
	}

yield_token:

	if (!InputStreamWasRead && buffer.length())
		MakeToken(buffer, token_origin, token);

	return InputStreamWasRead;
}

void Lexer::MakeToken(std::string& buffer, size_t blob_index, Token& token)
{
	token.idx = blob_index;
	token.len = buffer.length();
	token.ident = buffer;
	buffer.clear();
}

InputStreamClass Lexer::ClassifyCharacter(uint8_t c)
{
	bool is_whitespace = (c == ' ');
	
	bool is_operator = c == '=' || c == '+' || c == '-' || c == '*' ||
		c == '/' || c == '<' || c == '>' || c == '^' || c == '%';
	
	bool is_delimiter = c == '(' || c == ')' || c == '{' ||
		c == '}' || c == ',';
	
	if (is_whitespace)		return InputStreamClass::Whitespace;
	else if (is_operator)	return InputStreamClass::Operator;
	else if (is_delimiter)	return InputStreamClass::Delimiter;
	else					return InputStreamClass::Symbol;
}