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

### Object Validation
- **ValidatorBuilder**: Build validators for structs/classes with multiple fields
- **Field validation**: `add(fieldName, fieldPtr, validator)` - validate individual fields
- **Vector validation**: `addVector(fieldName, fieldPtr, validator)` - validate arrays/vectors of values
- **Nested objects**: Support for validating nested structures with dot-notation error paths
- **Error control**: `bStopOnError` parameter to stop validation after first error or collect all errors

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

### Object Validation with ValidatorBuilder

The `ValidatorBuilder` template class allows you to build validators for structs and classes, validating multiple fields at once:

```cpp
struct Person {
    int age;
    std::string name;
    std::string email;
};

Validator v;
ValidatorBuilder<Person> builder;

// Add validators for each field
builder.add("age", &Person::age, v.number.between(0, 120));
builder.add("name", &Person::name, v.string.length.between(1, 50));
builder.add("email", &Person::email, v.string.email());

// Validate an object
Person person{25, "John Doe", "john@example.com"};
if (builder.validate(person)) {
    // All fields are valid
}

// Collect all validation errors
std::vector<std::string> errors;
Person invalidPerson{150, "", "not-an-email"};
if (!builder.validate(invalidPerson, "person", errors, false)) {
    // errors contains all validation errors
    // errors[0]: "ValidationError: 'person.age' received 150, expected 0 <= {value} <= 120."
    // errors[1]: "ValidationError: 'person.name' received \"\", expected length between 1 and 50."
    // errors[2]: "ValidationError: 'person.email' received \"not-an-email\", expected to be a valid email address."
}
```

#### Stop on First Error

You can configure the validator to stop validation after the first error:

```cpp
std::vector<std::string> errors;
if (!builder.validate(invalidPerson, "person", errors, true)) {
    // errors contains only the first validation error
    // errors.size() == 1
}
```

#### Vector Field Validation

Validate arrays/vectors of values:

```cpp
struct Product {
    int id;
    std::vector<int> tags;
    std::vector<std::string> categories;
};

Validator v;
ValidatorBuilder<Product> builder;

builder.add("id", &Product::id, v.number.greaterThan(0));
builder.addVector("tags", &Product::tags, v.number.between(1, 100));
builder.addVector("categories", &Product::categories, v.string.length.min(3));

Product product{1, {1, 2, 3}, {"Electronics", "Gadgets"}};
if (builder.validate(product)) {
    // All fields and vector elements are valid
}

// Invalid product with errors in vector elements
Product invalidProduct{1, {0, 101, 50}, {"El", "Ga"}};
std::vector<std::string> errors;
if (!builder.validate(invalidProduct, "product", errors, false)) {
    // errors contains validation errors for each invalid element
    // errors[0]: "ValidationError: 'product.tags[0]' received 0, expected 1 <= {value} <= 100."
    // errors[1]: "ValidationError: 'product.tags[1]' received 101, expected 1 <= {value} <= 100."
    // errors[2]: "ValidationError: 'product.categories[0]' received \"El\", expected length >= 3."
    // errors[3]: "ValidationError: 'product.categories[1]' received \"Ga\", expected length >= 3."
}
```

#### Nested Object Validation

Validate nested structures by combining multiple `ValidatorBuilder` instances:

```cpp
struct Address {
    std::string street;
    std::string city;
    std::string zipCode;
};

struct Company {
    std::string name;
    Address address;
    Person owner;
    int employeeCount;
};

Validator v;

// Build validator for nested Address
ValidatorBuilder<Address> addressBuilder;
addressBuilder.add("street", &Address::street, v.string.length.min(5));
addressBuilder.add("city", &Address::city, v.string.length.min(3));
addressBuilder.add("zipCode", &Address::zipCode, v.string.regex("^[0-9]{5}(-[0-9]{4})?$"));

// Build validator for nested Person
ValidatorBuilder<Person> personBuilder;
personBuilder.add("age", &Person::age, v.number.between(18, 100));
personBuilder.add("name", &Person::name, v.string.length.between(1, 50));
personBuilder.add("email", &Person::email, v.string.email());

// Build validator for Company with nested validators
ValidatorBuilder<Company> companyBuilder;
companyBuilder.add("name", &Company::name, v.string.length.between(1, 100));
companyBuilder.add("address", &Company::address, addressBuilder);
companyBuilder.add("owner", &Company::owner, personBuilder);
companyBuilder.add("employeeCount", &Company::employeeCount, v.number.greaterOrEqual(0));

// Validate nested structure
Address address{"123 Main Street", "New York", "10001"};
Person owner{35, "John Smith", "john.smith@example.com"};
Company company{"Acme Corp", address, owner, 50};

if (companyBuilder.validate(company)) {
    // All fields including nested objects are valid
}

// Error paths include nested field names
// e.g., "ValidationError: 'company.address.street' received \"123\", expected length >= 5."
// e.g., "ValidationError: 'company.owner.age' received 15, expected 18 <= {value} <= 100."
```

## License

MIT License. See [LICENSE file](LICENSE).

Copyright (c) Nicolas VENTER All rights reserved.
