#include <unordered_map> 
#include <iostream>
#include <fstream>
#include <string>
#include <chrono>

#include <colour.hpp>
#include <lexer.hpp>

void PrintUsage() {
	std::cout << Palette::get(Colour::Yellow);
	std::cout << "Usage: clight <filepath>\n";
	std::cout << Palette::get(Colour::Default);
}

void PrintError(const std::string err_msg) {
	std::cout << Palette::get(Colour::Red);
	std::cout << "Error: " << err_msg << '\n';
	std::cout << Palette::get(Colour::Default);
}

const std::unordered_map<TokenType, Colour> ColourMap = {
	{TokenType::Identifier,		Colour::Default},
	{TokenType::Keyword,		Colour::Red},
	{TokenType::Datatype,		Colour::Blue},
	{TokenType::Operator,		Colour::Yellow},
	{TokenType::Seperator,		Colour::Blue},
	{TokenType::Number_Literal, Colour::Magenta},
	{TokenType::String_Literal, Colour::Green},
	{TokenType::Char_Literal,	Colour::Black},
	{TokenType::Preprocessor,	Colour::Magenta},
	{TokenType::Comment,		Colour::Black}
};

void Highlight(const std::string& blob) {
	Lexer lexer(blob.c_str());

	size_t prev_token_end = 0;
	const std::vector<Token>& tokens = lexer.getTokens();

	std::cout << Palette::get(Colour::Red);
	std::cout << "Performance: " << lexer.getElapsedTime() << "\n\n";
	std::cout << Palette::get(Colour::Default);

	for (auto& token : tokens) {
		if (prev_token_end != token.idx) {
			for (size_t k = prev_token_end; k < token.idx; ++k) {
				std::cout << blob[k];
			}
		}

		prev_token_end = token.idx + token.len;

		auto col = ColourMap.at(token.type);
		std::cout << Palette::get(col);
		std::cout << blob.substr(token.idx, token.len);
		std::cout << Palette::get(Colour::Default);
	}
}

int main(int argc, char* argv[]) 
{
	if (argc == 2) {
		const std::string arg = argv[1];
	
		bool usage_wanted = !arg.compare("/?") || !arg.compare("/h") ||
			!arg.compare("-?") || !arg.compare("-h");
		if (usage_wanted) {
			PrintUsage();
			std::exit(EXIT_SUCCESS);
		}

		std::ifstream fileobj(arg);
		if (!fileobj.is_open()) {
			std::string err_msg = "Failed to open file ";
			err_msg += '\"';
			err_msg += arg;
			err_msg += "\"";

			PrintError(err_msg);
			std::exit(EXIT_FAILURE);
		}

		fileobj.seekg(0, std::ios::end);
		auto fsize = fileobj.tellg();
		fileobj.seekg(0);

		std::string blob(fsize, 0);
		fileobj.read(&blob[0], fsize);
		fileobj.close();

		Highlight(blob);
	} 
	else {
		PrintError("Invalid argument");
		PrintUsage();

		std::exit(EXIT_FAILURE);
	}

	
	return EXIT_SUCCESS;
}