#include <lexer.hpp>
#include <iostream>

const std::vector<const char*> Preprocessor_Keywords = {
	"#define", "#elif", "#else",
	"#endif", "#error", "#if",
	"#ifdef", "#ifndef", "#import",
	"#include", "#line", "#pragma",
	"#undef", "#using"
};

const std::vector<const char*> DataType_Keywords = {
	"auto", "bool", "char", "size_t",
	"uint8_t", "uint16_t", "uint32_t",
	"int8_t", "int16_t", "int32_t",
	"double", "float", "enum", 
	"int", "long", "short", "signed",
	"unsigned", "signed", "void",
	"wchar_t"
};

const std::vector<const char*> Language_Keywords = {
	"alignas", "alignof", "and", "and_eq", "asm",
	"atomic_cancel", "atomic_commit", "atomic_noexcept",
	"bitand", "bitor", "break", "case", "catch",
	"class", "compl", "concept",
	"const", "consteval", "constexpr", "constinit", "const_cast",
	"continue", "co_await", "co_return", "co_yield", "decltype",
	"default", "delete", "do", "dynamic_cast", "else",
	"explicit", "export", "extern", "false", "for",
	"friend", "goto", "if", "inline", "mutable", "namespace",
	"new", "noexcept", "not", "not_eq", "nullptr", "operator", "or", "or_eq",
	"private", "protected", "public", "reflexpr", "register", "reinterpret_cast",
	"requires", "return", "sizeof", "static", "static_assert",
	"static_cast", "struct", "switch", "synchronized", "template", "this",
	"thread_local", "throw", "true", "try", "typedef", "typeid", "typename",
	"union", "using", "virtual", "volatile",
	"while", "xor", "xor_eq"
};

Lexer::Lexer(const char* blob, bool defer_lex)
	: m_ReadIdx(0), m_Blob(blob), m_Mode(LexMode::Default) , m_ElapsedTime(0)
{
	if(!defer_lex) 
		LexBlob();
}

void Lexer::LexBlob() {
	auto t0 = std::chrono::steady_clock::now();

	Token token;
	while (token = NextToken()) {
		m_Tokens.push_back(token);
	}

	auto t1 = std::chrono::steady_clock::now();
	m_ElapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0);
}

void Lexer::DefaultLexingPath(Token& token, const std::uint8_t c, 
	const std::size_t origin, std::size_t& len, std::uint8_t& sliteral_terminator)
{
	bool last_token_was_preproc = m_Tokens.size() ? (m_Tokens.back().type == TokenType::Preprocessor) : false; 
	
	if (std::isspace(c)) {
		if (len) YieldToken(origin, len, token);
		else token = NextToken();
	}
	// @note: EndOfInputStream prevents possible read access violation here.
	else if(c == '/' && m_Blob[m_ReadIdx] == '/') {
		m_Mode = LexMode::SingleLineComment;
		len++;
	}
	// @note: EndOfInputStream prevents possible read access violation here.
	else if(c == '/' && m_Blob[m_ReadIdx] == '*') {
		m_Mode = LexMode::MultiLineComment;
		len++;
	}
	else if (c == '\'' || c == '\"' || (last_token_was_preproc && c == '<')) {
		if (len) {
			YieldToken(origin, len, token);
			RewindInputStream();
		}
		else {
			m_Mode = LexMode::String_Literal;
			sliteral_terminator = (c == '<') ? '>' : c;
			len++;
		}
	}
	else if (std::ispunct(c) && c != '_' && c != '#') {
		if (len) {
			YieldToken(origin, len, token);
			RewindInputStream();
		}
		else {
			auto type = IsMathSymbol(c) ? TokenType::Operator : TokenType::Seperator;
			YieldToken(origin, 1, token, type);
		}
	}
	else if (std::isdigit(c) && !len) {
		m_Mode = LexMode::Number_Literal;
		len++;
	}
	else if (c == EndOfInputStream && len) {
		YieldToken(origin, len, token);
	}
	else {
		len++;
	}
}

