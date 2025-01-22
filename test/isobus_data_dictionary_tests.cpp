#include <gtest/gtest.h>

#include "isobus/isobus/isobus_data_dictionary.hpp"

using namespace isobus;

TEST(DATA_DICTIONARY_TESTS, DDI_Lookups)
{
	auto testEntry = DataDictionary::get_entry(229); // Test "actual net weight"

	EXPECT_EQ(229, testEntry.ddi);
	EXPECT_EQ("Actual Net Weight", testEntry.name);
	EXPECT_EQ(1, testEntry.resolution);
	EXPECT_EQ("g", testEntry.unitSymbol);
	EXPECT_EQ("Mass large", testEntry.unitDescription);
	EXPECT_NEAR(-2147483648.0f, testEntry.displayRange.first, 0.001);
	EXPECT_NEAR(2147483647.0f, testEntry.displayRange.second, 0.001);

	auto testEntry2 = DataDictionary::get_entry(40962); // Test  40962 - Default Crop Grade Length
	EXPECT_EQ(40962, testEntry2.ddi);
	EXPECT_EQ("Default Crop Grade Length", testEntry2.name);
	EXPECT_NEAR(0.001, testEntry2.resolution, 0.001);
	EXPECT_EQ("mm", testEntry2.unitSymbol);
	EXPECT_EQ("Length", testEntry2.unitDescription);
	EXPECT_NEAR(0.0f, testEntry2.displayRange.first, 0.001);
	EXPECT_NEAR(2147483647.0f, testEntry2.displayRange.second, 0.001);

	// Test an invalid, random ddi
	auto testEntry3 = DataDictionary::get_entry(1957);
	EXPECT_EQ(65535, testEntry3.ddi);
	EXPECT_EQ("Unknown", testEntry3.name);
	EXPECT_EQ(0, testEntry3.resolution);
	EXPECT_EQ("Unknown", testEntry3.unitSymbol);
	EXPECT_EQ("Unknown", testEntry3.unitDescription);
	EXPECT_EQ(0.0f, testEntry3.displayRange.first);
	EXPECT_EQ(0.0f, testEntry3.displayRange.second);
}
