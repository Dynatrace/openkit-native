/**
 * Copyright 2018-2020 Dynatrace LLC
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "util/json/lexer/JsonLexer.h"
#include "util/json/lexer/JsonLexerConstants.h"
#include "util/json/constants/JsonLiterals.h"
#include "util/json/reader/DefaultResettableReader.h"
#include "core/util/StringUtil.h"

#include <iomanip>

using namespace util::json::lexer;


JsonLexer::JsonLexer(const std::string& input)
	: JsonLexer(std::make_shared<util::json::reader::DefaultResettableReader>(input))
{
}

JsonLexer::JsonLexer(const JsonLexer::ReaderPtr input)
	: mReader(input)
{
}

JsonLexer::~JsonLexer()
{
}

const JsonLexer::JsonTokenPtr JsonLexer::nextToken()
{
	if (mLexerState == JsonLexerState::ERROR)
	{
		throw JsonLexerException("JSON Lexer is in erroneous state");
	}

	if (mLexerState == JsonLexerState::END_OF_FILE)
	{
		return nullptr;
	}

	// "Insignificant whitespace is allowed before or after any of the six structural characters."
	// (https://tools.ietf.org/html/rfc8259#section-2), therefore consume all whitespace characters
	JsonTokenPtr nextToken;
	try
	{
		bool isEndOfFileReached = consumeWhitespaceCharacters();
		if(isEndOfFileReached)
		{
			mLexerState = JsonLexerState::END_OF_FILE;
			nextToken = nullptr;
		}
		else
		{
			mLexerState = JsonLexerState::PARSING;
			nextToken = doParseNextToken();
		}
	}
	catch (JsonLexerException& e)
	{
		mLexerState = JsonLexerState::ERROR;
		throw e;
	}
	catch (std::ios_base::failure&)
	{
		mLexerState = JsonLexerState::ERROR;
		std::throw_with_nested(JsonLexerException("Exception when reading from input stream"));
	}

	return nextToken;
}

const JsonLexer::JsonTokenPtr JsonLexer::doParseNextToken()
{
	// parse next character
	mReader->mark(1);
	char nextChar = static_cast<char>(mReader->read());

	switch (nextChar)
	{
		case LEFT_SQUARE_BRACKET:
			return JsonToken::LEFT_SQUARE_BRACKET_TOKEN;
		case RIGHT_SQUARE_BRACKET:
			return JsonToken::RIGHT_SQUARE_BRACKET_TOKEN;
		case LEFT_BRACE:
			return JsonToken::LEFT_BRACE_TOKEN;
		case RIGHT_BRACE:
			return JsonToken::RIGHT_BRACE_TOKEN;
		case COLON:
			return JsonToken::COLON_TOKEN;
		case COMMA:
			return JsonToken::COMMA_TOKEN;
		case TRUE_LITERAL_START: // fallthrough
		case FALSE_LITERAL_START:
			// could be a boolean literal (true|false)
			mReader->reset();
			return tryParseBooleanLiteral();
		case NULL_LITERAL_START:
			// could be null literal
			mReader->reset();
			return tryParseNullLiteral();
		case QUOTATION_MARK:
			// string value - omit the starting "
			return tryParseStringToken();
		default:
			// check if it is a number or a completely unknown token
			if(isDigitOrMinus(nextChar))
			{
				mReader->reset();
				return tryParseNumberToken();
			}
			else
			{
				mReader->reset();
				std::string literalToken = parseLiteral();
				throw JsonLexerException(unexpectedLiteralTokenMessage(literalToken));
			}
	}
}

const JsonLexer::JsonTokenPtr JsonLexer::tryParseBooleanLiteral()
{
	auto literal = parseLiteral();
	if (literal == util::json::constants::JsonLiterals::BOOLEAN_TRUE_LITERAL)
	{
		return JsonToken::BOOLEAN_TRUE_TOKEN;
	}
	else if (literal == util::json::constants::JsonLiterals::BOOLEAN_FALSE_LITERAL)
	{
		return JsonToken::BOOLEAN_FALSE_TOKEN;
	}

	// not a valid boolean literal
	throw JsonLexerException(unexpectedLiteralTokenMessage(literal));
}

const JsonLexer::JsonTokenPtr JsonLexer::tryParseNullLiteral()
{
	auto literal = parseLiteral();
	if (literal == util::json::constants::JsonLiterals::NULL_LITERAL)
	{
		return JsonToken::NULL_TOKEN;
	}

	// not a valid null literal
	throw JsonLexerException(unexpectedLiteralTokenMessage(literal));
}

const JsonLexer::JsonTokenPtr JsonLexer::tryParseStringToken()
{
	auto stringValueStream = std::stringstream();
	auto nextChar = mReader->read();

	while (nextChar != EOF  && nextChar != QUOTATION_MARK)
	{
		if (isEscapeCharacter(nextChar))
		{
			tryParseEscapeSequence(stringValueStream);
		}
		else if (isCharacterThatNeedsEscaping(nextChar))
		{
			std::ostringstream os;
			os << "Invalid control character \"\\u" << std::hex << std::setw(4) << std::setfill('0') << std::uppercase << nextChar << "\"";
			throw JsonLexerException(os.str());
		}
		else
		{
			stringValueStream << static_cast<char>(nextChar);
		}

		nextChar = mReader->read();
	}

	auto stringValue = stringValueStream.str();

	if (nextChar != EOF)
	{
		// string is properly terminated
		return JsonToken::createStringToken(stringValue);
	}

	// string is not properly terminated because EOF was reached
	throw JsonLexerException("Unterminated string literal \"" + stringValue + "\"");
}

void JsonLexer::tryParseEscapeSequence(std::stringstream& literalString)
{
	auto nextChar = mReader->read();

	switch (nextChar)
	{
		case EOF:
			throw JsonLexerException("Unterminated string literal \"" + literalString.str() + "\"");
		case QUOTATION_MARK:  // fallthrough
		case REVERSE_SOLIDUS: // fallthrough
		case SOLIDUS:
			literalString << static_cast<char>(nextChar);
			break;
		case 'b':
			literalString << BACKSPACE;
			break;
		case 'f':
			literalString << FORM_FEED;
			break;
		case 'n':
			literalString << LINE_FEED;
			break;
		case 'r':
			literalString << CARRIAGE_RETURN;
			break;
		case 't':
			literalString << HORIZONTAL_TAB;
			break;
		case 'u':
			tryParseUnicodeEscapeSequence(literalString);
			break;
		default:
			std::ostringstream os;
			os << "Invalid escape sequence \"\\" << static_cast<char>(nextChar) << "\"";
			throw JsonLexerException(os.str());
	}
}

void JsonLexer::tryParseUnicodeEscapeSequence(std::stringstream& literalString)
{
	auto unicodeSequence = readUnicodeEscapeSequence();

	// parse out the hex character
	auto parsedInt = std::strtol(unicodeSequence.c_str(), nullptr, 16);

	unsigned char parsedIntHighPart = (0xFF00 & parsedInt) >> 8;
	unsigned char parsedIntLowPart = 0x00FF & parsedInt;

	if (core::util::StringUtil::isHighSurrogateCharacter(parsedInt))
	{
		// try to parse subsequent low surrogate
		bool hasParsedLowSurrogateChar = false;
		auto lowSurrogate = tryParseLowSurrogateChar(hasParsedLowSurrogateChar);
		if (!hasParsedLowSurrogateChar || !core::util::StringUtil::isLowSurrogateCharacter(lowSurrogate))
		{
			throw JsonLexerException("Invalid UTF-16 surrogate pair \"\\u" + unicodeSequence + "\"");
		}

		// convert UTF-16 surrogate pair to proper UTF-8 representation
		std::u16string surrogatePairString{ static_cast<char16_t >(parsedInt), static_cast<char16_t>(lowSurrogate) };
		auto convertedSurrogatePair = core::util::StringUtil::convertUtf16StringToUtf8String(surrogatePairString);

		literalString << convertedSurrogatePair;
	}
	else if (core::util::StringUtil::isLowSurrogateCharacter(parsedInt))
	{
		// low surrogate character without previous high surrogate
		throw JsonLexerException("Invalid UTF-16 surrogate pair \"\\u" + unicodeSequence + "\"");
	}
	else
	{
		if (parsedIntHighPart)
		{
			literalString << parsedIntHighPart;
		}

		literalString << parsedIntLowPart;
	}
}

int32_t JsonLexer::tryParseLowSurrogateChar(bool& isSuccess)
{
	mReader->mark(1);

	if (mReader->read() == REVERSE_SOLIDUS && mReader->read() == 'u')
	{
		auto unicodeSequence = readUnicodeEscapeSequence();
		auto lowSurrogateChar = static_cast<int32_t>(strtol(unicodeSequence.c_str(), nullptr, 16));
		isSuccess = true;
		return lowSurrogateChar;
	}

	isSuccess = false;
	mReader->reset();
	return -1;
}

std::string JsonLexer::readUnicodeEscapeSequence()
{
	auto stream = std::stringstream();

	auto nextChar = mReader->read();
	while (nextChar != EOF && stream.tellp() < NUM_UNICODE_CHARACTERS)
	{
		if (!isHexCharacter(nextChar))
		{
			throw JsonLexerException("Invalid unicode escape sequence \"\\u" + stream.str() + static_cast<char>(nextChar) + "\"");
		}

		stream << static_cast<char>(nextChar);

		mReader->mark(1);
		nextChar = mReader->read();
	}

	if (nextChar == EOF)
	{
		// string is not properly terminated, because EOF was reached
		throw JsonLexerException("Unterminated string literal \"\\u" + stream.str() + "\"");
	}

	// 4 hex characters were parsed
	mReader->reset();

	return stream.str();
}

JsonLexer::JsonTokenPtr JsonLexer::tryParseNumberToken()
{
	auto literal = parseLiteral();

	if (std::regex_match(literal, util::json::constants::JsonLiterals::getNumberPattern()))
	{
		return JsonToken::createNumberToken(literal);
	}

	// not a valid number literal
	throw JsonLexerException("Invalid number literal \"" + literal + "\"");
}

const std::string JsonLexer::parseLiteral()
{
	auto literalStream = std::stringstream();

	mReader->mark(1);
	auto chr = mReader->read();

	while (chr != EOF && !isJsonWhitespaceCharacter(chr) && !isJsonStructuralCharacter(chr))
	{
		literalStream << static_cast<char>(chr);

		mReader->mark(1);
		chr = mReader->read();
	}

	if (chr != EOF)
	{
		// reset read position, which might be a whitespace or structural character
		mReader->reset();
	}

	return literalStream.str();
}

bool JsonLexer::consumeWhitespaceCharacters()
{
	int32_t chr;
	do
	{
		mReader->mark(1);
		chr = mReader->read();
	}
	while (chr != EOF && isJsonWhitespaceCharacter(chr));

	if (chr != EOF)
	{
		// reset read position to the first non-whitespace character
		mReader->reset();
		return false;
	}

	return true;
}

bool JsonLexer::isJsonWhitespaceCharacter(int32_t chr)
{
	switch (chr)
	{
		case SPACE:            // fallthrough
		case HORIZONTAL_TAB:   // fallthrough
		case LINE_FEED:        // fallthrough
		case CARRIAGE_RETURN:
			return true;
		default:
			return false;
	}
}

bool JsonLexer::isJsonStructuralCharacter(int32_t chr)
{
	switch (chr)
	{
		case LEFT_SQUARE_BRACKET:  // fallthrough
		case RIGHT_SQUARE_BRACKET: // fallthrough
		case LEFT_BRACE:           // fallthrough
		case RIGHT_BRACE:          // fallthrough
		case COLON:                // fallthrough
		case COMMA:
			return true;
		default:
			return false;
	}
}

bool JsonLexer::isDigitOrMinus(int32_t chr)
{
	return chr == '-' || (chr >= '0' && chr <= '9');
}

bool JsonLexer::isEscapeCharacter(int32_t chr)
{
	return chr == REVERSE_SOLIDUS;
}

bool JsonLexer::isCharacterThatNeedsEscaping(int32_t chr)
{
	return chr == QUOTATION_MARK
		|| chr == REVERSE_SOLIDUS
		|| (chr >= 0 && chr <= 0x1F);
}

bool JsonLexer::isHexCharacter(int32_t chr)
{
	return (chr >= '0' && chr <= '9')
		|| (chr >= 'a' && chr <= 'f')
		|| (chr >= 'A' && chr <= 'F');
}

std::string JsonLexer::unexpectedLiteralTokenMessage(std::string& literalToken)
{
	return "Unexpected literal \"" + literalToken + "\"";
}