#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "../valdox.hpp"
#include "doctest.h"
#include <string>
#include <vector>

// Number Validator Tests
TEST_CASE("NumberBetweenValidator")
{
	Validator v;
	auto validator = v.number.between(5, 10);

	CHECK(validator.validate(5));
	CHECK(validator.validate(7));
	CHECK(validator.validate(10));
	CHECK_FALSE(validator.validate(4));
	CHECK_FALSE(validator.validate(11));

	// Test with errors
	std::vector<std::string> errors;
	CHECK(validator.validate(7, "testVar", errors));
	CHECK(errors.empty());

	errors.clear();
	CHECK_FALSE(validator.validate(15, "testVar", errors));
	CHECK_FALSE(errors.empty());
	CHECK(errors[0].find("testVar") != std::string::npos);

	// Test clamp
	CHECK(validator.clamp(3) == 5);
	CHECK(validator.clamp(7) == 7);
	CHECK(validator.clamp(15) == 10);
}

TEST_CASE("NumberGreaterThanValidator")
{
	Validator v;
	auto validator = v.number.greaterThan(5);

	CHECK(validator.validate(6));
	CHECK(validator.validate(10));
	CHECK_FALSE(validator.validate(5));
	CHECK_FALSE(validator.validate(4));

	std::vector<std::string> errors;
	CHECK_FALSE(validator.validate(3, "testVar", errors));
	CHECK_FALSE(errors.empty());

	CHECK(validator.max(3) == 5);
	CHECK(validator.max(7) == 7);
}

TEST_CASE("NumberGreaterOrEqualValidator")
{
	Validator v;
	auto validator = v.number.greaterOrEqual(5);

	CHECK(validator.validate(5));
	CHECK(validator.validate(6));
	CHECK_FALSE(validator.validate(4));

	std::vector<std::string> errors;
	CHECK_FALSE(validator.validate(3, "testVar", errors));
	CHECK_FALSE(errors.empty());

	CHECK(validator.max(3) == 5);
	CHECK(validator.max(7) == 7);
}

TEST_CASE("NumberLessThanValidator")
{
	Validator v;
	auto validator = v.number.lessThan(10);

	CHECK(validator.validate(9));
	CHECK(validator.validate(5));
	CHECK_FALSE(validator.validate(10));
	CHECK_FALSE(validator.validate(11));

	std::vector<std::string> errors;
	CHECK_FALSE(validator.validate(15, "testVar", errors));
	CHECK_FALSE(errors.empty());

	CHECK(validator.min(15) == 10);
	CHECK(validator.min(7) == 7);
}

TEST_CASE("NumberLessOrEqualValidator")
{
	Validator v;
	auto validator = v.number.lessOrEqual(10);

	CHECK(validator.validate(10));
	CHECK(validator.validate(9));
	CHECK_FALSE(validator.validate(11));

	std::vector<std::string> errors;
	CHECK_FALSE(validator.validate(15, "testVar", errors));
	CHECK_FALSE(errors.empty());

	CHECK(validator.min(15) == 10);
	CHECK(validator.min(7) == 7);
}

TEST_CASE("NumberMultipleOfValidator")
{
	Validator v;
	auto validator = v.number.multipleOf(5);

	CHECK(validator.validate(5));
	CHECK(validator.validate(10));
	CHECK(validator.validate(15));
	CHECK(validator.validate(0));
	CHECK_FALSE(validator.validate(7));
	CHECK_FALSE(validator.validate(13));

	std::vector<std::string> errors;
	CHECK_FALSE(validator.validate(7, "testVar", errors));
	CHECK_FALSE(errors.empty());
}

TEST_CASE("NumberLiteralValidator")
{
	Validator v;
	auto validator = v.number.literals<int>({1, 3, 5, 7, 9});

	CHECK(validator.validate(1));
	CHECK(validator.validate(5));
	CHECK(validator.validate(9));
	CHECK_FALSE(validator.validate(2));
	CHECK_FALSE(validator.validate(10));

	std::vector<std::string> errors;
	CHECK_FALSE(validator.validate(2, "testVar", errors));
	CHECK_FALSE(errors.empty());
}

