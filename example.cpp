#include "valdox.hpp"

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
	if (l.validate("orange", errors))
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
	return 0;
}
