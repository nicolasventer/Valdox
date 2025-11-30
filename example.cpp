#include "valdox.hpp"
#include <iostream>

int main()
{
	Validator v;

	auto between = v.number.between(5, 10);
	if (between.validate(7))
	{
		// Valid
	}
	else
	{
		// Invalid
	}

	auto l = v.string.literals({"apple", "banana", "cherry"});
	std::vector<std::string> errors;
	if (l.validate("orange", "fruit", errors))
	{
		// Valid
	}
	else
	{
		// Invalid
		for (const auto& err : errors)
		{
			// Handle errors
		}
	}

	auto r = v.string.regex("^[0-9]+$");

	if (r.validate("1234567890", "number", errors))
	{
		// Valid
		std::cout << "Valid" << std::endl;
	}
	else
	{
		// Invalid
		std::cout << "Invalid" << std::endl;
	}

	for (const auto& err : errors)
	{
		std::cout << err << std::endl;
	}
	return 0;
}