TEST_CASE("NumberValidator - Different Types")
{
	Validator v;

	// Test with int
	auto intValidator = v.number.between(5, 10);
	CHECK(intValidator.validate(7));

	// Test with double
	auto doubleValidator = v.number.between(5.0, 10.0);
	CHECK(doubleValidator.validate(7.5));

	// Test with float
	auto floatValidator = v.number.greaterThan(5.0f);
	CHECK(floatValidator.validate(6.0f));
}

// String Length Validator Tests
TEST_CASE("StringLengthBetweenValidator")
{
	Validator v;
	auto validator = v.string.length.between(3, 10);

	CHECK(validator.validate("abc"));
	CHECK(validator.validate("abcdefghij"));
	CHECK(validator.validate("hello"));
	CHECK_FALSE(validator.validate("ab"));
	CHECK_FALSE(validator.validate("abcdefghijk"));

	std::vector<std::string> errors;
	CHECK_FALSE(validator.validate("ab", "testVar", errors));
	CHECK_FALSE(errors.empty());
}

TEST_CASE("StringLengthMinValidator")
{
	Validator v;
	auto validator = v.string.length.min(5);

	CHECK(validator.validate("hello"));
	CHECK(validator.validate("hello world"));
	CHECK_FALSE(validator.validate("hi"));
	CHECK_FALSE(validator.validate(""));

	std::vector<std::string> errors;
	CHECK_FALSE(validator.validate("hi", "testVar", errors));
	CHECK_FALSE(errors.empty());
}

TEST_CASE("StringLengthMaxValidator")
{
	Validator v;
	auto validator = v.string.length.max(5);

	CHECK(validator.validate("hello"));
	CHECK(validator.validate("hi"));
	CHECK(validator.validate(""));
	CHECK_FALSE(validator.validate("hello world"));

	std::vector<std::string> errors;
	CHECK_FALSE(validator.validate("hello world", "testVar", errors));
	CHECK_FALSE(errors.empty());

	// Test crop
	CHECK(validator.crop("hello world") == "hello");
}

// String Validator Tests
TEST_CASE("StringLiteralValidator")
{
	Validator v;
	auto validator = v.string.literals({"apple", "banana", "cherry"});

	CHECK(validator.validate("apple"));
	CHECK(validator.validate("banana"));
	CHECK(validator.validate("cherry"));
	CHECK_FALSE(validator.validate("orange"));
	CHECK_FALSE(validator.validate("Apple")); // Case sensitive

	std::vector<std::string> errors;
	CHECK_FALSE(validator.validate("orange", "fruit", errors));
	CHECK_FALSE(errors.empty());
}

TEST_CASE("StringStartsWithValidator")
{
	Validator v;
	auto validator = v.string.startsWith("hello");

	CHECK(validator.validate("hello world"));
	CHECK(validator.validate("hello"));
	CHECK_FALSE(validator.validate("hi hello"));
	CHECK_FALSE(validator.validate("world"));

	std::vector<std::string> errors;
	CHECK_FALSE(validator.validate("world", "testVar", errors));
	CHECK_FALSE(errors.empty());
}

TEST_CASE("StringEndsWithValidator")
{
	Validator v;
	auto validator = v.string.endsWith("world");

	CHECK(validator.validate("hello world"));
	CHECK(validator.validate("world"));
	CHECK_FALSE(validator.validate("world hello"));
	CHECK_FALSE(validator.validate("hello"));

	std::vector<std::string> errors;
	CHECK_FALSE(validator.validate("hello", "testVar", errors));
	CHECK_FALSE(errors.empty());
}

TEST_CASE("StringIncludesValidator")
{
	Validator v;
	auto validator = v.string.includes("test");

	CHECK(validator.validate("this is a test"));
	CHECK(validator.validate("test"));
	CHECK(validator.validate("testing"));
	CHECK_FALSE(validator.validate("hello"));
	CHECK_FALSE(validator.validate(""));

	std::vector<std::string> errors;
	CHECK_FALSE(validator.validate("hello", "testVar", errors));
	CHECK_FALSE(errors.empty());
}

