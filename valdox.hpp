#include <sstream>
#include <string>
#include <type_traits>
#include <vector>

template <typename T> struct NumberBetweenValidator
{
	NumberBetweenValidator(T min_, T max_) : min(min_), max(max_) {}
	const T min;
	const T max;

	bool validate(T value) { return value >= min && value <= max; }

	bool validate(T value, std::vector<std::string>& errors)
	{
		if (validate(value)) return true;
		std::ostringstream errorMessage;
		errorMessage << "Value " << value << " is not between " << min << " and " << max;
		errors.push_back(errorMessage.str());
		return false;
	}

	T clamp(T value) { return (value < min) ? min : (value > max) ? max : value; }
};

template <typename T> struct NumberGreaterThanValidator
{
	NumberGreaterThanValidator(T min_) : min(min_) {}
	const T min;

	bool validate(T value) { return value > min; }

	bool validate(T value, std::vector<std::string>& errors)
	{
		if (validate(value)) return true;
		std::ostringstream errorMessage;
		errorMessage << "Value " << value << " is not greater than " << min;
		errors.push_back(errorMessage.str());
		return false;
	}

	T max(T value) { return (value < min) ? min : value; }
};

struct NumberValidator
{
	template <typename T> NumberBetweenValidator<T> between(T min, T max)
	{
		static_assert(std::is_arithmetic<T>::value, "T must be a numeric type");
		return NumberBetweenValidator(min, max);
	}

	template <typename T> NumberBetweenValidator<T> max(T min)
	{
		static_assert(std::is_arithmetic<T>::value, "T must be a numeric type");
		return NumberGreaterThanValidator(min);
	}
};

struct StringLiteralValidator
{
	StringLiteralValidator(const std::vector<std::string>& literals_) : literals(literals_) {}
	const std::vector<std::string> literals;

	bool validate(const std::string& value)
	{
		for (const auto& lit : literals)
			if (value == lit) return true;
		return false;
	}

	bool validate(const std::string& value, std::vector<std::string>& errors)
	{
		if (validate(value)) return true;
		std::ostringstream errorMessage;
		errorMessage << "Value \"" << value << "\" is not one of the allowed literals: ";
		for (size_t i = 0; i < literals.size(); ++i)
		{
			errorMessage << "\"" << literals[i] << "\"";
			if (i < literals.size() - 1) errorMessage << ", ";
		}
		errors.push_back(errorMessage.str());
		return false;
	}
};

struct StringValidator
{
	StringLiteralValidator literals(const std::vector<std::string>& lits) { return StringLiteralValidator(lits); }
};

struct Validator
{
	NumberValidator number;
	StringValidator string;
};
