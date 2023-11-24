#include <iostream>
#include <vector>
#include <string>

#include <colour.hpp>
#include <tokens.hpp>

int main(int argc, char* argv[]) 
{
	std::string blob = "3.5e23";
	std::cout << blob << "\n" << std::endl;

	Lexer lexer(blob.c_str());

	Token token;
	while (lexer.NextToken(token)) 
	{
		std::cout << blob.substr(token.idx, token.len) << "\n";
		//std::cout << "\t(" << static_cast<int>(token.type) << ")" << std::endl;
	}

	return 0;
}