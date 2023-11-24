#include <tokens.hpp>
#include <iostream>


bool Lexer::NextToken(Token& token) 
{
	#define yield() goto yield_token
	
	// We have run out of input stream to lex.
	if (m_ReadIdx == m_Blob.length()) 
		return false;

	bool inside_sliteral = false;
	bool inside_nliteral = false;

	size_t token_origin = m_ReadIdx;
	std::string buffer;

	uint8_t c = NULL;
	bool InputStreamWasRead = NULL;
	while (InputStreamWasRead = ConsumeInputStream(c))
	{
		switch (ClassifyCharacter(c, inside_sliteral, inside_nliteral))
		{
			case CharClass::Delimiter:
			{
				if (c == ' ') {
					// If buffer is populated, emit token & and skip whitespace 
					// ready for the next call to next_token; if buffer is empty, 
					// find the next token after the whitespace, and return.

					if (!buffer.length())
						return NextToken(token);
					else {
						MakeToken(buffer, token_origin, token);
						yield();
					}
				}
				else { // not letter/number/whitespace
					
					// If buffer is populated, emit token & ensure we stay on the
					// delimiter/operator for the next call to next_token;
					// if buffer is empty, then emit delimiter/operator as token.

					if (buffer.length()) --m_ReadIdx;
					else buffer += c; 

					MakeToken(buffer, token_origin, token);
					yield();
				}
			}
			break;
			
			case CharClass::Quotation:
			{
				if (inside_sliteral) {
					MakeToken(buffer, token_origin + 1, token);
					yield();
				}

				inside_sliteral = !inside_sliteral;
			}
			break;

			case CharClass::Numeral:
			{
				inside_nliteral = !inside_nliteral;
			}
			break;

			case CharClass::Symbol:
			{
				buffer += c;
			}
			break;
		}
	}

yield_token:

	if (!InputStreamWasRead && buffer.length())
		MakeToken(buffer, token_origin, token);

	return true;
}

void Lexer::MakeToken(std::string& buffer, size_t blob_index, Token& token)
{
	token.idx = blob_index;
	token.len = buffer.length();
	token.ident = buffer;
	buffer.clear();
}

CharClass Lexer::ClassifyCharacter(uint8_t c, bool in_sLiteral, bool in_nLiteral)
{
	bool is_operator = c == '=' || c == '+' || c == '-' || c == '*' ||
		c == '/' || c == '<' || c == '>' || c == '^' || c == '%';
	
	bool is_auxchar = c == '(' || c == ')' || c == '{' ||
		c == '}' || c == ',' || c == '.';
	
	if (c == ' ' || is_operator || is_auxchar)
		return (!in_sLiteral) ? CharClass::Delimiter : CharClass::Symbol;
	else if (c == '\"')
		return CharClass::Quotation;
	else if (std::isdigit(c))
		return CharClass::Numeral;
	else
		return CharClass::Symbol;

}

bool Lexer::ConsumeInputStream(uint8_t& c)
{
	bool InputStreamRemaining = m_ReadIdx < m_Blob.length();
	if (InputStreamRemaining)
		c = m_Blob[m_ReadIdx++];

	return InputStreamRemaining;
}