#include <iostream>
#include <vector>
#include <string>

#include <colour.hpp>
#include <tokens.hpp>

int main(int argc, char* argv[]) {

	// Our blob of text to highlight.
	const char* blob = "Lorem Ipsum is simply dummy text of the printing and typesetting industry.";

	// Tokenize the blob of text, according to our ruleset.
	Tokenizer tokenizer;
	tokenizer.parse(blob);

	// Assign colours to tokens.
	for (auto& token : tokenizer) {
		Tokenizer::assign(token);
	}

	// Print out tokens in their assigned colours.
	Palette::Init();

	for(const auto& token: tokenizer) {
		std::string token_str(blob + token.idx, token.len);

		std::cout << "\033" << Palette::get(token.col);
		std::cout << token_str;
		std::cout << "\033" << Palette::get(Colour::Default);
	}

	return 0;
}