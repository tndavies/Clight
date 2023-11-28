#include <unordered_map> 
#include <iostream>
#include <fstream>
#include <string>

#include <colour.hpp>
#include <lexer.hpp>

void PrintUsage() {
	std::cout << Palette::get(Colour::Yellow);

	std::cout << "Usage: clight -f <filepath>\n";
	std::cout << "Usage: clight <expr>\n";

	std::cout << Palette::get(Colour::Default);
}

void PrintError(const std::string err_msg) {
	std::cout << Palette::get(Colour::Red);
	std::cout << "Error: " << err_msg << '\n';
	std::cout << Palette::get(Colour::Default);
}

const std::unordered_map<TokenType, Colour> cset =
{
	{TokenType::Identifier,		Colour::Default},
	{TokenType::Operator,		Colour::Yellow},
	{TokenType::Seperator,		Colour::Blue},
	{TokenType::String_Literal, Colour::Green},
	{TokenType::Number_Literal, Colour::Magenta}
};

void Highlight(const std::string& blob) {
	/*size_t end_of_last_token = 0;
	Lexer lexer(blob.c_str());
	while (auto token = lexer.NextToken()) {
		if (end_of_last_token != token.idx) {
			size_t delta = token.idx - end_of_last_token;
			for (size_t k = 0; k < delta; ++k)
				std::cout << ' ';
		}

		end_of_last_token = token.idx + token.len;

		auto col = highlight_map[token.type];
		std::cout << Palette::get(col);
		std::cout << blob.substr(token.idx, token.len);
		std::cout << Palette::get(Colour::Default);
	}

	std::cout << '\n';
	*/
}

int main(int argc, char* argv[]) 
{
	if (argc == 1) {
		PrintError("No arguments were provided\n");
		std::exit(EXIT_FAILURE);
	}

	const std::string initial_arg = argv[1];
	if (argc == 3 && !initial_arg.compare("-f")) {
		// clight -f <filepath>
		auto filepath = argv[2];

		std::ifstream fileobj(filepath);
		if (!fileobj) {
			std::string err_msg = "Failed to open file ";
			err_msg += '\"';
			err_msg += filepath;
			err_msg += "\"";

			PrintError(err_msg);

			std::exit(EXIT_FAILURE);
		}

		fileobj.seekg(0, std::ios::end);
		auto fsize = fileobj.tellg();
		std::string blob(fsize, 0);
		fileobj.seekg(0);
		fileobj.read(&blob[0], fsize); // @fix: what if read fails!
		fileobj.close();

		/*
		if (!success) {
			std::string err_msg = "Failed to load file ";
			err_msg += '\"';
			err_msg += filepath;
			err_msg += "\"";

			PrintError(err_msg);

			std::exit(EXIT_FAILURE);
		}
		*/

		std::cout << blob;
	}
	else if (argc == 2) {
		// clight <expr>
		std::string expr = argv[1];
		Highlight(expr);
	}
	else if (argc == 1 && !initial_arg.compare("-h") || 
			!initial_arg.compare("-?")) 
	{
		PrintUsage();
	}
	else {
		PrintError("Invalid arguments\n");
		PrintUsage();

		std::exit(EXIT_FAILURE);
	}

	return EXIT_SUCCESS;
}