TEST_CASE("StringRegexValidator")
{
	Validator v;
	auto validator = v.string.regex("^[0-9]+$");

	CHECK(validator.validate("123"));
	CHECK(validator.validate("0"));
	CHECK(validator.validate("1234567890"));
	CHECK_FALSE(validator.validate("abc"));
	CHECK_FALSE(validator.validate("123abc"));

	std::vector<std::string> errors;
	CHECK_FALSE(validator.validate("abc", "testVar", errors));
	CHECK_FALSE(errors.empty());

	// Test match with captures
	std::vector<std::string> matches;
	auto emailValidator = v.string.regex("^([a-z]+)@([a-z]+)\\.com$");
	CHECK(emailValidator.match("test@example.com", matches));
	CHECK(matches.size() == 2);
	CHECK(matches[0] == "test");
	CHECK(matches[1] == "example");
}

TEST_CASE("StringEmailValidator")
{
	Validator v;
	auto validator = v.string.email();

	CHECK(validator.validate("test@example.com"));
	CHECK(validator.validate("user.name@domain.co.uk"));
	CHECK(validator.validate("user+tag@example.com"));
	CHECK_FALSE(validator.validate("notanemail"));
	CHECK_FALSE(validator.validate("@example.com"));
	CHECK_FALSE(validator.validate("test@"));

	std::vector<std::string> errors;
	CHECK_FALSE(validator.validate("notanemail", "email", errors));
	CHECK_FALSE(errors.empty());
}

TEST_CASE("StringUuidValidator")
{
	Validator v;
	auto validator = v.string.uuid();

	CHECK(validator.validate("123e4567-e89b-12d3-a456-426614174000"));
	CHECK(validator.validate("550e8400-e29b-41d4-a716-446655440000"));
	CHECK_FALSE(validator.validate("not-a-uuid"));
	CHECK_FALSE(validator.validate("123e4567-e89b-12d3-a456"));
	CHECK_FALSE(validator.validate("123e4567-e89b-02d3-a456-426614174000")); // Invalid version

	std::vector<std::string> errors;
	CHECK_FALSE(validator.validate("not-a-uuid", "uuid", errors));
	CHECK_FALSE(errors.empty());
}

TEST_CASE("StringUrlValidator")
{
	Validator v;

	// Test HTTP/HTTPS URLs
	auto httpValidator = v.string.url(EUrlProtocolFlag::Http, EUrlSecureFlag::AllSecureFlags);
	CHECK(httpValidator.validate("http://example.com"));
	CHECK(httpValidator.validate("https://example.com"));
	CHECK_FALSE(httpValidator.validate("ws://example.com"));

	// Test WebSocket URLs
	auto wsValidator = v.string.url(EUrlProtocolFlag::Ws, EUrlSecureFlag::AllSecureFlags);
	CHECK(wsValidator.validate("ws://example.com"));
	CHECK(wsValidator.validate("wss://example.com"));
	CHECK_FALSE(wsValidator.validate("http://example.com"));

	// Test all protocols
	auto allValidator = v.string.url(EUrlProtocolFlag::AllProtocols, EUrlSecureFlag::AllSecureFlags);
	CHECK(allValidator.validate("http://example.com"));
	CHECK(allValidator.validate("https://example.com"));
	CHECK(allValidator.validate("ws://example.com"));
	CHECK(allValidator.validate("wss://example.com"));

	std::vector<std::string> errors;
	CHECK_FALSE(httpValidator.validate("not-a-url", "url", errors));
	CHECK_FALSE(errors.empty());
}