void Lexer::LexStringLiteral(Token& token, const std::uint8_t c,
	const std::size_t origin, std::size_t& len, std::uint8_t& sliteral_terminator,
	std::size_t& backslash_count)
{
	len++;
	
	if(c == EndOfInputStream) {
		auto type = (sliteral_terminator == '\'') ? TokenType::Char_Literal : TokenType::String_Literal; 
		YieldToken(origin, len - 1, token, type);
		m_Mode = LexMode::Default;
		return;
	}
	else if(c == sliteral_terminator) {
		bool escaped = static_cast<bool>(backslash_count % 2);
		backslash_count = 0;
		
		if(!escaped) {
			auto type = (sliteral_terminator == '\'') ? TokenType::Char_Literal : TokenType::String_Literal; 
			YieldToken(origin, len, token, type);
			m_Mode = LexMode::Default;
		}
	}
	else if(c == '\\') {
		backslash_count++;
	}
	else {
		backslash_count = 0;
	}
}

void Lexer::LexNumberLiteral(Token& token, const std::uint8_t c,
	const std::size_t origin, std::size_t& len, std::uint8_t& sliteral_terminator)
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

	std::size_t backslash_count = 0;
	std::uint8_t sliteral_terminator = NULL;
	
	std::uint8_t c = NULL;
	bool InputStreamWasRead = false;
	
	while (token.type == TokenType::Invalid && 
		(InputStreamWasRead = ConsumeFromInputStream(c)) )
	{
		if (c == '\r') continue;

		switch (m_Mode) {
			case LexMode::Default:
				DefaultLexingPath(token, c, origin, len, sliteral_terminator);
				break;

			case LexMode::String_Literal:
				LexStringLiteral(token, c, origin, len, sliteral_terminator, backslash_count);
				break;

			case LexMode::Number_Literal:
				LexNumberLiteral(token, c, origin, len, sliteral_terminator);
				break;
				
			case LexMode::SingleLineComment:
				len++;
				
				if(c == EndOfInputStream || c == '\n') {
					YieldToken(origin, len - 1, token, TokenType::Comment);
					m_Mode = LexMode::Default;
				}
				
				break;
				
			case LexMode::MultiLineComment:
				len++;
				
				//
				// @note: there is no risk of an access violation here, as we
				// require a /* to appear in the blob first, before we run
				// this codepath.
				//
				if(c == EndOfInputStream || c == '/' && m_Blob[m_ReadIdx-2] == '*') {
					YieldToken(origin, len, token, TokenType::Comment);
					m_Mode = LexMode::Default;
				}
				
				break;
		}
	}

	return token;
}

bool Lexer::MatchToken(const std::vector<const char*>& dict, const std::size_t origin, const std::size_t len)
{
	std::string token_string = m_Blob.substr(origin, len);
	for (auto kw: dict) {
		if (token_string.compare(kw) == 0) {
			return true;
		}
	}
	
	return false;
}

void Lexer::YieldToken(const std::size_t origin, const std::size_t len,
	Token& token, TokenType type)
{
	token.idx = origin;
	token.len = len;
	token.type = type;

	bool last_token_was_preproc = m_Tokens.size() ? (m_Tokens.back().type == TokenType::Preprocessor) : false; 
	std::string token_str = m_Blob.substr(origin, len);
	if(token_str.compare("once") == 0 && last_token_was_preproc) {
		token.type = TokenType::Preprocessor;
	}
	
	if(token.type == TokenType::Invalid) {
		bool is_keyword = MatchToken(Language_Keywords, origin, len);
		bool is_datatype = MatchToken(DataType_Keywords, origin, len);
		bool is_preproc = MatchToken(Preprocessor_Keywords, origin, len);

		if(is_keyword) 			token.type = TokenType::Keyword;
		else if(is_datatype) 	token.type = TokenType::Datatype;
		else if(is_preproc) 	token.type = TokenType::Preprocessor;
		else 					token.type = TokenType::Identifier;
	}
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

bool Lexer::IsNumericSymbol(const std::uint8_t c) {
	return std::isdigit(c) || std::isxdigit(c) || c == '.' || c == 'e' || c == '-' || c == 'x';
}

bool Lexer::IsMathSymbol(const std::uint8_t c) {
	return c == '+' || c == '-' || c == '*' || c == '/' || c == '%' 
		|| c == '=' || c == '!' || c == '<' || c == '>' || c == '~'
		|| c == '&' || c == '|';
}