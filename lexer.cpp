#include <lexer.hpp>
#include <iostream>

// @feature: add support for preprocessor directives 
// @feature: add support for single-line & multi-line comments.
// @fix: '\\'

const char* Preprocessor_Keywords[] = {
	"#define", "#elif", "#else",
	"#endif", "#error", "#if",
	"#ifdef", "#ifndef", "#import",
	"#include", "#line", "#pragma",
	"#undef", "#using"
};

const char* Language_Keywords[] = {
	"alignas", "alignof", "and", "and_eq", "asm",
	"atomic_cancel", "atomic_commit", "atomic_noexcept", "auto",
	"bitand", "bitor", "bool", "break", "case", "catch", "char",
	"char8_t", "char16_t", "char32_t", "class", "compl", "concept",
	"const", "consteval", "constexpr", "constinit", "const_cast",
	"continue", "co_await", "co_return", "co_yield", "decltype",
	"default", "delete", "do", "double", "dynamic_cast", "else",
	"enum", "explicit", "export", "extern", "false", "float", "for",
	"friend", "goto", "if", "inline", "int", "long", "mutable", "namespace",
	"new", "noexcept", "not", "not_eq", "nullptr", "operator", "or", "or_eq",
	"private", "protected", "public", "reflexpr", "register", "reinterpret_cast",
	"requires", "return", "short", "signed", "sizeof", "static", "static_assert",
	"static_cast", "struct", "switch", "synchronized", "template", "this",
	"thread_local", "throw", "true", "try", "typedef", "typeid", "typename",
	"union", "unsigned", "using", "virtual", "void", "volatile", "wchar_t",
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
	m_ElapsedTime = std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0);
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
	else if (std::ispunct(c) && c != '_') {
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

	auto c_prev = m_Blob[m_ReadIdx - 2];
	if (!lexing_comment && c == EndOfInputStream || 
		c == StringLiteralPrefix && c_prev != '\\') 
	{
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

bool Lexer::MatchKeyword(const std::size_t origin, const std::size_t len) {
	std::string token = m_Blob.substr(origin, len);

	for (auto kw : Language_Keywords) {
		if (token.compare(kw) == 0) {
			return true;
		}
	}

	for (auto kw : Preprocessor_Keywords) {
		if (token.compare(kw) == 0) {
			return true;
		}
	}

	return false; // token didn't match any language/preprocessor keyword.
}

void Lexer::YieldToken(const std::size_t origin, const std::size_t len,
	Token& token, TokenType type) 
{
	token.idx = origin;
	token.len = len;

	if (type == TokenType::Invalid) {
		token.type = MatchKeyword(origin, len) ? TokenType::Keyword : TokenType::Identifier;
	}
	else {
		token.type = type;
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

bool Lexer::IsNumericSymbol(std::uint8_t c) {
	return std::isdigit(c) || c == '.' || c == 'e' || c == '-';
}

bool Lexer::IsOperator(const std::uint8_t c) {
	return c == '+' || c == '-' || c == '*' || c == '/' || c == '%' || c == '=';
}