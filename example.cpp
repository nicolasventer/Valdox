#include "valdox.hpp"
#include <iostream>

int main()
{
	Validator v;

	auto ageValidator = v.number.between(1, 100);
	int age = 25;

	// basic validation
	if (ageValidator.validate(age)) std::cout << "Age is valid" << std::endl;
	else
		std::cout << "Age is invalid" << std::endl;

	auto nameValidator = v.string.length.max(3);
	std::string name = "John";

	// validation with errors
	std::vector<std::string> errors;
	if (nameValidator.validate(name, "name", errors)) std::cout << "Name is valid" << std::endl;
	else
		std::cout << "Name is invalid" << std::endl;

	for (const auto& error : errors) std::cout << error << std::endl;

	// special functions
	int validAge = ageValidator.clamp(age);
	std::cout << "Valid age: " << validAge << std::endl;
	std::string validName = nameValidator.crop(name);
	std::cout << "Valid name: " << validName << std::endl;

	return 0;
}
