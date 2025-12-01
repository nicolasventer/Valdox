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
		NumberBetweenValidator(T min_, T max_, bool includeMin_, bool includeMax_) :
			min(min_), max(max_), includeMin(includeMin_), includeMax(includeMax_)
		{
		}
		const T min;
		const T max;
		const bool includeMin;
		const bool includeMax;

		bool validate(T value) const
		{
			return (includeMin ? value >= min : value > min) && (includeMax ? value <= max : value < max);
		}

		bool validate(T value, const std::string& varName, std::vector<std::string>& errors) const
		{
			if (validate(value)) return true;
			std::ostringstream errorMessage;
			errorMessage << "ValidationError: '" << varName << "' received " << value << ", expected " << min
						 << (includeMin ? " <= " : " < ") << "{value}" << (includeMax ? " <= " : " < ") << max << ".";
			errors.push_back(errorMessage.str());
			return false;
		}

		T clamp(T value) const
		{
			return (value < min) ? includeMin ? min : min + 1 : (value > max) ? includeMax ? max : max - 1 : value;
		}
	};

	template <typename T> struct NumberGreaterThanValidator
	{
		NumberGreaterThanValidator(T min_) : min(min_) {}
		const T min;

		bool validate(T value) const { return value > min; }

		bool validate(T value, const std::string& varName, std::vector<std::string>& errors) const
		{
			if (validate(value)) return true;
			std::ostringstream errorMessage;
			errorMessage << "ValidationError: '" << varName << "' received " << value << ", expected value greater than " << min
						 << ".";
			errors.push_back(errorMessage.str());
			return false;
		}

		T clamp(T value) const { return (value < min) ? min + 1 : value; }
	};

	template <typename T> struct NumberGreaterOrEqualValidator
	{
		NumberGreaterOrEqualValidator(T min_) : min(min_) {}
		const T min;

		bool validate(T value) const { return value >= min; }

		bool validate(T value, const std::string& varName, std::vector<std::string>& errors) const
		{
			if (validate(value)) return true;
			std::ostringstream errorMessage;
			errorMessage << "ValidationError: '" << varName << "' received " << value << ", expected value >= " << min << ".";
			errors.push_back(errorMessage.str());
			return false;
		}

		T clamp(T value) const { return (value < min) ? min : value; }
	};

	template <typename T> struct NumberLessThanValidator
	{
		NumberLessThanValidator(T max_) : max(max_) {}
		const T max;

		bool validate(T value) const { return value < max; }

		bool validate(T value, const std::string& varName, std::vector<std::string>& errors) const
		{
			if (validate(value)) return true;
			std::ostringstream errorMessage;
			errorMessage << "ValidationError: '" << varName << "' received " << value << ", expected value less than " << max
						 << ".";
			errors.push_back(errorMessage.str());
			return false;
		}

		T clamp(T value) const { return (value > max) ? max - 1 : value; }
	};

	template <typename T> struct NumberLessOrEqualValidator
	{
		NumberLessOrEqualValidator(T max_) : max(max_) {}
		const T max;

		bool validate(T value) const { return value <= max; }

		bool validate(T value, const std::string& varName, std::vector<std::string>& errors) const
		{
			if (validate(value)) return true;
			std::ostringstream errorMessage;
			errorMessage << "ValidationError: '" << varName << "' received " << value << ", expected value <= " << max << ".";
			errors.push_back(errorMessage.str());
			return false;
		}

		T clamp(T value) const { return (value > max) ? max : value; }
	};

	template <typename T> struct NumberMultipleOfValidator
	{
		NumberMultipleOfValidator(T divisor_) : divisor(divisor_) {}
		const T divisor;

		bool validate(T value) const { return value % divisor == 0; }

		bool validate(T value, const std::string& varName, std::vector<std::string>& errors) const
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

		bool validate(T value) const
		{
			for (const auto& lit : literals)
				if (value == lit) return true;
			return false;
		}

		bool validate(T value, const std::string& varName, std::vector<std::string>& errors) const
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
		template <typename T, typename = std::enable_if_t<is_numeric<T>::value>>
		NumberBetweenValidator<T> between(T min, T max, bool includeMin = true, bool includeMax = true) const
		{
			return NumberBetweenValidator<T>(min, max, includeMin, includeMax);
		}

		template <typename T, typename = std::enable_if_t<is_numeric<T>::value>>
		NumberGreaterThanValidator<T> greaterThan(T min) const
		{
			return NumberGreaterThanValidator<T>(min);
		}

		template <typename T, typename = std::enable_if_t<is_numeric<T>::value>>
		NumberGreaterOrEqualValidator<T> greaterOrEqual(T min) const
		{
			return NumberGreaterOrEqualValidator<T>(min);
		}

		template <typename T, typename = std::enable_if_t<is_numeric<T>::value>> NumberGreaterOrEqualValidator<T> min(T min) const
		{
			return NumberGreaterOrEqualValidator(min);
		}

		template <typename T, typename = std::enable_if_t<is_numeric<T>::value>> NumberLessThanValidator<T> lessThan(T max) const
		{
			return NumberLessThanValidator<T>(max);
		}

		template <typename T, typename = std::enable_if_t<is_numeric<T>::value>>
		NumberLessOrEqualValidator<T> lessOrEqual(T max) const
		{
			return NumberLessOrEqualValidator<T>(max);
		}

		template <typename T, typename = std::enable_if_t<is_numeric<T>::value>>
		NumberMultipleOfValidator<T> multipleOf(T divisor) const
		{
			return NumberMultipleOfValidator<T>(divisor);
		}

		template <typename T, typename = std::enable_if_t<is_numeric<T>::value>>
		NumberLiteralValidator<T> literals(const std::vector<T>& lits) const
		{
			return NumberLiteralValidator<T>(lits);
		}
	};

	struct StringLengthBetweenValidator
	{
		StringLengthBetweenValidator(size_t min_, size_t max_) : min(min_), max(max_) {}
		const size_t min;
		const size_t max;

		bool validate(const std::string& value) const { return value.length() >= min && value.length() <= max; }

		bool validate(const std::string& value, const std::string& varName, std::vector<std::string>& errors) const
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

		bool validate(const std::string& value) const { return value.length() >= min; }

		bool validate(const std::string& value, const std::string& varName, std::vector<std::string>& errors) const
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

		bool validate(const std::string& value) const { return value.length() <= max; }

		bool validate(const std::string& value, const std::string& varName, std::vector<std::string>& errors) const
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
		// inclusive
		StringLengthBetweenValidator between(size_t min, size_t max) const { return StringLengthBetweenValidator(min, max); }
		// inclusive
		StringLengthMinValidator min(size_t min) const { return StringLengthMinValidator(min); }
		// inclusive
		StringLengthMaxValidator max(size_t max) const { return StringLengthMaxValidator(max); }
	};

	struct StringLiteralValidator
	{
		StringLiteralValidator(const std::vector<std::string>& literals_) : literals(literals_) {}
		const std::vector<std::string> literals;

		bool validate(const std::string& value) const
		{
			for (const auto& lit : literals)
				if (value == lit) return true;
			return false;
		}

		bool validate(const std::string& value, const std::string& varName, std::vector<std::string>& errors) const
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

		bool validate(const std::string& value) const
		{
			return value.length() >= prefix.length() && value.substr(0, prefix.length()) == prefix;
		}

		bool validate(const std::string& value, const std::string& varName, std::vector<std::string>& errors) const
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

		bool validate(const std::string& value) const
		{
			return value.length() >= suffix.length() && value.substr(value.length() - suffix.length()) == suffix;
		}

		bool validate(const std::string& value, const std::string& varName, std::vector<std::string>& errors) const
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

		bool validate(const std::string& value) const { return value.find(substring) != std::string::npos; }

		bool validate(const std::string& value, const std::string& varName, std::vector<std::string>& errors) const
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

		bool validate(const std::string& value) const
		{
			std::vector<std::string> matches;
			return match(value, matches);
		}

		bool validate(const std::string& value, const std::string& varName, std::vector<std::string>& errors) const
		{
			std::vector<std::string> matches;
			if (match(value, matches)) return true;
			std::ostringstream errorMessage;
			errorMessage << "ValidationError: '" << varName << "' received \"" << value << "\", expected to match regex /"
						 << regex << "/.";
			errors.push_back(errorMessage.str());
			return false;
		}

		bool match(const std::string& value, std::vector<std::string>& matches) const
		{
			return stringRegexMatchFn(regex, value, matches);
		}

		bool match(const std::string& value,
			const std::string& varName,
			std::vector<std::string>& matches,
			std::vector<std::string>& errors) const
		{
			if (match(value, matches)) return true;
			std::ostringstream errorMessage;
			errorMessage << "ValidationError: '" << varName << "' received \"" << value << "\", expected to match regex \""
						 << regex << "\".";
			errors.push_back(errorMessage.str());
			return false;
		}
	};

	struct StringEmailValidator
	{
		static std::string getRegex() { return "^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\\.[a-zA-Z]{2,}$"; }

		StringEmailValidator() : regex(getRegex()) {}
		const std::string regex;

		bool validate(const std::string& value) const { return std::regex_match(value, std::regex(regex)); }

		bool validate(const std::string& value, const std::string& varName, std::vector<std::string>& errors) const
		{
			if (validate(value)) return true;
			std::ostringstream errorMessage;
			errorMessage << "ValidationError: '" << varName << "' received \"" << value
						 << "\", expected to be a valid email address.";
			errors.push_back(errorMessage.str());
			return false;
		}
	};

	struct StringUuidValidator
	{
		static std::string getRegex()
		{
			return "^[0-9a-fA-F]{8}-[0-9a-fA-F]{4}-[1-8][0-9a-fA-F]{3}-[89abAB][0-9a-fA-F]{3}-[0-9a-fA-F]{12}$";
		}

		StringUuidValidator() : regex(getRegex()) {}
		const std::string regex;

		bool validate(const std::string& value) const { return std::regex_match(value, std::regex(regex)); }

		bool validate(const std::string& value, const std::string& varName, std::vector<std::string>& errors) const
		{
			if (validate(value)) return true;
			std::ostringstream errorMessage;
			errorMessage << "ValidationError: '" << varName << "' received \"" << value << "\", expected to be a valid UUID.";
			errors.push_back(errorMessage.str());
			return false;
		}
	};

	enum EUrlProtocolFlag
	{
		Ws = 1 << 0,
		Http = 1 << 1,
		AllProtocols = Ws | Http
	};
	enum EUrlSecureFlag
	{
		NonSecure = 1 << 0,
		Secure = 1 << 1,
		AllSecureFlags = NonSecure | Secure
	};

	struct StringUrlValidator
	{
		static std::string getRegex(EUrlProtocolFlag protocol, EUrlSecureFlag secure)
		{
			std::ostringstream regexOss;
			regexOss << "^((?:";
			if (protocol & EUrlProtocolFlag::Ws) regexOss << "ws";
			regexOss << "|";
			if (protocol & EUrlProtocolFlag::Http) regexOss << "http";
			regexOss << ")";
			if (secure & EUrlSecureFlag::Secure) regexOss << "s";
			if (secure & EUrlSecureFlag::AllSecureFlags) regexOss << "?";
			regexOss << "):\\/\\/([^\\s/$.?#].[^\\s]*)$";
			return regexOss.str();
		}

		StringUrlValidator(EUrlProtocolFlag protocol_, EUrlSecureFlag secure_) :
			protocol(protocol_), secure(secure_), regex(getRegex(protocol_, secure_))
		{
		}

		const EUrlProtocolFlag protocol;
		const EUrlSecureFlag secure;
		const std::string regex;

		bool validate(const std::string& value) const { return std::regex_match(value, std::regex(regex)); }

		bool validate(const std::string& value, const std::string& varName, std::vector<std::string>& errors) const
		{
			if (validate(value)) return true;
			std::ostringstream errorMessage;
			errorMessage << "ValidationError: '" << varName << "' received \"" << value
						 << "\", expected to be a valid URL of protocol '";
			if (protocol & EUrlProtocolFlag::Ws) errorMessage << "ws";
			errorMessage << "|";
			if (protocol & EUrlProtocolFlag::Http) errorMessage << "http";
			errorMessage << ")";
			if (secure & EUrlSecureFlag::Secure) errorMessage << "s";
			if (secure & EUrlSecureFlag::AllSecureFlags) errorMessage << "?";
			errorMessage << "'.";
			errors.push_back(errorMessage.str());
			return false;
		}
	};

	enum class EDateTimeOffset
	{
		None,
		Optional,
		Required,
	};

	struct StringDateTimeGlobalValidator
	{
		static std::string getRegex(EDateTimeOffset offsetOption)
		{
			std::ostringstream regexOss;
			regexOss << "^(\\d{4}-\\d{2}-\\d{2})T(\\d{2}:\\d{2}:\\d{2}(?:\\.\\d+)?)";
			switch (offsetOption)
			{
			case EDateTimeOffset::None:
				regexOss << "Z$";
				break;
			case EDateTimeOffset::Optional:
				regexOss << "([+-]\\d{2}:\\d{2}|Z)?$";
				break;
			case EDateTimeOffset::Required:
				regexOss << "([+-]\\d{2}:\\d{2}|Z)$";
				break;
			}
			return regexOss.str();
		}

		StringDateTimeGlobalValidator(EDateTimeOffset offsetOption_) : offsetOption(offsetOption_), regex(getRegex(offsetOption_))
		{
		}
		const EDateTimeOffset offsetOption;
		const std::string regex;

		bool validate(const std::string& value) const { return std::regex_match(value, std::regex(regex)); }

		bool validate(const std::string& value, const std::string& varName, std::vector<std::string>& errors) const
		{
			if (validate(value)) return true;
			std::ostringstream errorMessage;
			errorMessage << "ValidationError: '" << varName << "' received \"" << value
						 << "\", expected to be a valid global date time.";
			errors.push_back(errorMessage.str());
			return false;
		}
	};

	struct StringDateTimeLocalValidator
	{
		static std::string getRegex()
		{
			return "^(\\d{4}-(?:0[1-9]|1[0-2])-(?:0[1-9]|[12]\\d|3[01]))T((?:[01]\\d|2[0-3]):[0-5]\\d(?::[0-5]\\d)?)$";
		}

		StringDateTimeLocalValidator() : regex(getRegex()) {}
		const std::string regex;

		bool validate(const std::string& value) const { return std::regex_match(value, std::regex(regex)); }

		bool validate(const std::string& value, const std::string& varName, std::vector<std::string>& errors) const
		{
			if (validate(value)) return true;
			std::ostringstream errorMessage;
			errorMessage << "ValidationError: '" << varName << "' received \"" << value
						 << "\", expected to be a valid local date time.";
			errors.push_back(errorMessage.str());
			return false;
		}
	};

	struct StringDateTimeValidator
	{
		StringDateTimeGlobalValidator global(EDateTimeOffset offsetOption = EDateTimeOffset::None) const
		{
			return StringDateTimeGlobalValidator(offsetOption);
		}
		StringDateTimeLocalValidator local() const { return StringDateTimeLocalValidator(); }
	};

	struct StringDateValidator
	{
		static std::string getRegex() { return "^(\\d{4})-(0[1-9]|1[0-2])-(0[1-9]|[12]\\d|3[01])$"; }

		StringDateValidator() : regex(getRegex()) {}
		const std::string regex;

		bool validate(const std::string& value) const { return std::regex_match(value, std::regex(regex)); }

		bool validate(const std::string& value, const std::string& varName, std::vector<std::string>& errors) const
		{
			if (validate(value)) return true;
			std::ostringstream errorMessage;
			errorMessage << "ValidationError: '" << varName << "' received \"" << value << "\", expected to be a valid date.";
			errors.push_back(errorMessage.str());
			return false;
		}
	};

	struct StringTimeValidator
	{
		static std::string getRegex() { return "^([01]\\d|2[0-3]):([0-5]\\d)(?::([0-5]\\d(?:\\.\\d+)?))?$"; }
		StringTimeValidator() : regex(getRegex()) {}
		const std::string regex;

		bool validate(const std::string& value) const { return std::regex_match(value, std::regex(regex)); }

		bool validate(const std::string& value, const std::string& varName, std::vector<std::string>& errors) const
		{
			if (validate(value)) return true;
			std::ostringstream errorMessage;
			errorMessage << "ValidationError: '" << varName << "' received \"" << value << "\", expected to be a valid time.";
			errors.push_back(errorMessage.str());
			return false;
		}
	};

	enum class EIpVersion
	{
		Ipv4,
		Ipv6,
	};

	struct StringIpValidator
	{
		static std::string getRegex(EIpVersion version, bool withPrefixLength)
		{
			std::ostringstream regexOss;
			switch (version)
			{
			case EIpVersion::Ipv4:
				regexOss << "^((?:(?:25[0-5]|2[0-4]\\d|1\\d{2}|[1-9]\\d|\\d)\\.){3}(?:25[0-5]|2[0-4]\\d|1\\d{2}|[1-9]\\d|\\d))";
				if (withPrefixLength) regexOss << "(?:/([0-9]|[12][0-9]|3[0-2]))";
				regexOss << "$";
				break;
			case EIpVersion::Ipv6:
				regexOss << "^((?:[0-9a-fA-F]{1,4}:){7}[0-9a-fA-F]{1,4}|((?:[0-9a-fA-F]{1,4}:){0,7}[0-9a-fA-F]{1,4})?::((?:[0-9a-"
							"fA-F]{1,4}:){0,7}[0-9a-fA-F]{1,4})?|::)";
				if (withPrefixLength) regexOss << "(?:/([0-9]|[1-9][0-9]|1[01][0-9]|12[0-8]))";
				regexOss << "$";
				break;
			}
			return regexOss.str();
		}

		StringIpValidator(EIpVersion version_, bool withPrefixLength_) :
			version(version_), withPrefixLength(withPrefixLength_), regex(getRegex(version_, withPrefixLength_))
		{
		}
		const EIpVersion version;
		const bool withPrefixLength;
		const std::string regex;

		bool validate(const std::string& value) const { return std::regex_match(value, std::regex(regex)); }

		bool validate(const std::string& value, const std::string& varName, std::vector<std::string>& errors) const
		{
			if (validate(value)) return true;
			std::ostringstream errorMessage;
			errorMessage << "ValidationError: '" << varName << "' received \"" << value << "\", expected to be a valid IP"
						 << (version == EIpVersion::Ipv4 ? "v4" : "v6") << " address"
						 << (withPrefixLength ? " with prefix length" : "") << ".";
			errors.push_back(errorMessage.str());
			return false;
		}
	};

	struct StringMacValidator
	{
		static std::string getRegex(const std::string& separator)
		{
			return "^([0-9A-Fa-f]{2}" + separator + "){5}([0-9A-Fa-f]{2})$";
		}

		StringMacValidator(const std::string& separator_) : separator(separator_), regex(getRegex(separator_)) {}
		const std::string separator;
		const std::string regex;

		bool validate(const std::string& value) const { return std::regex_match(value, std::regex(regex)); }

		bool validate(const std::string& value, const std::string& varName, std::vector<std::string>& errors) const
		{
			if (validate(value)) return true;
			std::ostringstream errorMessage;
			errorMessage << "ValidationError: '" << varName << "' received \"" << value
						 << "\", expected to be a valid MAC address with separator \"" << separator << "\".";
			errors.push_back(errorMessage.str());
			return false;
		}
	};

	struct StringValidator
	{
		StringLengthValidator length;
		StringLiteralValidator literals(const std::vector<std::string>& lits) const { return StringLiteralValidator(lits); }
		StringStartsWithValidator startsWith(const std::string& prefix) const { return StringStartsWithValidator(prefix); }
		StringEndsWithValidator endsWith(const std::string& suffix) const { return StringEndsWithValidator(suffix); }
		StringIncludesValidator includes(const std::string& substring) const { return StringIncludesValidator(substring); }
		StringRegexValidator regex(const std::string& regex) const { return StringRegexValidator(regex); }
		StringEmailValidator email() const { return StringEmailValidator(); }
		StringUuidValidator uuid() const { return StringUuidValidator(); }
		StringUrlValidator url(EUrlProtocolFlag protocol = EUrlProtocolFlag::AllProtocols,
			EUrlSecureFlag secure = EUrlSecureFlag::AllSecureFlags) const
		{
			return StringUrlValidator(protocol, secure);
		}
		StringDateTimeValidator dateTime() const { return StringDateTimeValidator(); }
		StringDateValidator date() const { return StringDateValidator(); }
		StringTimeValidator time() const { return StringTimeValidator(); }
		StringIpValidator ip(EIpVersion version, bool withPrefixLength = false) const
		{
			return StringIpValidator(version, withPrefixLength);
		}
		StringMacValidator mac(const std::string& separator = ":") const { return StringMacValidator(separator); }
	};

	struct Validator
	{
		NumberValidator number;
		StringValidator string;
	};

#ifdef VALDOX_USE_NAMESPACE
}
#endif