TEST_CASE("StringDateTimeGlobalValidator")
{
	Validator v;
	auto dateTime = v.string.dateTime();

	// Test global date time without offset
	auto globalNone = dateTime.global(EDateTimeOffset::None);
	CHECK(globalNone.validate("2023-12-25T10:30:00Z"));
	CHECK(globalNone.validate("2023-12-25T10:30:00.123Z"));
	CHECK_FALSE(globalNone.validate("2023-12-25T10:30:00+05:00"));

	// Test global date time with optional offset
	auto globalOptional = dateTime.global(EDateTimeOffset::Optional);
	CHECK(globalOptional.validate("2023-12-25T10:30:00Z"));
	CHECK(globalOptional.validate("2023-12-25T10:30:00+05:00"));
	CHECK(globalOptional.validate("2023-12-25T10:30:00-05:00"));
	CHECK(globalOptional.validate("2023-12-25T10:30:00"));

	// Test global date time with required offset
	auto globalRequired = dateTime.global(EDateTimeOffset::Required);
	CHECK(globalRequired.validate("2023-12-25T10:30:00Z"));
	CHECK(globalRequired.validate("2023-12-25T10:30:00+05:00"));
	CHECK_FALSE(globalRequired.validate("2023-12-25T10:30:00"));

	std::vector<std::string> errors;
	CHECK_FALSE(globalNone.validate("invalid", "datetime", errors));
	CHECK_FALSE(errors.empty());
}

TEST_CASE("StringDateTimeLocalValidator")
{
	Validator v;
	auto dateTime = v.string.dateTime();
	auto local = dateTime.local();

	CHECK(local.validate("2023-12-25T10:30:00"));
	CHECK(local.validate("2023-12-25T23:59:59"));
	CHECK(local.validate("2023-01-01T00:00:00"));
	CHECK_FALSE(local.validate("2023-12-25T24:00:00")); // Invalid hour
	CHECK_FALSE(local.validate("2023-13-25T10:30:00")); // Invalid month

	std::vector<std::string> errors;
	CHECK_FALSE(local.validate("invalid", "datetime", errors));
	CHECK_FALSE(errors.empty());
}

TEST_CASE("StringDateValidator")
{
	Validator v;
	auto validator = v.string.date();

	CHECK(validator.validate("2023-12-25"));
	CHECK(validator.validate("2023-01-01"));
	CHECK(validator.validate("2023-02-28"));
	CHECK_FALSE(validator.validate("2023-13-01")); // Invalid month
	CHECK_FALSE(validator.validate("2023-12-32")); // Invalid day
	CHECK_FALSE(validator.validate("23-12-25"));   // Wrong format

	std::vector<std::string> errors;
	CHECK_FALSE(validator.validate("invalid", "date", errors));
	CHECK_FALSE(errors.empty());
}

TEST_CASE("StringTimeValidator")
{
	Validator v;
	auto validator = v.string.time();

	CHECK(validator.validate("10:30:00"));
	CHECK(validator.validate("23:59:59"));
	CHECK(validator.validate("00:00:00"));
	CHECK(validator.validate("10:30"));			 // Seconds optional
	CHECK(validator.validate("10:30:00.123"));	 // With milliseconds
	CHECK_FALSE(validator.validate("24:00:00")); // Invalid hour
	CHECK_FALSE(validator.validate("10:60:00")); // Invalid minute

	std::vector<std::string> errors;
	CHECK_FALSE(validator.validate("invalid", "time", errors));
	CHECK_FALSE(errors.empty());
}

