#include <functional>
#include <regex>
#include <sstream>
#include <string>
#include <type_traits>
#include <vector>

#ifdef VALDOX_USE_NAMESPACE
namespace valdox
{
#endif
	using StringRegexMatchFn
		= std::function<bool(const std::string& regex, const std::string& value, std::vector<std::string>& matches)>;

	// To be overridden by the user if wanted
	static StringRegexMatchFn stringRegexMatchFn
		= [](const std::string& regex, const std::string& value, std::vector<std::string>& matches)
	{
		std::smatch match;
		if (!std::regex_match(value, match, std::regex(regex))) return false;
		for (size_t i = 1; i < match.size(); ++i) matches.push_back(match[i].str());
		return true;
	};

	template <typename T>
	struct is_numeric :
		std::bool_constant<(std::is_integral_v<T> && !std::is_same_v<T, bool> && !std::is_same_v<T, char>
							   && !std::is_same_v<T, signed char> && !std::is_same_v<T, unsigned char>)
						   || std::is_floating_point_v<T>>
	{
	};

	template <typename T> struct NumberBetweenValidator
	{
		NumberBetweenValidator(T min_, T max_) : min(min_), max(max_) {}
		const T min;
		const T max;

		bool validate(T value) { return value >= min && value <= max; }

		bool validate(T value, const std::string& varName, std::vector<std::string>& errors)
		{
			if (validate(value)) return true;
			std::ostringstream errorMessage;
			errorMessage << "ValidationError: '" << varName << "' received " << value << ", expected value between " << min
						 << " and " << max << ".";
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

		bool validate(T value, const std::string& varName, std::vector<std::string>& errors)
		{
			if (validate(value)) return true;
			std::ostringstream errorMessage;
			errorMessage << "ValidationError: '" << varName << "' received " << value << ", expected value greater than " << min
						 << ".";
			errors.push_back(errorMessage.str());
			return false;
		}

		T max(T value) { return (value < min) ? min : value; }
	};

	template <typename T> struct NumberGreaterOrEqualValidator
	{
		NumberGreaterOrEqualValidator(T min_) : min(min_) {}
		const T min;

		bool validate(T value) { return value >= min; }

		bool validate(T value, const std::string& varName, std::vector<std::string>& errors)
		{
			if (validate(value)) return true;
			std::ostringstream errorMessage;
			errorMessage << "ValidationError: '" << varName << "' received " << value << ", expected value >= " << min << ".";
			errors.push_back(errorMessage.str());
			return false;
		}

		T max(T value) { return (value < min) ? min : value; }
	};

	template <typename T> struct NumberLessThanValidator
	{
		NumberLessThanValidator(T max_) : max(max_) {}
		const T max;

		bool validate(T value) { return value < max; }

		bool validate(T value, const std::string& varName, std::vector<std::string>& errors)
		{
			if (validate(value)) return true;
			std::ostringstream errorMessage;
			errorMessage << "ValidationError: '" << varName << "' received " << value << ", expected value less than " << max
						 << ".";
			errors.push_back(errorMessage.str());
			return false;
		}

		T min(T value) { return (value > max) ? max : value; }
	};

	template <typename T> struct NumberLessOrEqualValidator
	{
		NumberLessOrEqualValidator(T max_) : max(max_) {}
		const T max;

		bool validate(T value) { return value <= max; }

		bool validate(T value, const std::string& varName, std::vector<std::string>& errors)
		{
			if (validate(value)) return true;
			std::ostringstream errorMessage;
			errorMessage << "ValidationError: '" << varName << "' received " << value << ", expected value <= " << max << ".";
			errors.push_back(errorMessage.str());
			return false;
		}

		T min(T value) { return (value > max) ? max : value; }
	};

	template <typename T> struct NumberMultipleOfValidator
	{
		NumberMultipleOfValidator(T divisor_) : divisor(divisor_) {}
		const T divisor;

		bool validate(T value) { return value % divisor == 0; }

		bool validate(T value, const std::string& varName, std::vector<std::string>& errors)
		{
			if (validate(value)) return true;
			std::ostringstream errorMessage;
			errorMessage << "ValidationError: '" << varName << "' received " << value << ", expected multiple of " << divisor
						 << ".";
			errors.push_back(errorMessage.str());
			return false;
		}
	};

	template <typename T> struct NumberLiteralValidator
	{
		NumberLiteralValidator(const std::vector<T>& literals_) : literals(literals_) {}
		const std::vector<T> literals;

		bool validate(T value)
		{
			for (const auto& lit : literals)
				if (value == lit) return true;
			return false;
		}

		bool validate(T value, const std::string& varName, std::vector<std::string>& errors)
		{
			if (validate(value)) return true;
			std::ostringstream errorMessage;
			errorMessage << "ValidationError: '" << varName << "' received " << value << ", expected one of [";
			for (size_t i = 0; i < literals.size(); ++i)
			{
				errorMessage << literals[i];
				if (i < literals.size() - 1) errorMessage << ", ";
			}
			errorMessage << "].";
			errors.push_back(errorMessage.str());
			return false;
		}
	};

	struct NumberValidator
	{
		template <typename T, typename = std::enable_if_t<is_numeric<T>::value>> NumberBetweenValidator<T> between(T min, T max)
		{
			return NumberBetweenValidator(min, max);
		}

		template <typename T, typename = std::enable_if_t<is_numeric<T>::value>> NumberGreaterThanValidator<T> greaterThan(T min)
		{
			return NumberGreaterThanValidator(min);
		}

		template <typename T, typename = std::enable_if_t<is_numeric<T>::value>>
		NumberGreaterOrEqualValidator<T> greaterOrEqual(T min)
		{
			return NumberGreaterOrEqualValidator(min);
		}

		template <typename T, typename = std::enable_if_t<is_numeric<T>::value>> NumberLessThanValidator<T> lessThan(T max)
		{
			return NumberLessThanValidator(max);
		}

		template <typename T, typename = std::enable_if_t<is_numeric<T>::value>> NumberLessOrEqualValidator<T> lessOrEqual(T max)
		{
			return NumberLessOrEqualValidator(max);
		}

		template <typename T, typename = std::enable_if_t<is_numeric<T>::value>>
		NumberMultipleOfValidator<T> multipleOf(T divisor)
		{
			return NumberMultipleOfValidator(divisor);
		}

		template <typename T, typename = std::enable_if_t<is_numeric<T>::value>>
		NumberLiteralValidator<T> literals(const std::vector<T>& lits)
		{
			return NumberLiteralValidator(lits);
		}
	};

	struct StringLengthBetweenValidator
	{
		StringLengthBetweenValidator(size_t min_, size_t max_) : min(min_), max(max_) {}
		const size_t min;
		const size_t max;

		bool validate(const std::string& value) { return value.length() >= min && value.length() <= max; }

		bool validate(const std::string& value, const std::string& varName, std::vector<std::string>& errors)
		{
			if (validate(value)) return true;
			std::ostringstream errorMessage;
			errorMessage << "ValidationError: '" << varName << "' received \"" << value << "\", expected length between " << min
						 << " and " << max << ".";
			errors.push_back(errorMessage.str());
			return false;
		}
	};

	struct StringLengthMinValidator
	{
		StringLengthMinValidator(size_t min_) : min(min_) {}
		const size_t min;

		bool validate(const std::string& value) { return value.length() >= min; }

		bool validate(const std::string& value, const std::string& varName, std::vector<std::string>& errors)
		{
			if (validate(value)) return true;
			std::ostringstream errorMessage;
			errorMessage << "ValidationError: '" << varName << "' received \"" << value << "\", expected length >= " << min
						 << ".";
			errors.push_back(errorMessage.str());
			return false;
		}
	};

	struct StringLengthMaxValidator
	{
		StringLengthMaxValidator(size_t max_) : max(max_) {}
		const size_t max;

		bool validate(const std::string& value) { return value.length() <= max; }

		bool validate(const std::string& value, const std::string& varName, std::vector<std::string>& errors)
		{
			if (validate(value)) return true;
			std::ostringstream errorMessage;
			errorMessage << "ValidationError: '" << varName << "' received \"" << value << "\", expected length <= " << max
						 << ".";
			errors.push_back(errorMessage.str());
			return false;
		}

		std::string crop(const std::string& value) { return value.substr(0, max); }
	};

	struct StringLengthValidator
	{
		StringLengthBetweenValidator between(size_t min, size_t max) { return StringLengthBetweenValidator(min, max); }
		StringLengthMinValidator min(size_t min) { return StringLengthMinValidator(min); }
		StringLengthMaxValidator max(size_t max) { return StringLengthMaxValidator(max); }
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

		bool validate(const std::string& value, const std::string& varName, std::vector<std::string>& errors)
		{
			if (validate(value)) return true;
			std::ostringstream errorMessage;
			errorMessage << "ValidationError: '" << varName << "' received \"" << value << "\", expected one of [";
			for (size_t i = 0; i < literals.size(); ++i)
			{
				errorMessage << "\"" << literals[i] << "\"";
				if (i < literals.size() - 1) errorMessage << ", ";
			}
			errorMessage << "].";
			errors.push_back(errorMessage.str());
			return false;
		}
	};

	struct StringStartsWithValidator
	{
		StringStartsWithValidator(const std::string& prefix_) : prefix(prefix_) {}
		const std::string prefix;

		bool validate(const std::string& value)
		{
			return value.length() >= prefix.length() && value.substr(0, prefix.length()) == prefix;
		}

		bool validate(const std::string& value, const std::string& varName, std::vector<std::string>& errors)
		{
			if (validate(value)) return true;
			std::ostringstream errorMessage;
			errorMessage << "ValidationError: '" << varName << "' received \"" << value << "\", expected to start with \""
						 << prefix << "\".";
			errors.push_back(errorMessage.str());
			return false;
		}
	};

	struct StringEndsWithValidator
	{
		StringEndsWithValidator(const std::string& suffix_) : suffix(suffix_) {}
		const std::string suffix;

		bool validate(const std::string& value)
		{
			return value.length() >= suffix.length() && value.substr(value.length() - suffix.length()) == suffix;
		}

		bool validate(const std::string& value, const std::string& varName, std::vector<std::string>& errors)
		{
			if (validate(value)) return true;
			std::ostringstream errorMessage;
			errorMessage << "ValidationError: '" << varName << "' received \"" << value << "\", expected to end with \"" << suffix
						 << "\".";
			errors.push_back(errorMessage.str());
			return false;
		}
	};

	struct StringIncludesValidator
	{
		StringIncludesValidator(const std::string& substring_) : substring(substring_) {}
		const std::string substring;

		bool validate(const std::string& value) { return value.find(substring) != std::string::npos; }

		bool validate(const std::string& value, const std::string& varName, std::vector<std::string>& errors)
		{
			if (validate(value)) return true;
			std::ostringstream errorMessage;
			errorMessage << "ValidationError: '" << varName << "' received \"" << value << "\", expected to include \""
						 << substring << "\".";
			errors.push_back(errorMessage.str());
			return false;
		}
	};

	struct StringRegexValidator
	{
	public:
		StringRegexValidator(const std::string& regex_) : regex(regex_) {}
		const std::string regex;

		bool validate(const std::string& value)
		{
			std::vector<std::string> matches;
			return match(value, matches);
		}

		bool validate(const std::string& value, const std::string& varName, std::vector<std::string>& errors)
		{
			std::vector<std::string> matches;
			if (match(value, matches)) return true;
			std::ostringstream errorMessage;
			errorMessage << "ValidationError: '" << varName << "' received \"" << value << "\", expected to match regex /"
						 << regex << "/.";
			errors.push_back(errorMessage.str());
			return false;
		}

		bool match(const std::string& value, std::vector<std::string>& matches)
		{
			return stringRegexMatchFn(regex, value, matches);
		}

		bool match(const std::string& value,
			const std::string& varName,
			std::vector<std::string>& matches,
			std::vector<std::string>& errors)
		{
			if (match(value, matches)) return true;
			std::ostringstream errorMessage;
			errorMessage << "ValidationError: '" << varName << "' received \"" << value << "\", expected to match regex \""
						 << regex << "\".";
			errors.push_back(errorMessage.str());
			return false;
		}
	};

	struct StringValidator
	{
		StringLengthValidator length;
		StringLiteralValidator literals(const std::vector<std::string>& lits) { return StringLiteralValidator(lits); }
		StringStartsWithValidator startsWith(const std::string& prefix) { return StringStartsWithValidator(prefix); }
		StringEndsWithValidator endsWith(const std::string& suffix) { return StringEndsWithValidator(suffix); }
		StringIncludesValidator includes(const std::string& substring) { return StringIncludesValidator(substring); }
		StringRegexValidator regex(const std::string& regex_) { return StringRegexValidator(regex_); }
	};

	struct Validator
	{
		NumberValidator number;
		StringValidator string;
	};

#ifdef VALDOX_USE_NAMESPACE
}
#endif