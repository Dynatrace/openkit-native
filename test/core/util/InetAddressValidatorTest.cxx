/**
* Copyright 2018 Dynatrace LLC
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

#include "core/util/InetAddressValidator.h"
#include "core/UTF8String.h"

#include <gtest/gtest.h>

using namespace core::util;

class InetAddressValidatorTest : public testing::Test
{

};

TEST_F(InetAddressValidatorTest, IPV4AddressIsValid)
{
	//given
	core::UTF8String ipv4TestString("122.133.55.22");

	//then 
	ASSERT_TRUE(InetAddressValidator::IsValidIP(ipv4TestString));
}

TEST_F(InetAddressValidatorTest, IPV4AddressIsValidAllZero)
{
	//given
	core::UTF8String ipv4TestString("0.0.0.0");

	//then 
	ASSERT_TRUE(InetAddressValidator::IsValidIP(ipv4TestString));
}

TEST_F(InetAddressValidatorTest, IPV4AddressIsValidAllEigth)
{
	//given
	core::UTF8String ipv4TestString("8.8.8.8");

	//then 
	ASSERT_TRUE(InetAddressValidator::IsValidIP(ipv4TestString));
}

TEST_F(InetAddressValidatorTest, IPV4AddressIsValidHighestPossible)
{
	//given
	core::UTF8String ipv4TestString("255.255.255.255");

	//then 
	ASSERT_TRUE(InetAddressValidator::IsValidIP(ipv4TestString));
}

TEST_F(InetAddressValidatorTest, IPV4AddressIsInvalidBecauseOfOverflow)
{
	//given
	core::UTF8String ipv4TestString("255.255.255.256");

	//then 
	ASSERT_FALSE(InetAddressValidator::IsValidIP(ipv4TestString));
}

TEST_F(InetAddressValidatorTest, IPV4AddressIsInvalidDoubleColonsInsteadOfPoints)
{
	//given
	core::UTF8String ipv4TestString("255:255:255:255");

	//then 
	ASSERT_FALSE(InetAddressValidator::IsValidIP(ipv4TestString));
}

TEST_F(InetAddressValidatorTest, IPV4AddressIsInvalidDueToAdditionalCharacterInFirstBlock)
{
	//given
	core::UTF8String ipv4TestString("122x.133.55.22");

	//then 
	ASSERT_FALSE(InetAddressValidator::IsValidIP(ipv4TestString));
}

TEST_F(InetAddressValidatorTest, IPV4AddressIsInvalidDueToAdditionalCharacterInSecondBlock)
{
	//given
	core::UTF8String ipv4TestString("122.133x.55.22");

	//then 
	ASSERT_FALSE(InetAddressValidator::IsValidIP(ipv4TestString));
}

TEST_F(InetAddressValidatorTest, IPV4AddressIsInvalidDueToAdditionalCharacterInThirdBlock)
{
	//given
	core::UTF8String ipv4TestString("122.133.55x.22");

	//then 
	ASSERT_FALSE(InetAddressValidator::IsValidIP(ipv4TestString));
}

TEST_F(InetAddressValidatorTest, IPV4AddressIsInvalidDueToAdditionalCharacterInFourthBlock)
{
	//given
	core::UTF8String ipv4TestString("122.133.55.22x");

	//then 
	ASSERT_FALSE(InetAddressValidator::IsValidIP(ipv4TestString));
}

TEST_F(InetAddressValidatorTest, IPV4AddressIsInvalidDueToIllegalValueOverrun)
{
	//given
	core::UTF8String ipv4TestString("122.133.256.22");

	//then 
	ASSERT_FALSE(InetAddressValidator::IsValidIP(ipv4TestString));
}

TEST_F(InetAddressValidatorTest, IPV4AddressIsInvalidDueToIllegalValueNegative)
{
	//given
	core::UTF8String ipv4TestString("122.133.256.-22");

	//then 
	ASSERT_FALSE(InetAddressValidator::IsValidIP(ipv4TestString));
}

TEST_F(InetAddressValidatorTest, IPV6AddressIsValid)
{
	//given
	core::UTF8String ipv6TestString("23fe:33af:1232:5522:abcd:2532:1a2b:1");

	//then 
	ASSERT_TRUE(InetAddressValidator::IsValidIP(ipv6TestString));
}

TEST_F(InetAddressValidatorTest, IPV6AddressIsInvalidOverflow)
{
	//given
	core::UTF8String ipv6TestString("23fec:33af:1232:5522:abcd:2532:1a2b:1");

	//then 
	ASSERT_FALSE(InetAddressValidator::IsValidIP(ipv6TestString));
}

TEST_F(InetAddressValidatorTest, IPV6AddressIsInvalidIllegalCharacter)
{
	//given
	core::UTF8String ipv6TestString("23fl:33af:1232:5522:abcd:2532:1a2b:1");

	//then 
	ASSERT_FALSE(InetAddressValidator::IsValidIP(ipv6TestString));
}

TEST_F(InetAddressValidatorTest, IPV6AddressIsInvalidTooManyBlocks)
{
	//given
	core::UTF8String ipv6TestString("23fl:33af:1232:5522:abcd:2532:1a2b:1:2:3");

	//then 
	ASSERT_FALSE(InetAddressValidator::IsValidIP(ipv6TestString));
}

TEST_F(InetAddressValidatorTest, IPV6AddressHexCompressedIsValidBlock4)
{
	//given
	core::UTF8String ipv6TestString("2001:db:85:b::1A");

	//then 
	ASSERT_TRUE(InetAddressValidator::IsValidIP(ipv6TestString));
}

TEST_F(InetAddressValidatorTest, IPV6AddressHexCompressedIsValidBlock3)
{
	//given
	core::UTF8String ipv6TestString("2001:db:85::b:1A");

	//then 
	ASSERT_TRUE(InetAddressValidator::IsValidIP(ipv6TestString));
}

TEST_F(InetAddressValidatorTest, IPV6AddressHexCompressedIsValidBlock2)
{
	//given
	core::UTF8String ipv6TestString("2001:db::85:b:1A");

	//then 
	ASSERT_TRUE(InetAddressValidator::IsValidIP(ipv6TestString));
}

TEST_F(InetAddressValidatorTest, IPV6AddressHexCompressedIsValidBlock1)
{
	//given
	core::UTF8String ipv6TestString("2001::db:85:b:1A");

	//then 
	ASSERT_TRUE(InetAddressValidator::IsValidIP(ipv6TestString));
}

TEST_F(InetAddressValidatorTest, IPV6AddressHexCompressedIsValidShortestPossible)
{
	//given
	core::UTF8String ipv6TestString("2001::b1A");

	//then 
	ASSERT_TRUE(InetAddressValidator::IsValidIP(ipv6TestString));
}

TEST_F(InetAddressValidatorTest, IPV6AddressHexCompressedIsInvalidTwoCompressedBlocks)
{
	//given
	core::UTF8String ipv6TestString("2001::db:85::b1A");

	//then 
	ASSERT_FALSE(InetAddressValidator::IsValidIP(ipv6TestString));
}

TEST_F(InetAddressValidatorTest, IPV6AddressHexCompressedIsInvalidFirstBlockMissing)
{
	//given
	core::UTF8String ipv6TestString(":4::5:6");

	//then 
	ASSERT_FALSE(InetAddressValidator::IsValidIP(ipv6TestString));
}


TEST_F(InetAddressValidatorTest, IPV6AddressMixedNotationIsValid)
{
	//given
	core::UTF8String ipv6TestString("0::FF:FF:172.12.55.18");

	//then 
	ASSERT_TRUE(InetAddressValidator::IsValidIP(ipv6TestString));
}

TEST_F(InetAddressValidatorTest, IPV6AddressMixedNotationIsInvalidOnly3IPv4Blocks)
{
	//given
	core::UTF8String ipv6TestString("0::FF:FF:172.12.55");

	//then 
	ASSERT_FALSE(InetAddressValidator::IsValidIP(ipv6TestString));
}

TEST_F(InetAddressValidatorTest, IPV6AddressMixedNotationIsInvalidIPV6PartInvalid)
{
	//given
	core::UTF8String ipv6TestString("0::FF:FF:FF:172.12.55");

	//then 
	ASSERT_FALSE(InetAddressValidator::IsValidIP(ipv6TestString));
}

TEST_F(InetAddressValidatorTest, IPV6AddressMixedNotationIsInvalidMissingNumberFirstBlock)
{
	//given
	core::UTF8String ipv6TestString("::FF:FF:172.12.55");

	//then 
	ASSERT_FALSE(InetAddressValidator::IsValidIP(ipv6TestString));
}

TEST_F(InetAddressValidatorTest, IPV6AddressLinkLocalIsValid)
{
	//given
	core::UTF8String ipv6TestStringLinkLocal("fe80::208:74ff:feda:625c%5");

	//then 
	ASSERT_TRUE(InetAddressValidator::IsValidIP(ipv6TestStringLinkLocal));
}

TEST_F(InetAddressValidatorTest, IPV6AddressLinkLocalIsValidVeryShortLinkLocal)
{
	//given
	core::UTF8String ipv6TestStringLinkLocal("fe80::625c%5");

	//then 
	ASSERT_TRUE(InetAddressValidator::IsValidIP(ipv6TestStringLinkLocal));
}

TEST_F(InetAddressValidatorTest, IPV6AddressLinkLocalIsInvalidTooManyBlocks)
{
	//given
	core::UTF8String ipv6TestStringLinkLocal("fe80:34:208:74ff:feda:dada:625c:8976:abcd%5");

	//then 
	ASSERT_FALSE(InetAddressValidator::IsValidIP(ipv6TestStringLinkLocal));
}

TEST_F(InetAddressValidatorTest, IPV6AddressLinkLocalIsInvalidIllegalNonHexCharacter)
{
	//given
	core::UTF8String ipv6TestStringLinkLocal("fe80::208t:74ff:feda:dada:625c%5");

	//then 
	ASSERT_FALSE(InetAddressValidator::IsValidIP(ipv6TestStringLinkLocal));
}

TEST_F(InetAddressValidatorTest, IPV6AddressLinkLocalIsInvalidDueToTwoDoubleColonsInAddress)
{
	//given
	core::UTF8String ipv6TestStringLinkLocal("fe80::208:74ff::dada:625c%5");

	//then 
	ASSERT_FALSE(InetAddressValidator::IsValidIP(ipv6TestStringLinkLocal));
}

TEST_F(InetAddressValidatorTest, IPV6AddressLinkLocalIsInvalidZoneIndexUsedWithInvalidPrefix)
{
	//given
	core::UTF8String ipv6TestStringLinkLocal("fedd::208:74ff::dada:625c%5");

	//then 
	ASSERT_FALSE(InetAddressValidator::IsValidIP(ipv6TestStringLinkLocal));
}

// the following two addresses are not valid according to RFC5952 but are accepted by python's implementation and also ours

TEST_F(InetAddressValidatorTest, IPV6AddressValid_RFCLeadingZeros)
{
	//given
	core::UTF8String ipv6TestStringLinkLocal("2001:0db8::0001");

	//then 
	ASSERT_TRUE(InetAddressValidator::IsValidIP(ipv6TestStringLinkLocal));
}

TEST_F(InetAddressValidatorTest, IPV6AddressValid_RFCEmptyBlockNotShortened)
{
	//given
	core::UTF8String ipv6TestStringLinkLocal("2001:db8::0:1");

	//then 
	ASSERT_TRUE(InetAddressValidator::IsValidIP(ipv6TestStringLinkLocal));
}

TEST_F(InetAddressValidatorTest, IPV6AddressValid_RFCInvalidBlockCompress)
{
	//given
	core::UTF8String ipv6TestStringLinkLocal("2001:db8::1:1:1:1:1 ");

	//then 
	ASSERT_FALSE(InetAddressValidator::IsValidIP(ipv6TestStringLinkLocal));
}

TEST_F(InetAddressValidatorTest, IPV6AddressValid_RFCExample)
{
	//given
	core::UTF8String ipv6TestStringLinkLocal("2001:db8::1:0:0:1");

	//then 
	ASSERT_TRUE(InetAddressValidator::IsValidIP(ipv6TestStringLinkLocal));
}