TEST_CASE("StringIpValidator")
{
	Validator v;

	// Test IPv4
	auto ipv4Validator = v.string.ip(EIpVersion::Ipv4, false);
	CHECK(ipv4Validator.validate("192.168.1.1"));
	CHECK(ipv4Validator.validate("0.0.0.0"));
	CHECK(ipv4Validator.validate("255.255.255.255"));
	CHECK_FALSE(ipv4Validator.validate("256.1.1.1"));
	CHECK_FALSE(ipv4Validator.validate("192.168.1"));

	// Test IPv4 with prefix length
	auto ipv4PrefixValidator = v.string.ip(EIpVersion::Ipv4, true);
	CHECK(ipv4PrefixValidator.validate("192.168.1.1/24"));
	CHECK(ipv4PrefixValidator.validate("10.0.0.0/8"));
	CHECK_FALSE(ipv4PrefixValidator.validate("192.168.1.1/33")); // Invalid prefix

	// Test IPv6
	auto ipv6Validator = v.string.ip(EIpVersion::Ipv6, false);
	CHECK(ipv6Validator.validate("2001:0db8:85a3:0000:0000:8a2e:0370:7334"));
	CHECK(ipv6Validator.validate("2001:db8:85a3::8a2e:370:7334"));
	CHECK(ipv6Validator.validate("::1"));
	CHECK_FALSE(ipv6Validator.validate("not-an-ipv6"));

	// Test IPv6 with prefix length
	auto ipv6PrefixValidator = v.string.ip(EIpVersion::Ipv6, true);
	CHECK(ipv6PrefixValidator.validate("2001:db8::/32"));
	CHECK_FALSE(ipv6PrefixValidator.validate("2001:db8::/129")); // Invalid prefix

	std::vector<std::string> errors;
	CHECK_FALSE(ipv4Validator.validate("invalid", "ip", errors));
	CHECK_FALSE(errors.empty());
}

TEST_CASE("StringMacValidator")
{
	Validator v;

	// Test with default separator (colon)
	auto colonValidator = v.string.mac(":");
	CHECK(colonValidator.validate("00:11:22:33:44:55"));
	CHECK(colonValidator.validate("AA:BB:CC:DD:EE:FF"));
	CHECK_FALSE(colonValidator.validate("00-11-22-33-44-55"));
	CHECK_FALSE(colonValidator.validate("00:11:22:33:44")); // Too short

	// Test with hyphen separator
	auto hyphenValidator = v.string.mac("-");
	CHECK(hyphenValidator.validate("00-11-22-33-44-55"));
	CHECK_FALSE(hyphenValidator.validate("00:11:22:33:44:55"));

	// Test with no separator
	auto noSepValidator = v.string.mac("");
	CHECK(noSepValidator.validate("001122334455"));
	CHECK_FALSE(noSepValidator.validate("00:11:22:33:44:55"));

	std::vector<std::string> errors;
	CHECK_FALSE(colonValidator.validate("invalid", "mac", errors));
	CHECK_FALSE(errors.empty());
}

// Integration Tests
TEST_CASE("Multiple Validators - Error Collection")
{
	Validator v;
	std::vector<std::string> errors;

	auto numberValidator = v.number.between(5, 10);
	auto stringValidator = v.string.length.between(3, 10);

	CHECK(numberValidator.validate(7, "num", errors));
	CHECK(stringValidator.validate("hello", "str", errors));
	CHECK(errors.empty());

	CHECK_FALSE(numberValidator.validate(15, "num", errors));
	CHECK_FALSE(stringValidator.validate("ab", "str", errors));
	CHECK(errors.size() == 2);
}

TEST_CASE("Validator - Edge Cases")
{
	Validator v;

	// Empty string tests
	auto minLength = v.string.length.min(0);
	CHECK(minLength.validate(""));

	auto maxLength = v.string.length.max(5);
	CHECK(maxLength.validate(""));

	// Zero value tests
	auto greaterThanZero = v.number.greaterThan(0);
	CHECK_FALSE(greaterThanZero.validate(0));
	CHECK(greaterThanZero.validate(1));

	// Negative number tests
	auto betweenNeg = v.number.between(-10, -5);
	CHECK(betweenNeg.validate(-7));
	CHECK_FALSE(betweenNeg.validate(-15));
	CHECK_FALSE(betweenNeg.validate(-3));
}

TEST_CASE("Validator - Floating Point Numbers")
{
	Validator v;

	auto doubleValidator = v.number.between(5.0, 10.0);
	CHECK(doubleValidator.validate(7.5));
	CHECK(doubleValidator.validate(5.0));
	CHECK(doubleValidator.validate(10.0));
	CHECK_FALSE(doubleValidator.validate(4.9));
	CHECK_FALSE(doubleValidator.validate(10.1));

	auto floatValidator = v.number.greaterThan(0.0f);
	CHECK(floatValidator.validate(0.1f));
	CHECK_FALSE(floatValidator.validate(0.0f));
}
