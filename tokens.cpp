#include <tokens.hpp>
#include <iostream>

// foo = my_function(abc+3.1415**2.156e30, 103)

Lexer::Lexer(const char* blob) 
{
	m_Blob = std::string(blob);
}

bool Lexer::next(Token& token) 
{
	std::string buffer;
	auto idx = m_Index;

	while (idx < m_Blob.length())
	{
		uint8_t c = m_Blob[idx];
		
		if (IsWhiteSpace(c)) {
			if (buffer.length()) {
				YieldToken(buffer, m_Index, token);
				m_Index = idx + 1;
				break;
			}
			else {
				m_Index = idx + 1;
				next(token);
				break;
			}
			
		}
		else if (IsDelimiter(c)) {
			if (!buffer.length()) {
				buffer += c;
				YieldToken(buffer, m_Index, token);
				m_Index = idx + 1;
				break;
			}

			YieldToken(buffer, m_Index, token);
			m_Index = idx;
			break;
		}
		else if (IsOperator(c)) {
			if (!buffer.length()) {
				buffer += c;
				YieldToken(buffer, m_Index, token);
				m_Index = idx + 1;
				break;
			}
		
			YieldToken(buffer, m_Index, token);
			m_Index = idx;
			break;
			
		}

		buffer += c;
		++idx;
	}

	if (buffer.length()) {
		YieldToken(buffer, m_Index, token);
	}

	return idx < m_Blob.length();
}

void Lexer::YieldToken(std::string& buffer, size_t blob_index, Token& token)
{
	token.idx = blob_index;
	token.len = buffer.length();
	token.ident = buffer;

	buffer.clear();
}

bool Lexer::IsWhiteSpace(uint8_t c)
{
	return c == ' ';
}

bool Lexer::IsOperator(uint8_t c)
{
	bool result = c == '=' ||c == '+' || c == '-' || c == '*' || 
		c == '/' || c == '<' || c == '>' ||c == '^' || c == '%';
	
	return result;
}

bool Lexer::IsDelimiter(uint8_t c)
{
	bool result = c == '(' || c == ')' || c == '{' || 
		c == '}' || c == ',';

	return result;
}