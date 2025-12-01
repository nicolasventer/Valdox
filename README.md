# Valdox

## Description

Valdox is a **header-only C++ library** that provides a simple and intuitive API for **data validation**.  
It supports validation for both numeric values and strings, with comprehensive error reporting and utility functions for value correction.

## Features

### Number Validation
- **Range validation**: `between(min, max, includeMin, includeMax)` - validates numbers within a range (inclusive/exclusive bounds)
- **Comparison validators**: `greaterThan(min)`, `greaterOrEqual(min)`, `lessThan(max)`, `lessOrEqual(max)`
- **Multiple of**: `multipleOf(divisor)` - validates if a number is a multiple of another
- **Literal matching**: `literals({...})` - validates against a list of allowed values
- **Value correction**: `clamp(value)` - clamps numbers to valid ranges

### String Validation
- **Length validation**: `length.between(min, max)`, `length.min(min)`, `length.max(max)`
- **Pattern matching**: `startsWith(prefix)`, `endsWith(suffix)`, `includes(substring)`, `regex(pattern)` with capture group extraction
- **Literal matching**: `literals({...})` - validates against a list of allowed strings
- **Format validation**: `email()`, `uuid()`, `url(protocol, secure)`, `dateTime().global(offset)` / `dateTime().local()`, `date()`, `time()`, `ip(version, withPrefixLength)`, `mac(separator)`
- **Value correction**: `crop(value)` - truncates strings to maximum length

### Error Reporting
- Simple boolean validation: `validate(value)`
- Detailed error messages: `validate(value, varName, errors)` - collects descriptive error messages

## Installation

### Header Only

Include the [`valdox.hpp`](valdox.hpp) file anywhere you want to use it. Optionally, you can use the namespace by defining `VALDOX_USE_NAMESPACE`:

```cpp
// #define VALDOX_USE_NAMESPACE // optional, to use the namespace valdox
#include "valdox.hpp"
```

### Requirements

- C++11 or later required for compilation
- No external dependencies (uses only standard library)

## Example

_Content of [example.cpp](example.cpp):_

```cpp
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
```

Output:

```
Age is valid
Name is invalid
ValidationError: 'name' received "John", expected length <= 3.
Valid age: 25
Valid name: Joh
```

## Usage

### Number Validation

```cpp
Validator v;

// Range validation (inclusive by default)
auto ageValidator = v.number.between(18, 65);
// Exclusive bounds: between(18, 65, false, false) - excludes both endpoints
auto exclusiveValidator = v.number.between(18, 65, false, false);
int age = 25;
if (ageValidator.validate(age)) { /* valid */ }

// Comparison validators
auto positiveValidator = v.number.greaterThan(0);
auto nonNegativeValidator = v.number.greaterOrEqual(0);
auto smallNumberValidator = v.number.lessThan(100);
auto maxNumberValidator = v.number.lessOrEqual(100);

// Multiple of
auto evenValidator = v.number.multipleOf(2);

// Literal matching
auto statusCodeValidator = v.number.literals({200, 404, 500});

// Value correction
int clamped = ageValidator.clamp(150); // returns 65
int clampedLow = ageValidator.clamp(10); // returns 18
```

### String Validation

```cpp
Validator v;

// Length validation
auto shortNameValidator = v.string.length.max(50);
auto longPasswordValidator = v.string.length.min(8);
auto usernameValidator = v.string.length.between(3, 20);

// Format validation
auto emailValidator = v.string.email();
auto uuidValidator = v.string.uuid();

// URL validation with protocol and security options
auto urlValidator = v.string.url(); // All protocols (http, https, ws, wss)
auto httpsValidator = v.string.url(EUrlProtocolFlag::Http, EUrlSecureFlag::Secure);
auto wsValidator = v.string.url(EUrlProtocolFlag::Ws, EUrlSecureFlag::AllSecureFlags);

// Date/Time validation
auto dateTimeGlobal = v.string.dateTime().global(EDateTimeOffset::Optional); // With optional timezone
auto dateTimeLocal = v.string.dateTime().local(); // Local date-time
auto dateValidator = v.string.date();
auto timeValidator = v.string.time();

// IP validation
auto ipv4Validator = v.string.ip(EIpVersion::Ipv4, false);
auto ipv4WithPrefix = v.string.ip(EIpVersion::Ipv4, true); // Includes prefix length (e.g., /24)
auto ipv6Validator = v.string.ip(EIpVersion::Ipv6, false);

// MAC address validation
auto macColon = v.string.mac(":"); // Default separator
auto macHyphen = v.string.mac("-");
auto macNoSep = v.string.mac(""); // No separator

// Custom patterns
auto prefixValidator = v.string.startsWith("https://");
auto suffixValidator = v.string.endsWith(".com");
auto containsValidator = v.string.includes("example");

// Regex validation with capture group extraction
auto regexValidator = v.string.regex("^([A-Z][a-z]+)$");
std::vector<std::string> matches;
if (regexValidator.match("Hello", matches)) {
    // matches[0] contains the first capture group
}

// Literal matching
auto colorValidator = v.string.literals({"red", "green", "blue"});

// Value correction
std::string cropped = shortNameValidator.crop("Very long name"); // returns "Very long na"
```

### Error Reporting

```cpp
Validator v;
auto validator = v.number.between(1, 100);

// Simple validation
if (validator.validate(50)) { /* valid */ }

// Validation with error messages
std::vector<std::string> errors;
if (!validator.validate(150, "age", errors)) {
    // errors now contains: "ValidationError: 'age' received 150, expected 1 <= {value} <= 100."
    for (const auto& error : errors) {
        std::cout << error << std::endl;
    }
}
```

### Regex Match Extraction

The `StringRegexValidator` provides a `match()` method to extract capture groups from regex patterns:

```cpp
Validator v;
auto emailRegex = v.string.regex("^([a-z]+)@([a-z]+)\\.com$");

std::vector<std::string> matches;
if (emailRegex.match("test@example.com", matches)) {
    // matches[0] = "test" (first capture group)
    // matches[1] = "example" (second capture group)
}

// With error reporting
std::vector<std::string> errors;
if (!emailRegex.match("invalid", "email", matches, errors)) {
    // errors contains validation error message
    // matches remains empty
}
```

### Custom Regex Implementation

You can override the default regex matching function by reassigning `stringRegexMatchFn`:

```cpp
// Custom regex implementation (e.g., using a different regex library)
stringRegexMatchFn = [](const std::string& regex, const std::string& value, std::vector<std::string>& matches) {
    // Your custom implementation
    // Return true if match, false otherwise
    // Populate matches with capture groups
    return true;
};
```

## License

MIT License. See [LICENSE file](LICENSE).

Copyright (c) Nicolas VENTER All rights reserved.
