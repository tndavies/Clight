#include <iostream>
#include <vector>
#include <string>

#include <colour.hpp>
#include <tokens.hpp>

int main(int argc, char* argv[]) 
{
	std::string blob = "foo = my_function(abc+3.1415%2.156e30, \"my string\") abc";
	std::cout << blob << "\n" << std::endl;

	Lexer lexer(blob.c_str());

	Token token;
	while (lexer.next(token)) 
	{
		std::cout << blob.substr(token.idx, token.len) << "\n";
		//std::cout << "\t(" << static_cast<int>(token.type) << ")" << std::endl;
	}

	return 0;
}