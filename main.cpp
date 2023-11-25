#include <iostream>
#include <vector>
#include <string>

#include <colour.hpp>
#include <tokens.hpp>

int main(int argc, char* argv[]) 
{
	std::string blob = "hello \"my.string\" world";
	std::cout << blob << "\n" << std::endl;

	Lexer lexer(blob.c_str());

	while(auto token = lexer.NextToken())
	{
		std::cout << blob.substr(token.idx, token.len) << "\n";
		//std::cout << "\t(" << static_cast<int>(token.type) << ")" << std::endl;
	}

	return 0;
}