#include <lexer.hpp>

const std::vector<const char*> Preprocessor_Keywords = {
	"#define", "#elif", "#else",
	"#endif", "#error", "#if",
	"#ifdef", "#ifndef", "#import",
	"#include", "#line", "#pragma",
	"#undef", "#using"
};

const std::vector<const char*> DataType_Keywords = {
	"bool", "char", "size_t",
	"uint8_t", "uint16_t", "uint32_t",
	"int8_t", "int16_t", "int32_t",
	"double", "float", "enum", 
	"int", "long", "short", "signed",
	"unsigned", "signed", "void",
	"wchar_t"
};

const std::vector<const char*> Language_Keywords = {
	"auto", "alignas", "alignof", "and", "and_eq", "asm",
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

void Highlighter::Parse() {
	Token token;
	while (token = NextToken()) {
		m_Tokens.push_back(token);
	}
}

Token Highlighter::NextToken() {
	Token token = { TokenType::Invalid , m_ReadIdx, 0 };

	std::size_t backslash_count = 0;
	std::uint8_t string_terminator = NULL;
	
	std::uint8_t c = NULL;
	bool InputStreamWasRead = false;

	while (token.type == TokenType::Invalid && 
		(InputStreamWasRead = ConsumeFromInputStream(c)) )
	{
		if (c == '\r') continue;

		switch (m_Mode) {
			case LexMode::Default:
				DefaultMode(token, c, string_terminator);
				break;

			case LexMode::String_Literal:
				StringLiteralMode(token, c, string_terminator, backslash_count);
				break;

			case LexMode::Number_Literal:
				NumberLiteralMode(token, c, string_terminator);
				break;
				
			case LexMode::SingleLineComment:
				SingleCommentMode(token, c);
				break;
				
			case LexMode::MultiLineComment:
				MultiCommentMode(token, c);
				break;
		}
	}

	return token;
}

void Highlighter::DefaultMode(Token& token, const std::uint8_t c, std::uint8_t& string_terminator) {
	bool last_token_was_preproc = m_Tokens.size() ? (m_Tokens.back().type == TokenType::Preprocessor) : false;

	if (std::isspace(c)) {
		if (token.len) InferTokenType(token);
		else token = NextToken();
	}
	// Note: EndOfInputStream prevents possible read access violation here.
	else if (c == '/' && m_Blob[m_ReadIdx] == '/') {
		m_Mode = LexMode::SingleLineComment;
		token.len++;
	}
	// Note: EndOfInputStream prevents possible read access violation here.
	else if (c == '/' && m_Blob[m_ReadIdx] == '*') {
		m_Mode = LexMode::MultiLineComment;
		token.len++;
	}
	else if (c == '\'' || c == '\"' || (last_token_was_preproc && c == '<')) {
		if (token.len) {
			InferTokenType(token);
			RewindInputStream();
		}
		else {
			m_Mode = LexMode::String_Literal;
			string_terminator = (c == '<') ? '>' : c;
			token.len++;
		}
	}
	else if (std::ispunct(c) && c != '_' && c != '#') {
		if (token.len) {
			InferTokenType(token);
			RewindInputStream();
		}
		else {
			auto type = IsMathSymbol(c) ? TokenType::Operator : TokenType::Seperator;
			InferTokenType(token, type);
		}
	}
	else if (std::isdigit(c) && !token.len) {
		m_Mode = LexMode::Number_Literal;
		token.len++;
	}
	else if (c == EndOfInputStream && token.len) {
		InferTokenType(token);
	}
	else {
		token.len++;
	}
}

void Highlighter::StringLiteralMode(Token& token, const std::uint8_t c, std::uint8_t& string_terminator, std::size_t& backslash_count) {
	token.len++;

	if (c == EndOfInputStream) {
		token.len--;
		
		auto type = (string_terminator == '\'') ? TokenType::Char_Literal : TokenType::String_Literal;
		InferTokenType(token, type);
		
		m_Mode = LexMode::Default;
		
		return;
	}
	else if (c == string_terminator) {
		bool escaped = static_cast<bool>(backslash_count % 2);
		backslash_count = 0;

		if (!escaped) {
			auto type = (string_terminator == '\'') ? TokenType::Char_Literal : TokenType::String_Literal;
			InferTokenType(token, type);
			m_Mode = LexMode::Default;
		}
	}
	else if (c == '\\') {
		backslash_count++;
	}
	else {
		backslash_count = 0;
	}
}

void Highlighter::NumberLiteralMode(Token& token, const std::uint8_t c, std::uint8_t& string_terminator) {
	token.len++;

	if (!IsNumericSymbol(c)) {
		token.len--;
		InferTokenType(token, TokenType::Number_Literal);

		m_Mode = LexMode::Default;

		RewindInputStream();
	}
}

void Highlighter::SingleCommentMode(Token& token, const std::uint8_t c) {
	token.len++;

	if (c == EndOfInputStream || c == '\n') {
		token.len--;

		InferTokenType(token, TokenType::Comment);
		
		m_Mode = LexMode::Default;
	}
}

void Highlighter::MultiCommentMode(Token& token, const std::uint8_t c) {
	token.len++;

	// Note: the index into the blob is guarenteed to be safe here as this path
	// only runs once we detect a '/*'. 
	if (c == EndOfInputStream || c == '/' && m_Blob[m_ReadIdx - 2] == '*') {
		InferTokenType(token, TokenType::Comment);
		m_Mode = LexMode::Default;
	}
}

void Highlighter::InferTokenType(Token& token, TokenType type)
{
	auto origin = token.origin;
	auto len = token.len;
	token.type = type;
	
	if(type == TokenType::Invalid) {
		bool last_token_was_preproc = m_Tokens.size() ? (m_Tokens.back().type == TokenType::Preprocessor) : false;
		bool token_is_once = (m_Blob.substr(origin, len).compare("once") == 0);

		// Note: Special case to highlight 'once' correctly in preprocessor directives.
		if (token_is_once && last_token_was_preproc) {
			token.type = TokenType::Preprocessor;
		}
		else {
			std::string token_string = m_Blob.substr(origin, len);

			bool is_keyword = AppearsIn(Language_Keywords, token_string);
			bool is_datatype = AppearsIn(DataType_Keywords, token_string);
			bool is_preproc = AppearsIn(Preprocessor_Keywords, token_string);

			if (is_keyword) 			token.type = TokenType::Keyword;
			else if (is_datatype) 	token.type = TokenType::Datatype;
			else if (is_preproc) 	token.type = TokenType::Preprocessor;
			else 					token.type = TokenType::Identifier;
		}
	}
}

bool Highlighter::ConsumeFromInputStream(std::uint8_t& c) {
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

bool Highlighter::AppearsIn(const std::vector<const char*>& dict, const std::string& string)
{
	for (auto kw : dict) {
		if (string.compare(kw) == 0) {
			return true;
		}
	}

	return false;
}

bool Highlighter::IsMathSymbol(const std::uint8_t c) {
	return c == '+' || c == '-' || c == '*' || c == '/' || c == '%'
		|| c == '=' || c == '!' || c == '<' || c == '>' || c == '~'
		|| c == '&' || c == '|';
}

bool Highlighter::IsNumericSymbol(const std::uint8_t c) {
	return std::isdigit(c) || std::isxdigit(c) || c == '.' || c == 'e' || c == '-' || c == 'x';
}