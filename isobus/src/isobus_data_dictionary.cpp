//================================================================================================
/// @file isobus_data_dictionary.cpp
///
/// @brief This file contains an auto-generated lookup table of all ISOBUS DDIs as defined
/// in ISO11783-11, exported from isobus.net.
/// This file was generated January 16, 2025.
///
/// @author Adrian Del Grosso
/// @author Daan Steenbergen
/// @copyright 2024 The Open-Agriculture Developers
//================================================================================================
#include "isobus/isobus/isobus_data_dictionary.hpp"

#include "isobus/isobus/isobus_standard_data_description_indices.hpp"

#include <limits>
#include <map>
#include <sstream>

namespace isobus
{
	const DataDictionary::Entry &DataDictionary::get_entry(std::uint16_t dataDictionaryIdentifier)
	{
#ifndef DISABLE_ISOBUS_DATA_DICTIONARY
		for (std::uint_fast16_t i = 0; i < sizeof(DDI_ENTRIES) / sizeof(DataDictionary::Entry); i++)
		{
			if (DDI_ENTRIES[i].ddi == dataDictionaryIdentifier)
			{
				return DDI_ENTRIES[i];
			}
		}
#endif
		return DEFAULT_ENTRY;
	}

	std::string DataDictionary::Entry::to_string() const
	{
		return name;
	}

	std::string DataDictionary::Entry::format_value(const std::int32_t value) const
	{
#ifndef DISABLE_ISOBUS_DATA_DICTIONARY
		switch (ddi)
		{
			case static_cast<std::uint16_t>(DataDescriptionIndex::ActualWorkState):
			case static_cast<std::uint16_t>(DataDescriptionIndex::PrescriptionControlState):
			case static_cast<std::uint16_t>(DataDescriptionIndex::SectionControlState):
			case static_cast<std::uint16_t>(DataDescriptionIndex::ActualHeaderWorkingHeightStatus):
			case static_cast<std::uint16_t>(DataDescriptionIndex::ActualHeaderRotationalSpeedStatus):
			case static_cast<std::uint16_t>(DataDescriptionIndex::YieldHoldStatus):
			case static_cast<std::uint16_t>(DataDescriptionIndex::SetpointWorkState):
			case static_cast<std::uint16_t>(DataDescriptionIndex::TramlineControlState):
			{
				switch (value & 0x03)
				{
					case 0x00:
						return "Off";
					case 0x01:
						return "On";
					case 0x02:
						return "Error";
					case 0x03:
						return "Not installed";
					default:
						return "Unknown";
				}
			}
			case static_cast<std::uint16_t>(DataDescriptionIndex::ConnectorType):
			{
				switch (value)
				{
					case -1:
						return "Not available";
					case 0:
						return "unknown (default)";
					case 1:
						return "ISO 6489-3 Tractor drawbar";
					case 2:
						return "ISO 730 Three-point-hitch semi-mounted";
					case 3:
						return "ISO 730 Three-point-hitch mounted";
					case 4:
						return "ISO 6489-1 Hitch-hook";
					case 5:
						return "ISO 6489-2 Clevis coupling 40";
					case 6:
						return "ISO 6489-4 Piton type coupling";
					case 7:
						return "ISO 6489-5 CUNA hitch";
					case 8:
						return "ISO 24347 Ball type hitch";
					case 9:
						return "Chassis Mounted - Self-Propelled";
					case 10:
						return "ISO 5692-2 Pivot wagon hitch";
					default:
						return "reserved for future assignments";
				}
			}
			case static_cast<std::uint16_t>(DataDescriptionIndex::ActualCondensedWorkState1_16):
			case static_cast<std::uint16_t>(DataDescriptionIndex::ActualCondensedWorkState17_32):
			case static_cast<std::uint16_t>(DataDescriptionIndex::ActualCondensedWorkState33_48):
			case static_cast<std::uint16_t>(DataDescriptionIndex::ActualCondensedWorkState49_64):
			case static_cast<std::uint16_t>(DataDescriptionIndex::ActualCondensedWorkState65_80):
			case static_cast<std::uint16_t>(DataDescriptionIndex::ActualCondensedWorkState81_96):
			case static_cast<std::uint16_t>(DataDescriptionIndex::ActualCondensedWorkState97_112):
			case static_cast<std::uint16_t>(DataDescriptionIndex::ActualCondensedWorkState113_128):
			case static_cast<std::uint16_t>(DataDescriptionIndex::ActualCondensedWorkState129_144):
			case static_cast<std::uint16_t>(DataDescriptionIndex::ActualCondensedWorkState145_160):
			case static_cast<std::uint16_t>(DataDescriptionIndex::ActualCondensedWorkState161_176):
			case static_cast<std::uint16_t>(DataDescriptionIndex::ActualCondensedWorkState177_192):
			case static_cast<std::uint16_t>(DataDescriptionIndex::ActualCondensedWorkState193_208):
			case static_cast<std::uint16_t>(DataDescriptionIndex::ActualCondensedWorkState209_224):
			case static_cast<std::uint16_t>(DataDescriptionIndex::ActualCondensedWorkState225_240):
			case static_cast<std::uint16_t>(DataDescriptionIndex::ActualCondensedWorkState241_256):
			case static_cast<std::uint16_t>(DataDescriptionIndex::SetpointCondensedWorkState1_16):
			case static_cast<std::uint16_t>(DataDescriptionIndex::SetpointCondensedWorkState17_32):
			case static_cast<std::uint16_t>(DataDescriptionIndex::SetpointCondensedWorkState33_48):
			case static_cast<std::uint16_t>(DataDescriptionIndex::SetpointCondensedWorkState49_64):
			case static_cast<std::uint16_t>(DataDescriptionIndex::SetpointCondensedWorkState65_80):
			case static_cast<std::uint16_t>(DataDescriptionIndex::SetpointCondensedWorkState81_96):
			case static_cast<std::uint16_t>(DataDescriptionIndex::SetpointCondensedWorkState97_112):
			case static_cast<std::uint16_t>(DataDescriptionIndex::SetpointCondensedWorkState113_128):
			case static_cast<std::uint16_t>(DataDescriptionIndex::SetpointCondensedWorkState129_144):
			case static_cast<std::uint16_t>(DataDescriptionIndex::SetpointCondensedWorkState145_160):
			case static_cast<std::uint16_t>(DataDescriptionIndex::SetpointCondensedWorkState161_176):
			case static_cast<std::uint16_t>(DataDescriptionIndex::SetpointCondensedWorkState177_192):
			case static_cast<std::uint16_t>(DataDescriptionIndex::SetpointCondensedWorkState193_208):
			case static_cast<std::uint16_t>(DataDescriptionIndex::SetpointCondensedWorkState209_224):
			case static_cast<std::uint16_t>(DataDescriptionIndex::SetpointCondensedWorkState225_240):
			case static_cast<std::uint16_t>(DataDescriptionIndex::SetpointCondensedWorkState241_256):
			case static_cast<std::uint16_t>(DataDescriptionIndex::ActualTramlineCondensedWorkState1_16):
			case static_cast<std::uint16_t>(DataDescriptionIndex::ActualTramlineCondensedWorkState17_32):
			case static_cast<std::uint16_t>(DataDescriptionIndex::ActualTramlineCondensedWorkState33_48):
			case static_cast<std::uint16_t>(DataDescriptionIndex::ActualTramlineCondensedWorkState49_64):
			case static_cast<std::uint16_t>(DataDescriptionIndex::ActualTramlineCondensedWorkState65_80):
			case static_cast<std::uint16_t>(DataDescriptionIndex::ActualTramlineCondensedWorkState81_96):
			case static_cast<std::uint16_t>(DataDescriptionIndex::ActualTramlineCondensedWorkState97_112):
			case static_cast<std::uint16_t>(DataDescriptionIndex::ActualTramlineCondensedWorkState113_128):
			case static_cast<std::uint16_t>(DataDescriptionIndex::ActualTramlineCondensedWorkState129_144):
			case static_cast<std::uint16_t>(DataDescriptionIndex::ActualTramlineCondensedWorkState145_160):
			case static_cast<std::uint16_t>(DataDescriptionIndex::ActualTramlineCondensedWorkState161_176):
			case static_cast<std::uint16_t>(DataDescriptionIndex::ActualTramlineCondensedWorkState177_192):
			case static_cast<std::uint16_t>(DataDescriptionIndex::ActualTramlineCondensedWorkState193_208):
			case static_cast<std::uint16_t>(DataDescriptionIndex::ActualTramlineCondensedWorkState209_224):
			case static_cast<std::uint16_t>(DataDescriptionIndex::ActualTramlineCondensedWorkState225_240):
			case static_cast<std::uint16_t>(DataDescriptionIndex::ActualTramlineCondensedWorkState241_256):
			case static_cast<std::uint16_t>(DataDescriptionIndex::SetpointTramlineCondensedWorkState1_16):
			case static_cast<std::uint16_t>(DataDescriptionIndex::SetpointTramlineCondensedWorkState17_32):
			case static_cast<std::uint16_t>(DataDescriptionIndex::SetpointTramlineCondensedWorkState33_48):
			case static_cast<std::uint16_t>(DataDescriptionIndex::SetpointTramlineCondensedWorkState49_64):
			case static_cast<std::uint16_t>(DataDescriptionIndex::SetpointTramlineCondensedWorkState65_80):
			case static_cast<std::uint16_t>(DataDescriptionIndex::SetpointTramlineCondensedWorkState81_96):
			case static_cast<std::uint16_t>(DataDescriptionIndex::SetpointTramlineCondensedWorkState97_112):
			case static_cast<std::uint16_t>(DataDescriptionIndex::SetpointTramlineCondensedWorkState113_128):
			case static_cast<std::uint16_t>(DataDescriptionIndex::SetpointTramlineCondensedWorkState129_144):
			case static_cast<std::uint16_t>(DataDescriptionIndex::SetpointTramlineCondensedWorkState145_160):
			case static_cast<std::uint16_t>(DataDescriptionIndex::SetpointTramlineCondensedWorkState161_176):
			case static_cast<std::uint16_t>(DataDescriptionIndex::SetpointTramlineCondensedWorkState177_192):
			case static_cast<std::uint16_t>(DataDescriptionIndex::SetpointTramlineCondensedWorkState193_208):
			case static_cast<std::uint16_t>(DataDescriptionIndex::SetpointTramlineCondensedWorkState209_224):
			case static_cast<std::uint16_t>(DataDescriptionIndex::SetpointTramlineCondensedWorkState225_240):
			case static_cast<std::uint16_t>(DataDescriptionIndex::SetpointTramlineCondensedWorkState241_256):
			case static_cast<std::uint16_t>(DataDescriptionIndex::CondensedSectionOverrideState1_16):
			case static_cast<std::uint16_t>(DataDescriptionIndex::CondensedSectionOverrideState17_32):
			case static_cast<std::uint16_t>(DataDescriptionIndex::CondensedSectionOverrideState33_48):
			case static_cast<std::uint16_t>(DataDescriptionIndex::CondensedSectionOverrideState49_64):
			case static_cast<std::uint16_t>(DataDescriptionIndex::CondensedSectionOverrideState65_80):
			case static_cast<std::uint16_t>(DataDescriptionIndex::CondensedSectionOverrideState81_96):
			case static_cast<std::uint16_t>(DataDescriptionIndex::CondensedSectionOverrideState97_112):
			case static_cast<std::uint16_t>(DataDescriptionIndex::CondensedSectionOverrideState113_128):
			case static_cast<std::uint16_t>(DataDescriptionIndex::CondensedSectionOverrideState129_144):
			case static_cast<std::uint16_t>(DataDescriptionIndex::CondensedSectionOverrideState145_160):
			case static_cast<std::uint16_t>(DataDescriptionIndex::CondensedSectionOverrideState161_176):
			case static_cast<std::uint16_t>(DataDescriptionIndex::CondensedSectionOverrideState177_192):
			case static_cast<std::uint16_t>(DataDescriptionIndex::CondensedSectionOverrideState193_208):
			case static_cast<std::uint16_t>(DataDescriptionIndex::CondensedSectionOverrideState209_224):
			case static_cast<std::uint16_t>(DataDescriptionIndex::CondensedSectionOverrideState225_240):
			case static_cast<std::uint16_t>(DataDescriptionIndex::CondensedSectionOverrideState241_256):
			{
				std::ostringstream oss;
				for (int i = 0; i < 16; i++) // 16 sections (32 bits / 2 bits per section)
				{
					uint8_t state = (value >> (i * 2)) & 0x03; // Extract 2 bits for the i-th section
					oss << static_cast<int>(state);
					if (i < 15)
						oss << " "; // Space for better readability
				}
				return oss.str();
			}
			case static_cast<std::uint16_t>(DataDescriptionIndex::ActualCulturalPractice):
			case static_cast<std::uint16_t>(DataDescriptionIndex::SetpointCulturalPractice):
			{
				switch (value)
				{
					case 0:
						return "Unknown";
					case 1:
						return "Fertilizing";
					case 2:
						return "Sowing and Planting";
					case 3:
						return "Crop Protection";
					case 4:
						return "Tillage";
					case 5:
						return "Baling (Pressing)";
					case 6:
						return "Mowing";
					case 7:
						return "Wrapping";
					case 8:
						return "Harvesting";
					case 9:
						return "Forage Harvesting";
					case 10:
						return "Transport";
					case 11:
						return "Swathing";
					case 12:
						return "Slurry/Manure Application";
					case 13:
						return "Self-Loading Wagon";
					case 14:
						return "Tedding";
					case 15:
						return "Measuring";
					case 16:
						return "Irrigation";
					case 17:
						return "Feeding/Mixing";
					case 18:
						return "Mulching";
					default:
						return "Reserved for future Assignment";
				}
			}
			case static_cast<std::uint16_t>(DataDescriptionIndex::SkyConditions):
			{
				if (value == 0x00000000)
				{
					return "Error: Sky condition data invalid";
				}
				if (value == static_cast<std::int32_t>(0xFFFFFFFF))
				{
					return "Not available";
				}

				// Extract each byte (character)
				std::string condition = "";
				for (int i = 3; i >= 0; --i)
				{
					char byte = (value >> (i * 8)) & 0xFF;
					if (byte != ' ')
					{
						condition += byte;
					}
				}

				static const std::map<std::string, std::string> metarDescriptions = {
					{ "CLR", "Clear" },
					{ "NSC", "Mostly Sunny" },
					{ "FEW", "Partly Sunny" },
					{ "SCT", "Partly Cloudy" },
					{ "BKN", "Mostly Cloudy" },
					{ "OVC", "Overcast/Cloudy" }
				};

				// Find the description for the condition
				auto it = metarDescriptions.find(condition);
				if (it != metarDescriptions.end())
				{
					return it->second;
				}

				return "Unknown sky condition";
			}
			case static_cast<std::uint16_t>(DataDescriptionIndex::NetWeightState):
			case static_cast<std::uint16_t>(DataDescriptionIndex::GrossWeightState):
			{
				switch (value & 0x03)
				{
					case 0x00:
						return "unstable measurement";
					case 0x01:
						return "stable measurement";
					case 0x02:
						return "measuring error";
					default:
						return "unknown";
				}
			}
			case static_cast<std::uint16_t>(DataDescriptionIndex::ActualUnLoadingSystemStatus):
			{
				std::ostringstream oss;
				oss << "Unloading: ";
				switch (value & 0x03)
				{
					case 0x00:
						oss << "Off";
						break;
					case 0x01:
						oss << "On";
						break;
					case 0x02:
						oss << "Error";
						break;
					case 0x03:
						oss << "Not installed";
						break;
					default:
						oss << "Unknown";
						break;
				}

				oss << ", Loading: ";
				switch ((value >> 8) & 0x03)
				{
					case 0x00:
						oss << "Off";
						break;
					case 0x01:
						oss << "On";
						break;
					case 0x02:
						oss << "Error";
						break;
					case 0x03:
						oss << "Not installed";
						break;
					default:
						oss << "Unknown";
						break;
				}

				return oss.str();
			}
			case static_cast<std::uint16_t>(DataDescriptionIndex::FunctionOrOperationTechnique):
			{
				// Based on ISO11783-11-DDI-350-Function and Operation Technique Type-v8.pdf
				auto enumeration = static_cast<std::uint16_t>(value & 0xFFFF);
				auto subType = static_cast<std::uint16_t>((value >> 16) & 0xFFFF);

				std::ostringstream oss;
				oss << "Function: ";
				switch (enumeration)
				{
					case 0:
						oss << "Unknown";
						break;
					case 1:
						oss << "Header";
						break;
					case 2:
						oss << "Auger";
						break;
					case 3:
						oss << "Separator";
						break;
					case 4:
						oss << "Sensor";
						break;
					case 5:
						oss << "Tillage";
						break;
					case 6:
						oss << "Baling (Pressing)";
						break;
					// 7..57343  => Reserved for future ISO assignment
					// 57344..65534 => Manufacturer specific (Proprietary)
					case 65535:
						oss << "Reserved";
						break;
					default:
					{
						if (enumeration >= 7 && enumeration <= 57343)
							oss << "Reserved (ISO)";
						else if (enumeration >= 57344 && enumeration <= 65534)
							oss << "Manufacturer Proprietary";
						else
							oss << "Invalid or Out of Range";
					}
					break;
				}

				oss << ", SubType: " << subType;
				switch (enumeration)
				{
					case 0: // Unknown => subType = 0xFF (per spec says "FF"? or 65535?
						oss << "(N/A)";
						break;

					case 1: // Header
						switch (subType)
						{
							case 0:
								oss << "Unknown (default)";
								break;
							case 1:
								oss << "Row-independent";
								break;
							case 2:
								oss << "Row-crop";
								break;
							case 3:
								oss << "Crop-pick-up";
								break;
							// 4..57343 => Reserved
							default:
							{
								if (subType >= 4 && subType <= 57343)
									oss << "Reserved";
								else
									oss << "Out of Range";
							}
							break;
						}
						break;

					case 2: // Auger
						switch (subType)
						{
							case 0:
								oss << "Unknown (default)";
								break;
							// 1..57343 => Reserved
							default:
							{
								if (subType >= 1 && subType <= 57343)
									oss << "Reserved";

								else
									oss << "Out of Range";
							}
							break;
						}
						break;

					case 3: // Separator
						switch (subType)
						{
							case 0:
								oss << "Unknown (default)";
								break;
							// 1..57343 => Reserved
							default:
							{
								if (subType >= 1 && subType <= 57343)
									oss << "Reserved";
								else
									oss << "Out of Range";
							}
							break;
						}

					case 4: // Sensor
						switch (subType)
						{
							case 0:
								oss << "Unknown (default)";
								break;
							case 1:
								oss << "Yield";
								break;
							case 2:
								oss << "Moisture";
								break;
							case 3:
								oss << "Humidity";
								break;
							case 4:
								oss << "Temperature";
								break;
							case 5:
								oss << "Wind";
								break;
							case 6:
								oss << "Height";
								break;
							case 7:
								oss << "Load";
								break;
							// 8..57343 => Reserved
							default:
							{
								if (subType >= 8 && subType <= 57343)
									oss << "Reserved";
								else
									oss << "Out of Range";
							}
							break;
						}
						break;

					case 5: // Tillage
						switch (subType)
						{
							case 0:
								oss << "Unknown (default)";
								break;
							case 1:
								oss << "Disk";
								break;
							case 2:
								oss << "Ripper";
								break;
							case 3:
								oss << "Closing Disk";
								break;
							case 4:
								oss << "Shank";
								break;
							case 5:
								oss << "Opener";
								break;
							case 6:
								oss << "Basket";
								break;
							case 7:
								oss << "Coulter";
								break;
							case 8:
								oss << "Harrow";
								break;
							case 9:
								oss << "Roller";
								break;
							// 10..57343 => Reserved
							default:
							{
								if (subType >= 10 && subType <= 57343)
									oss << "Reserved";
								else
									oss << "Out of Range";
							}
							break;
						}
						break;

					case 6: // Baling (Pressing)
						switch (subType)
						{
							case 0:
								oss << "Unknown (default)";
								break;
							case 1:
								oss << "Square Bale";
								break;
							case 2:
								oss << "Round Bale";
								break;
							case 3:
								oss << "Pellets";
								break;
							// 4..57343 => Reserved
							default:
							{
								if (subType >= 4 && subType <= 57343)
									oss << "Reserved";
								else
									oss << "Out of Range";
							}
							break;
						}
						break;

					// 7..57343 => Reserved (ISO), 57344..65534 => Proprietary, 65535 => reserved
					// For all these, subType is not specifically enumerated, so we treat subType similarly.
					default:
					{
						if (enumeration >= 7 && enumeration <= 57343)
							oss << "Reserved (ISO)";
						else if (enumeration >= 57344 && enumeration <= 65534)
							oss << "Manufacturer Proprietary";
						else
							oss << "Invalid or Out of Range";
					}
					break;
				}

				return oss.str();
			}
			case static_cast<std::uint16_t>(DataDescriptionIndex::SpeedSource):
			{
				switch (static_cast<std::uint8_t>(value))
				{
					case 0:
						return "Unknown";
					case 1:
						return "Wheel-based speed";
					case 2:
						return "Ground-based speed";
					case 3:
						return "Navigation-based speed";
					case 4:
						return "Blended speed";
					case 5:
						return "Simulated speed";
					case 6:
						return "Machine Selected speed";
					case 7:
						return "Machine measured speed";
					case 255:
						return "No Source available";
					default:
						return "Reserved";
				}
			}
			case static_cast<std::uint16_t>(DataDescriptionIndex::SoilSnowFrozenCondition):
			{
				// based on ISO11783-11-DDI-468-Soil Snow Frozen Condition-v1.pdf
				switch (value)
				{
					case 0x00:
						return "Not defined";
					case 0x01:
						return "Not frozen or snow-covered";
					case 0x02:
						return "Frozen";
					case 0x03:
						return "Snow-covered";
					default:
						return "Reserved";
				}
			}
			case static_cast<std::uint16_t>(DataDescriptionIndex::EstimatedSoilWaterCondition):
			{
				// based on ISO11783-11-DDI-469-Estimated Soil Water Condition-v1.pdf
				switch (value)
				{
					case 0x00:
						return "Not defined";
					case 0x01:
						return "Dry";
					case 0x02:
						return "Moist";
					case 0x03:
						return "Very wet";
					case 0x04:
						return "Saturated";
					case 0x05:
						return "Saturated";
					case 0x06:
						return "Inundated";
					default:
						return "Unknown";
				}
			}
			case static_cast<std::uint16_t>(DataDescriptionIndex::SoilCompaction):
			{
				// based on ISO11783-11-DDI-470-Soil Compaction-v1.pdf
				switch (value)
				{
					case 0x00:
						return "Not defined";
					case 0x01:
						return "Loose";
					case 0x02:
						return "Slightly compacted";
					case 0x03:
						return "Compacted";
					case 0x04:
						return "Very compacted";
					default:
						return "Reserved";
				}
			}
			case static_cast<std::uint16_t>(DataDescriptionIndex::TramlineControlLevel):
			case static_cast<std::uint16_t>(DataDescriptionIndex::SetpointTramlineControlLevel):
			{
				// based on TramlineControl_BasicRequirements_v1.14-v1.pdf
				switch (value)
				{
					case 0x00:
						return "Level 1";
					case 0x01:
						return "Level 2";
					case 0x02:
						return "Level 3";
					default:
						return "Reserved";
				}
			}
			case static_cast<std::uint16_t>(DataDescriptionIndex::GNSSQuality):
			{
				switch (value)
				{
					case 0x00:
						return "No GNSS";
					case 0x01:
						return "GNSS Fix";
					case 0x02:
						return "DGNSS Fix";
					case 0x03:
						return "Precise GNSS";
					case 0x04:
						return "RTK Fixed Integer";
					case 0x05:
						return "RTK Float";
					case 0x06:
						return "Estimated Mode";
					case 0x07:
						return "Manual Input";
					case 0x08:
						return "Simulate Mode";
					case 0x0F:
						return "Null";
					default:
						return "Reserved";
				}
			}
			case static_cast<std::uint16_t>(DataDescriptionIndex::GNSSInstallationType):
			{
				switch (value)
				{
					case 0x00:
						return "Unknown";
					case 0x01:
						return "Tractor integrated antenna";
					case 0x02:
						return "Tractor universal antenna (removable)";
					case 0x03:
						return "First implement antenna";
					case 0x04:
						return "Second implement antenna";
					case 0x05:
						return "Display integrated antenna";
					default:
						return "Reserved";
				}
			}
			case static_cast<std::uint16_t>(DataDescriptionIndex::PresentWeatherConditions):
			{
				// based on ISO11783-11-DDI-556-Present Weather Conditions-v1.pdf
				switch (value)
				{
					case 0x00:
						return "not defined";
					case 0x01:
						return "sunny";
					case 0x02:
						return "partly cloudy";
					case 0x03:
						return "overcast";
					case 0x04:
						return "rain";
					case 0x05:
						return "sleet";
					case 0x06:
						return "snow";
					default:
						return "reserved";
				}
			}
			case static_cast<std::uint16_t>(DataDescriptionIndex::PreviousRainfall):
			{
				// based on Previous Rainfall-v1.pdf
				switch (value)
				{
					case 0x00:
						return "not defined";
					case 0x01:
						return "no rain in the last month";
					case 0x02:
						return "no rain in the last week";
					case 0x03:
						return "no rain in the last 24 hours";
					case 0x04:
						return "rainy without heavy rain in the last 24 hours";
					case 0x05:
						return "heavier rain for some days or rainstorm in the last 24 hours";
					case 0x06:
						return "prolonged rainfall or snowmelt";
					default:
						return "reserved";
				}
			}
			case static_cast<std::uint16_t>(DataDescriptionIndex::TractionType):
			{
				switch (value)
				{
					case 0x00:
						return "Unknown";
					case 0x01:
						return "Two track";
					case 0x02:
						return "Four track";
					case 0x03:
						return "Wheel";
					case 0x04:
						return "Front Track, Rear wheel";
					case 0x05:
						return "Front Wheel, Rear Track";
					default:
						return "Reserved";
				}
			}
			case static_cast<std::uint16_t>(DataDescriptionIndex::SteeringType):
			{
				switch (value)
				{
					case 0x00:
						return "Unknown";
					case 0x01:
						return "Articulated";
					case 0x02:
						return "Differential";
					case 0x03:
						return "Front wheel";
					case 0x04:
						return "Rear wheel";
					case 0x05:
						return "Four wheel";
					case 0x06:
						return "Differential Front with Active Rear support";
					case 0x07:
						return "Dog-walk Machine";
					default:
						return "Reserved";
				}
			}
			case static_cast<std::uint16_t>(DataDescriptionIndex::MachineMode):
			{
				switch (value)
				{
					case 0x00:
						return "Unknown / Not defined";
					case 0x01:
						return "Idle";
					case 0x02:
						return "Field Mode";
					case 0x03:
						return "Street Mode";
					case 0x04:
						return "Maintenance";
					case 0x05:
						return "Filling";
					case 0x06:
						return "Emptying";
					case 0x07:
						return "Cleaning";
					default:
						return "Reserved";
				}
			}
			case static_cast<std::uint16_t>(DataDescriptionIndex::BindingMethod):
			{
				switch (value)
				{
					case 0x00:
						return "Unknown";
					case 0x01:
						return "Mesh";
					case 0x02:
						return "Twine";
					case 0x03:
						return "Film";
					case 0x04:
						return "Twine & Mesh";
					case 0x05:
						return "Twine & Film";
					default:
						return "Reserved";
				}
			}
			case static_cast<std::uint16_t>(DataDescriptionIndex::WorkingDirection):
			{
				switch (value)
				{
					case 0x00:
						return "unknown";
					case 0x01:
						return "left to right";
					case 0x02:
						return "right to left";
					default:
						return "reserved";
				}
			}
			case static_cast<std::uint16_t>(DataDescriptionIndex::MaximumDropletSize):
			case static_cast<std::uint16_t>(DataDescriptionIndex::MinimumDropletSize):
			case static_cast<std::uint16_t>(DataDescriptionIndex::DefaultDropletSize):
			case static_cast<std::uint16_t>(DataDescriptionIndex::ActualDropletSize):
			case static_cast<std::uint16_t>(DataDescriptionIndex::SetpointDropletSize):
			{
				switch (value)
				{
					case 0x00:
						return "Unknown";
					case 0x01:
						return "Extremely fine";
					case 0x02:
						return "Very fine";
					case 0x03:
						return "Fine";
					case 0x04:
						return "Medium";
					case 0x05:
						return "Coarse";
					case 0x06:
						return "Very coarse";
					case 0x07:
						return "Extremely coarse";
					case 0x08:
						return "Ultra coarse";
					default:
						return "Reserved";
				}
			}
			default:
				break;
		}
#endif

		// No special formatting, apply offset, resolution and units
		double offset = displayRange.first - (static_cast<double>(std::numeric_limits<std::int32_t>::min()) * resolution);
		double scaledValue = static_cast<double>(value) * resolution + offset;
		std::string valueString = std::to_string(scaledValue);
		size_t end = valueString.find_last_not_of('0'); // Find the last non-zero character
		if (valueString[end] == '.')
		{
			--end; // Avoid leaving a dangling decimal point
		}
		return valueString.substr(0, end + 1) + unitSymbol;
	}

	std::string DataDictionary::ddi_to_string(std::uint16_t ddi)
	{
#ifdef DISABLE_ISOBUS_DATA_DICTIONARY
		return std::to_string(ddi);
#else
		const Entry &entry = get_entry(ddi);
		return entry.name;
#endif
	}

	std::string isobus::DataDictionary::format_value_with_ddi(std::uint16_t ddi, std::int32_t value)
	{
#ifdef DISABLE_ISOBUS_DATA_DICTIONARY
		return std::to_string(value);
#else
		const Entry &entry = get_entry(ddi);
		return entry.format_value(value);
#endif
	}

	const DataDictionary::Entry DataDictionary::DEFAULT_ENTRY = { 65535, "Unknown", "Unknown", "Unknown", 0.0f, std::make_pair(0.0f, 0.0f) };

#ifndef DISABLE_ISOBUS_DATA_DICTIONARY
	// The table below is auto-generated, and is not to be edited manually.
	const DataDictionary::Entry DataDictionary::DDI_ENTRIES[] = {
		{ 0, "Internal Data Base DDI", "", "n.a.", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 1, "Setpoint Volume Per Area Application Rate as [mm³/m²]", "mm³/m²", "Capacity per area unit", 0.01f, std::make_pair(0.00f, 21474836.47f) },
		{ 2, "Actual Volume Per Area Application Rate as [mm³/m²]", "mm³/m²", "Capacity per area unit", 0.01f, std::make_pair(0.00f, 21474836.47f) },
		{ 3, "Default Volume Per Area Application Rate as [mm³/m²]", "mm³/m²", "Capacity per area unit", 0.01f, std::make_pair(0.00f, 21474836.47f) },
		{ 4, "Minimum Volume Per Area Application Rate as [mm³/m²]", "mm³/m²", "Capacity per area unit", 0.01f, std::make_pair(0.00f, 21474836.47f) },
		{ 5, "Maximum Volume Per Area Application Rate as [mm³/m²]", "mm³/m²", "Capacity per area unit", 0.01f, std::make_pair(0.00f, 21474836.47f) },
		{ 6, "Setpoint Mass Per Area Application Rate", "mg/m²", "Mass per area unit", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 7, "Actual Mass Per Area Application Rate", "mg/m²", "Mass per area unit", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 8, "Default Mass Per Area Application Rate", "mg/m²", "Mass per area unit", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 9, "Minimum Mass Per Area Application Rate", "mg/m²", "Mass per area unit", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 10, "Maximum Mass Per Area Application Rate", "mg/m²", "Mass per area unit", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 11, "Setpoint Count Per Area Application Rate", "/m²", "Quantity per area unit", 0.001f, std::make_pair(0.000f, 2147483.647f) },
		{ 12, "Actual Count Per Area Application Rate", "/m²", "Quantity per area unit", 0.001f, std::make_pair(0.000f, 2147483.647f) },
		{ 13, "Default Count Per Area Application Rate", "/m²", "Quantity per area unit", 0.001f, std::make_pair(0.000f, 2147483.647f) },
		{ 14, "Minimum Count Per Area Application Rate", "/m²", "Quantity per area unit", 0.001f, std::make_pair(0.000f, 2147483.647f) },
		{ 15, "Maximum Count Per Area Application Rate", "/m²", "Quantity per area unit", 0.001f, std::make_pair(0.000f, 2147483.647f) },
		{ 16, "Setpoint Spacing Application Rate", "mm", "Length", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 17, "Actual Spacing Application Rate", "mm", "Length", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 18, "Default Spacing Application Rate", "mm", "Length", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 19, "Minimum Spacing Application Rate", "mm", "Length", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 20, "Maximum Spacing Application Rate", "mm", "Length", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 21, "Setpoint Volume Per Volume Application Rate", "mm³/m³", "Capacity per capacity unit", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 22, "Actual Volume Per Volume Application Rate", "mm³/m³", "Capacity per capacity unit", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 23, "Default Volume Per Volume Application Rate", "mm³/m³", "Capacity per capacity unit", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 24, "Minimum Volume Per Volume Application Rate", "mm³/m³", "Capacity per capacity unit", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 25, "Maximum Volume Per Volume Application Rate", "mm³/m³", "Capacity per capacity unit", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 26, "Setpoint Mass Per Mass Application Rate", "mg/kg", "Mass per mass unit", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 27, "Actual Mass Per Mass Application Rate", "mg/kg", "Mass per mass unit", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 28, "Default Mass Per Mass Application Rate", "mg/kg", "Mass per mass unit", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 29, "Minimum Mass Per Mass Application Rate", "mg/kg", "Mass per mass unit", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 30, "MaximumMass Per Mass Application Rate", "mg/kg", "Mass per mass unit", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 31, "Setpoint Volume Per Mass Application Rate", "mm³/kg", "Capacity per mass unit", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 32, "Actual Volume Per Mass Application Rate", "mm³/kg", "Capacity per mass unit", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 33, "Default Volume Per Mass Application Rate", "mm³/kg", "Capacity per mass unit", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 34, "Minimum Volume Per Mass Application Rate", "mm³/kg", "Capacity per mass unit", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 35, "Maximum Volume Per Mass Application Rate", "mm³/kg", "Capacity per mass unit", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 36, "Setpoint Volume Per Time Application Rate", "mm³/s", "Flow", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 37, "Actual Volume Per Time Application Rate", "mm³/s", "Flow", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 38, "Default Volume Per Time Application Rate", "mm³/s", "Flow", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 39, "Minimum Volume Per Time Application Rate", "mm³/s", "Flow", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 40, "Maximum Volume Per Time Application Rate", "mm³/s", "Flow", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 41, "Setpoint Mass Per Time Application Rate", "mg/s", "Mass flow", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 42, "Actual Mass Per Time Application Rate", "mg/s", "Mass flow", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 43, "Default Mass Per Time Application Rate", "mg/s", "Mass flow", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 44, "Minimum Mass Per Time Application Rate", "mg/s", "Mass flow", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 45, "Maximum Mass Per Time Application Rate", "mg/s", "Mass flow", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 46, "Setpoint Count Per Time Application Rate", "/s", "Quantity per time unit", 0.001f, std::make_pair(0.000f, 2147483.647f) },
		{ 47, "Actual Count Per Time Application Rate", "/s", "Quantity per time unit", 0.001f, std::make_pair(0.000f, 2147483.647f) },
		{ 48, "Default Count Per Time Application Rate", "/s", "Quantity per time unit", 0.001f, std::make_pair(0.000f, 2147483.647f) },
		{ 49, "Minimum Count Per Time Application Rate", "/s", "Quantity per time unit", 0.001f, std::make_pair(0.000f, 2147483.647f) },
		{ 50, "Maximum Count Per Time Application Rate", "/s", "Quantity per time unit", 0.001f, std::make_pair(0.000f, 2147483.647f) },
		{ 51, "Setpoint Tillage Depth", "mm", "Length", 1.0f, std::make_pair(-2147483648.0f, 2147483647.0f) },
		{ 52, "Actual Tillage Depth", "mm", "Length", 1.0f, std::make_pair(-2147483648.0f, 2147483647.0f) },
		{ 53, "Default Tillage Depth", "mm", "Length", 1.0f, std::make_pair(-2147483648.0f, 2147483647.0f) },
		{ 54, "Minimum Tillage Depth", "mm", "Length", 1.0f, std::make_pair(-2147483648.0f, 2147483647.0f) },
		{ 55, "Maximum Tillage Depth", "mm", "Length", 1.0f, std::make_pair(-2147483648.0f, 2147483647.0f) },
		{ 56, "Setpoint Seeding Depth", "mm", "Length", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 57, "Actual Seeding Depth", "mm", "Length", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 58, "Default Seeding Depth", "mm", "Length", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 59, "Minimum Seeding Depth", "mm", "Length", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 60, "Maximum Seeding Depth", "mm", "Length", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 61, "Setpoint Working Height", "mm", "Length", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 62, "Actual Working Height", "mm", "Length", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 63, "Default Working Height", "mm", "Length", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 64, "Minimum Working Height", "mm", "Length", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 65, "Maximum Working Height", "mm", "Length", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 66, "Setpoint Working Width", "mm", "Length", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 67, "Actual Working Width", "mm", "Length", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 68, "Default Working Width", "mm", "Length", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 69, "Minimum Working Width", "mm", "Length", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 70, "Maximum Working Width", "mm", "Length", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 71, "Setpoint Volume Content", "ml", "Capacity large", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 72, "Actual Volume Content", "ml", "Capacity large", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 73, "Maximum Volume Content", "ml", "Capacity large", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 74, "Setpoint Mass Content", "g", "Mass large", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 75, "Actual Mass Content", "g", "Mass large", 1.0f, std::make_pair(-2147483648.0f, 2147483647.0f) },
		{ 76, "Maximum Mass Content", "g", "Mass large", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 77, "Setpoint Count Content", "#", "Quantity/Count", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 78, "Actual Count Content", "#", "Quantity/Count", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 79, "Maximum Count Content", "#", "Quantity/Count", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 80, "Application Total Volume as [L]", "L", "Capacity count", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 81, "Application Total Mass in [kg]", "kg", "Mass", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 82, "Application Total Count", "#", "Quantity/Count", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 83, "Volume Per Area Yield", "ml/m²", "Capacity per area large", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 84, "Mass Per Area Yield", "mg/m²", "Mass per area unit", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 85, "Count Per Area Yield", "/m²", "Quantity per area unit", 0.001f, std::make_pair(0.000f, 2147483.647f) },
		{ 86, "Volume Per Time Yield", "ml/s", "Float large", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 87, "Mass Per Time Yield", "mg/s", "Mass flow", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 88, "Count Per Time Yield", "/s", "Quantity per time unit", 0.001f, std::make_pair(0.000f, 2147483.647f) },
		{ 89, "Yield Total Volume", "L", "Quantity per volume", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 90, "Yield Total Mass", "kg", "Mass", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 91, "Yield Total Count", "#", "Quantity/Count", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 92, "Volume Per Area Crop Loss", "ml/m²", "Capacity per area large", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 93, "Mass Per Area Crop Loss", "mg/m²", "Mass per area unit", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 94, "Count Per Area Crop Loss", "/m²", "Quantity per area unit", 0.001f, std::make_pair(0.000f, 2147483.647f) },
		{ 95, "Volume Per Time Crop Loss", "ml/s", "Float large", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 96, "Mass Per Time Crop Loss", "mg/s", "Mass flow", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 97, "Count Per Time Crop Loss", "/s", "Quantity per time unit", 0.001f, std::make_pair(0.000f, 2147483.647f) },
		{ 98, "Percentage Crop Loss", "ppm", "Parts per million", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 99, "Crop Moisture", "ppm", "Parts per million", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 100, "Crop Contamination", "ppm", "Parts per million", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 101, "Setpoint Bale Width", "mm", "Length", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 102, "Actual Bale Width", "mm", "Length", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 103, "Default Bale Width", "mm", "Length", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 104, "Minimum Bale Width", "mm", "Length", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 105, "Maximum Bale Width", "mm", "Length", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 106, "Setpoint Bale Height", "mm", "Length", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 107, "Actual Bale Height", "mm", "Length", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 108, "Default Bale Height", "mm", "Length", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 109, "Minimum Bale Height", "mm", "Length", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 110, "Maximum Bale Height", "mm", "Length", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 111, "Setpoint Bale Size", "mm", "Length", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 112, "Actual Bale Size", "mm", "Length", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 113, "Default Bale Size", "mm", "Length", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 114, "Minimum Bale Size", "mm", "Length", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 115, "Maximum Bale Size", "mm", "Length", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 116, "Total Area", "m²", "Area", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 117, "Effective Total Distance", "mm", "Length", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 118, "Ineffective Total Distance", "mm", "Length", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 119, "Effective Total Time", "s", "Time count", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 120, "Ineffective Total Time", "s", "Time count", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 121, "Product Density Mass Per Volume", "mg/l", "Mass per capacity unit", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 122, "Product Density Mass PerCount", "mg/1000", "1000 seed Mass", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 123, "Product Density Volume Per Count", "ml/1000", "Volume per quantity unit", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 124, "Auxiliary Valve Scaling Extend", "%", "Percent", 0.1f, std::make_pair(-214748364.8f, 214748364.7f) },
		{ 125, "Auxiliary Valve Scaling Retract", "%", "Percent", 0.1f, std::make_pair(-214748364.8f, 214748364.7f) },
		{ 126, "Auxiliary Valve Ramp Extend Up", "ms", "Time", 1.0f, std::make_pair(-2147483648.0f, 2147483647.0f) },
		{ 127, "Auxiliary Valve Ramp Extend Down", "ms", "Time", 1.0f, std::make_pair(-2147483648.0f, 2147483647.0f) },
		{ 128, "Auxiliary Valve Ramp Retract Up", "ms", "Time", 1.0f, std::make_pair(-2147483648.0f, 2147483647.0f) },
		{ 129, "Auxiliary Valve Ramp Retract Down", "ms", "Time", 1.0f, std::make_pair(-2147483648.0f, 2147483647.0f) },
		{ 130, "Auxiliary Valve Float Threshold", "%", "Percent", 0.1f, std::make_pair(-214748364.8f, 214748364.7f) },
		{ 131, "Auxiliary Valve Progressivity Extend", "", "n.a.", 1.0f, std::make_pair(-2147483648.0f, 2147483647.0f) },
		{ 132, "Auxiliary Valve Progressivity Retract", "", "n.a.", 1.0f, std::make_pair(-2147483648.0f, 2147483647.0f) },
		{ 133, "Auxiliary Valve Invert Ports", "", "n.a.", 1.0f, std::make_pair(-2147483648.0f, 2147483647.0f) },
		{ 134, "Device Element Offset X", "mm", "Length", 1.0f, std::make_pair(-2147483648.0f, 2147483647.0f) },
		{ 135, "Device Element Offset Y", "mm", "Length", 1.0f, std::make_pair(-2147483648.0f, 2147483647.0f) },
		{ 136, "Device Element Offset Z", "mm", "Length", 1.0f, std::make_pair(-2147483648.0f, 2147483647.0f) },
		{ 137, "Device Volume Capacity (Deprecated)", "ml", "Capacity large", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 138, "Device Mass Capacity (Deprecated)", "g", "Mass large", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 139, "Device Count Capacity (Deprecated)", "#", "Quantity/Count", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 140, "Setpoint Percentage Application Rate", "ppm", "Parts per million", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 141, "Actual Work State", "", "n.a.", 1.0f, std::make_pair(0.0f, 3.0f) },
		{ 142, "Physical Setpoint Time Latency", "ms", "Time", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 143, "Physical Actual Value Time Latency", "ms", "Time", 1.0f, std::make_pair(-2147483648.0f, 2147483647.0f) },
		{ 144, "Yaw Angle", "°", "Angle", 0.001f, std::make_pair(-180.000f, 180.000f) },
		{ 145, "Roll Angle", "°", "Angle", 0.001f, std::make_pair(-180.000f, 180.000f) },
		{ 146, "Pitch Angle", "°", "Angle", 0.001f, std::make_pair(-180.000f, 180.000f) },
		{ 147, "Log Count", "", "n.a.", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 148, "Total Fuel Consumption", "ml", "Capacity large", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 149, "Instantaneous Fuel Consumption per Time", "mm³/s", "Flow", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 150, "Instantaneous Fuel Consumption per Area", "mm³/m²", "Capacity per area unit", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 151, "Instantaneous Area Per Time Capacity", "mm²/s", "Area per time unit", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 153, "Actual Normalized Difference Vegetative Index (NDVI)", "", "n.a.", 0.001f, std::make_pair(-1.000f, 1.000f) },
		{ 154, "Physical Object Length", "mm", "Length", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 155, "Physical Object Width", "mm", "Length", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 156, "Physical Object Height", "mm", "Length", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 157, "Connector Type", "", "n.a.", 1.0f, std::make_pair(-1.0f, 10.0f) },
		{ 158, "Prescription Control State", "", "n.a.", 1.0f, std::make_pair(0.0f, 3.0f) },
		{ 159, "Number of Sub-Units per Section", "#", "Quantity/Count", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 160, "Section Control State", "", "n.a.", 1.0f, std::make_pair(0.0f, 3.0f) },
		{ 161, "Actual Condensed Work State (1-16)", "", "n.a.", 1.0f, std::make_pair(0.0f, 4294967295.0f) },
		{ 162, "Actual Condensed Work State (17-32)", "", "n.a.", 1.0f, std::make_pair(0.0f, 4294967295.0f) },
		{ 163, "Actual Condensed Work State (33-48)", "", "n.a.", 1.0f, std::make_pair(0.0f, 4294967295.0f) },
		{ 164, "Actual Condensed Work State (49-64)", "", "n.a.", 1.0f, std::make_pair(0.0f, 4294967295.0f) },
		{ 165, "Actual Condensed Work State (65-80)", "", "n.a.", 1.0f, std::make_pair(0.0f, 4294967295.0f) },
		{ 166, "Actual Condensed Work State (81-96)", "", "n.a.", 1.0f, std::make_pair(0.0f, 4294967295.0f) },
		{ 167, "Actual Condensed Work State (97-112)", "", "n.a.", 1.0f, std::make_pair(0.0f, 4294967295.0f) },
		{ 168, "Actual Condensed Work State (113-128)", "", "n.a.", 1.0f, std::make_pair(0.0f, 4294967295.0f) },
		{ 169, "Actual Condensed Work State (129-144)", "", "n.a.", 1.0f, std::make_pair(0.0f, 4294967295.0f) },
		{ 170, "Actual Condensed Work State (145-160)", "", "n.a.", 1.0f, std::make_pair(0.0f, 4294967295.0f) },
		{ 171, "Actual Condensed Work State (161-176)", "", "n.a.", 1.0f, std::make_pair(0.0f, 4294967295.0f) },
		{ 172, "Actual Condensed Work State (177-192)", "", "n.a.", 1.0f, std::make_pair(0.0f, 4294967295.0f) },
		{ 173, "Actual Condensed Work State (193-208)", "", "n.a.", 1.0f, std::make_pair(0.0f, 4294967295.0f) },
		{ 174, "Actual Condensed Work State (209-224)", "", "n.a.", 1.0f, std::make_pair(0.0f, 4294967295.0f) },
		{ 175, "Actual Condensed Work State (225-240)", "", "n.a.", 1.0f, std::make_pair(0.0f, 4294967295.0f) },
		{ 176, "Actual Condensed Work State (241-256)", "", "n.a.", 1.0f, std::make_pair(0.0f, 4294967295.0f) },
		{ 177, "Actual length of cut", "mm", "Length", 0.001f, std::make_pair(0.000f, 2147483.647f) },
		{ 178, "Element Type Instance", "", "n.a.", 1.0f, std::make_pair(0.0f, 65533.0f) },
		{ 179, "Actual Cultural Practice", "", "n.a.", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 180, "Device Reference Point (DRP) to Ground distance", "mm", "Length", 1.0f, std::make_pair(-2147483648.0f, 2147483647.0f) },
		{ 181, "Dry Mass Per Area Yield", "mg/m²", "Mass per area unit", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 182, "Dry Mass Per Time Yield", "mg/s", "Mass flow", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 183, "Yield Total Dry Mass", "kg", "Mass", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 184, "Reference Moisture For Dry Mass", "ppm", "Parts per million", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 185, "Seed Cotton Mass Per Area Yield", "mg/m²", "Mass per area unit", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 186, "Lint Cotton Mass Per Area Yield", "mg/m²", "Mass per area unit", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 187, "Seed Cotton Mass Per Time Yield", "mg/s", "Mass flow", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 188, "Lint Cotton Mass Per Time Yield", "mg/s", "Mass flow", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 189, "Yield Total Seed Cotton Mass", "kg", "Mass", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 190, "Yield Total Lint Cotton Mass", "kg", "Mass", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 191, "Lint Turnout Percentage", "ppm", "Parts per million", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 192, "Ambient temperature", "mK", "Temperature", 1.0f, std::make_pair(0.0f, 1000000.0f) },
		{ 193, "Setpoint Product Pressure", "Pa", "Pressure", 0.1f, std::make_pair(-214748364.8f, 214748364.7f) },
		{ 194, "Actual Product Pressure", "Pa", "Pressure", 0.1f, std::make_pair(-214748364.8f, 214748364.7f) },
		{ 195, "Minimum Product Pressure", "Pa", "Pressure", 0.1f, std::make_pair(-214748364.8f, 214748364.7f) },
		{ 196, "Maximum Product Pressure", "Pa", "Pressure", 0.1f, std::make_pair(-214748364.8f, 214748364.7f) },
		{ 197, "Setpoint Pump Output Pressure", "Pa", "Pressure", 0.1f, std::make_pair(-214748364.8f, 214748364.7f) },
		{ 198, "Actual Pump Output Pressure", "Pa", "Pressure", 0.1f, std::make_pair(-214748364.8f, 214748364.7f) },
		{ 199, "Minimum Pump Output Pressure", "Pa", "Pressure", 0.1f, std::make_pair(-214748364.8f, 214748364.7f) },
		{ 200, "Maximum Pump Output Pressure", "Pa", "Pressure", 0.1f, std::make_pair(-214748364.8f, 214748364.7f) },
		{ 201, "Setpoint Tank Agitation Pressure", "Pa", "Pressure", 0.1f, std::make_pair(-214748364.8f, 214748364.7f) },
		{ 202, "Actual Tank Agitation Pressure", "Pa", "Pressure", 0.1f, std::make_pair(-214748364.8f, 214748364.7f) },
		{ 203, "Minimum Tank Agitation Pressure", "Pa", "Pressure", 0.1f, std::make_pair(-214748364.8f, 214748364.7f) },
		{ 204, "Maximum Tank Agitation Pressure", "Pa", "Pressure", 0.1f, std::make_pair(-214748364.8f, 214748364.7f) },
		{ 205, "SC Setpoint Turn On Time", "ms", "Time", 1.0f, std::make_pair(-2147483648.0f, 2147483647.0f) },
		{ 206, "SC Setpoint Turn Off Time", "ms", "Time", 1.0f, std::make_pair(-2147483648.0f, 2147483647.0f) },
		{ 207, "Wind speed", "mm/s", "Speed", 1.0f, std::make_pair(0.0f, 100000000.0f) },
		{ 208, "Wind direction", "°", "Angle", 1.0f, std::make_pair(0.0f, 359.0f) },
		{ 209, "Relative Humidity", "%", "Percent", 1.0f, std::make_pair(0.0f, 100.0f) },
		{ 210, "Sky conditions", "", "n.a.", 1.0f, std::make_pair(0.0f, 4294967295.0f) },
		{ 211, "Last Bale Flakes per Bale", "#", "Quantity/Count", 1.0f, std::make_pair(0.0f, 1000.0f) },
		{ 212, "Last Bale Average Moisture", "ppm", "Parts per million", 1.0f, std::make_pair(0.0f, 100000000.0f) },
		{ 213, "Last Bale Average Strokes per Flake", "#", "Quantity/Count", 1.0f, std::make_pair(0.0f, 1000.0f) },
		{ 214, "Lifetime Bale Count", "#", "Quantity/Count", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 215, "Lifetime Working Hours", "h", "Hour", 0.05f, std::make_pair(0.00f, 210554060.75f) },
		{ 216, "Actual Bale Hydraulic Pressure", "Pa", "Pressure", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 217, "Last Bale Average Hydraulic Pressure", "Pa", "Pressure", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 218, "Setpoint Bale Compression Plunger Load", "ppm", "Parts per million", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 219, "Actual Bale Compression Plunger Load", "ppm", "Parts per million", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 220, "Last Bale Average Bale Compression Plunger Load", "ppm", "Parts per million", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 221, "Last Bale Applied Preservative", "ml", "Capacity large", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 222, "Last Bale Tag Number", "", "n.a.", 1.0f, std::make_pair(-2147483648.0f, 2147483647.0f) },
		{ 223, "Last Bale Mass", "g", "Mass large", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 224, "Delta T", "mK", "Temperature", 1.0f, std::make_pair(0.0f, 1000000.0f) },
		{ 225, "Setpoint Working Length", "mm", "Length", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 226, "Actual Working Length", "mm", "Length", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 227, "Minimum Working Length", "mm", "Length", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 228, "Maximum Working Length", "mm", "Length", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 229, "Actual Net Weight", "g", "Mass large", 1.0f, std::make_pair(-2147483648.0f, 2147483647.0f) },
		{ 230, "Net Weight State", "", "n.a.", 1.0f, std::make_pair(0.0f, 3.0f) },
		{ 231, "Setpoint Net Weight", "g", "Mass large", 1.0f, std::make_pair(-2147483648.0f, 2147483647.0f) },
		{ 232, "Actual Gross Weight", "g", "Mass large", 1.0f, std::make_pair(-2147483648.0f, 2147483647.0f) },
		{ 233, "Gross Weight State", "", "n.a.", 1.0f, std::make_pair(0.0f, 3.0f) },
		{ 234, "Minimum Gross Weight", "g", "Mass large", 1.0f, std::make_pair(-2147483648.0f, 2147483647.0f) },
		{ 235, "Maximum Gross Weight", "g", "Mass large", 1.0f, std::make_pair(-2147483648.0f, 2147483647.0f) },
		{ 236, "Thresher Engagement Total Time", "s", "Time count", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 237, "Actual Header Working Height Status", "", "n.a.", 1.0f, std::make_pair(0.0f, 3.0f) },
		{ 238, "Actual Header Rotational Speed Status", "", "n.a.", 1.0f, std::make_pair(0.0f, 3.0f) },
		{ 239, "Yield Hold Status", "", "n.a.", 1.0f, std::make_pair(0.0f, 3.0f) },
		{ 240, "Actual (Un)Loading System Status", "", "n.a.", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 241, "Crop Temperature", "mK", "Temperature", 1.0f, std::make_pair(0.0f, 1000000.0f) },
		{ 242, "Setpoint Sieve Clearance", "mm", "Length", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 243, "Actual Sieve Clearance", "mm", "Length", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 244, "Minimum Sieve Clearance", "mm", "Length", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 245, "Maximum Sieve Clearance", "mm", "Length", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 246, "Setpoint Chaffer Clearance", "mm", "Length", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 247, "Actual Chaffer Clearance", "mm", "Length", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 248, "Minimum Chaffer Clearance", "mm", "Length", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 249, "Maximum Chaffer Clearance", "mm", "Length", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 250, "Setpoint Concave Clearance", "mm", "Length", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 251, "Actual Concave Clearance", "mm", "Length", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 252, "Minimum Concave Clearance", "mm", "Length", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 253, "Maximum Concave Clearance", "mm", "Length", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 254, "Setpoint Separation Fan Rotational Speed", "/s", "Quantity per time unit", 0.001f, std::make_pair(0.000f, 2147483.647f) },
		{ 255, "Actual Separation Fan Rotational Speed", "/s", "Quantity per time unit", 0.001f, std::make_pair(0.000f, 2147483.647f) },
		{ 256, "Minimum Separation Fan Rotational Speed", "/s", "Quantity per time unit", 0.001f, std::make_pair(0.000f, 2147483.647f) },
		{ 257, "Maximum Separation Fan Rotational Speed", "/s", "Quantity per time unit", 0.001f, std::make_pair(0.000f, 2147483.647f) },
		{ 258, "Hydraulic Oil Temperature", "mK", "Temperature", 1.0f, std::make_pair(0.0f, 2000000.0f) },
		{ 259, "Yield Lag Ignore Time", "ms", "Time", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 260, "Yield Lead Ignore Time", "ms", "Time", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 261, "Average Yield Mass Per Time", "mg/s", "Mass flow", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 262, "Average Crop Moisture", "ppm", "Parts per million", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 263, "Average Yield Mass Per Area", "mg/m²", "Mass per area unit", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 264, "Connector Pivot X-Offset", "mm", "Length", 1.0f, std::make_pair(-2147483648.0f, 2147483647.0f) },
		{ 265, "Remaining Area", "m²", "Area", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 266, "Lifetime Application Total Mass", "kg", "Mass", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 267, "Lifetime Application Total Count", "#", "Quantity/Count", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 268, "Lifetime Yield Total Volume", "L", "Quantity per volume", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 269, "Lifetime Yield Total Mass", "kg", "Mass", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 270, "Lifetime Yield Total Count", "#", "Quantity/Count", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 271, "Lifetime Total Area", "m²", "Area", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 272, "Lifetime Effective Total Distance", "m", "Distance", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 273, "Lifetime Ineffective Total Distance", "m", "Distance", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 274, "Lifetime Effective Total Time", "s", "Time count", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 275, "Lifetime Ineffective Total Time", "s", "Time count", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 276, "Lifetime Fuel Consumption", "L", "Capacity count", 0.5f, std::make_pair(0.0f, 1073741823.5f) },
		{ 277, "Lifetime Average Fuel Consumption per Time", "mm³/s", "Flow", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 278, "Lifetime Average Fuel Consumption per Area", "mm³/m²", "Capacity per area unit", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 279, "Lifetime Yield Total Dry Mass", "kg", "Mass", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 280, "Lifetime Yield Total Seed Cotton Mass", "kg", "Mass", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 281, "Lifetime Yield Total Lint Cotton Mass", "kg", "Mass", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 282, "Lifetime Threshing Engagement Total Time", "s", "Time count", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 283, "Precut Total Count", "#", "Quantity/Count", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 284, "Uncut Total Count", "#", "Quantity/Count", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 285, "Lifetime Precut Total Count", "#", "Quantity/Count", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 286, "Lifetime Uncut Total Count", "#", "Quantity/Count", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 287, "Setpoint Prescription Mode", "", "n.a.", 1.0f, std::make_pair(0.0f, 6.0f) },
		{ 288, "Actual Prescription Mode", "", "n.a.", 1.0f, std::make_pair(0.0f, 5.0f) },
		{ 289, "Setpoint Work State", "", "n.a.", 1.0f, std::make_pair(0.0f, 3.0f) },
		{ 290, "Setpoint Condensed Work State (1-16)", "", "n.a.", 1.0f, std::make_pair(0.0f, 4294967295.0f) },
		{ 291, "Setpoint Condensed Work State (17-32)", "", "n.a.", 1.0f, std::make_pair(0.0f, 4294967295.0f) },
		{ 292, "Setpoint Condensed Work State (33-48)", "", "n.a.", 1.0f, std::make_pair(0.0f, 4294967295.0f) },
		{ 293, "Setpoint Condensed Work State (49-64)", "", "n.a.", 1.0f, std::make_pair(0.0f, 4294967295.0f) },
		{ 294, "Setpoint Condensed Work State (65-80)", "", "n.a.", 1.0f, std::make_pair(0.0f, 4294967295.0f) },
		{ 295, "Setpoint Condensed Work State (81-96)", "", "n.a.", 1.0f, std::make_pair(0.0f, 4294967295.0f) },
		{ 296, "Setpoint Condensed Work State (97-112)", "", "n.a.", 1.0f, std::make_pair(0.0f, 4294967295.0f) },
		{ 297, "Setpoint Condensed Work State (113-128)", "", "n.a.", 1.0f, std::make_pair(0.0f, 4294967295.0f) },
		{ 298, "Setpoint Condensed Work State (129-144)", "", "n.a.", 1.0f, std::make_pair(0.0f, 4294967295.0f) },
		{ 299, "Setpoint Condensed Work State (145-160)", "", "n.a.", 1.0f, std::make_pair(0.0f, 4294967295.0f) },
		{ 300, "Setpoint Condensed Work State (161-176)", "", "n.a.", 1.0f, std::make_pair(0.0f, 4294967295.0f) },
		{ 301, "Setpoint Condensed Work State (177-192)", "", "n.a.", 1.0f, std::make_pair(0.0f, 4294967295.0f) },
		{ 302, "Setpoint Condensed Work State (193-208)", "", "n.a.", 1.0f, std::make_pair(0.0f, 4294967295.0f) },
		{ 303, "Setpoint Condensed Work State (209-224)", "", "n.a.", 1.0f, std::make_pair(0.0f, 4294967295.0f) },
		{ 304, "Setpoint Condensed Work State (225-240)", "", "n.a.", 1.0f, std::make_pair(0.0f, 4294967295.0f) },
		{ 305, "Setpoint Condensed Work State (241-256)", "", "n.a.", 1.0f, std::make_pair(0.0f, 4294967295.0f) },
		{ 306, "True Rotation Point  X-Offset", "mm", "Length", 1.0f, std::make_pair(-2147483648.0f, 2147483647.0f) },
		{ 307, "True Rotation Point Y-Offset", "mm", "Length", 1.0f, std::make_pair(-2147483648.0f, 2147483647.0f) },
		{ 308, "Actual Percentage Application Rate", "ppm", "Parts per million", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 309, "Minimum Percentage Application Rate", "ppm", "Parts per million", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 310, "Maximum Percentage Application Rate", "ppm", "Parts per million", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 311, "Relative Yield Potential", "ppm", "Parts per million", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 312, "Minimum Relative Yield Potential", "ppm", "Parts per million", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 313, "Maximum Relative Yield Potential", "ppm", "Parts per million", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 314, "Actual Percentage Crop Dry Matter", "ppm", "Parts per million", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 315, "Average Percentage Crop Dry Matter", "ppm", "Parts per million", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 316, "Effective Total Fuel Consumption", "ml", "Capacity large", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 317, "Ineffective Total Fuel Consumption", "ml", "Capacity large", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 318, "Effective Total Diesel Exhaust Fluid Consumption", "ml", "Capacity large", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 319, "Ineffective Total Diesel Exhaust Fluid Consumption", "ml", "Capacity large", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 320, "Last loaded Weight", "g", "Mass large", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 321, "Last unloaded Weight", "g", "Mass large", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 322, "Load Identification Number", "#", "Quantity/Count", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 323, "Unload Identification Number", "#", "Quantity/Count", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 324, "Chopper Engagement Total Time", "s", "Time count", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 325, "Lifetime Application Total Volume", "L", "Capacity count", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 326, "Setpoint Header Speed", "/s", "Quantity per time unit", 0.001f, std::make_pair(0.000f, 2147483.647f) },
		{ 327, "Actual Header Speed", "/s", "Quantity per time unit", 0.001f, std::make_pair(0.000f, 2147483.647f) },
		{ 328, "Minimum Header Speed", "/s", "Quantity per time unit", 0.001f, std::make_pair(0.000f, 2147483.647f) },
		{ 329, "Maximum Header Speed", "/s", "Quantity per time unit", 0.001f, std::make_pair(0.000f, 2147483.647f) },
		{ 330, "Setpoint Cutting drum speed", "/s", "Quantity per time unit", 0.001f, std::make_pair(0.000f, 2147483.647f) },
		{ 331, "Actual Cutting drum speed", "/s", "Quantity per time unit", 0.001f, std::make_pair(0.000f, 2147483.647f) },
		{ 332, "Minimum Cutting drum speed", "/s", "Quantity per time unit", 0.001f, std::make_pair(0.0f, 2147483.647f) },
		{ 333, "Maximum Cutting drum speed", "/s", "Quantity per time unit", 0.001f, std::make_pair(0.000f, 2147483.647f) },
		{ 334, "Operating Hours Since Last Sharpening", "s", "Time count", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 335, "Front PTO hours", "s", "Time count", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 336, "Rear PTO hours", "s", "Time count", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 337, "Lifetime Front PTO hours", "h", "Hour", 0.1f, std::make_pair(0.0f, 214748364.7f) },
		{ 338, "Lifetime Rear PTO Hours", "h", "Hour", 0.1f, std::make_pair(0.0f, 214748364.7f) },
		{ 339, "Effective Total Loading Time", "s", "Time count", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 340, "Effective Total Unloading Time", "s", "Time count", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 341, "Setpoint Grain Kernel Cracker Gap", "mm", "Length", 0.001f, std::make_pair(0.000f, 2147483.647f) },
		{ 342, "Actual Grain Kernel Cracker Gap", "mm", "Length", 0.001f, std::make_pair(0.000f, 2147483.647f) },
		{ 343, "Minimum Grain Kernel Cracker Gap", "mm", "Length", 0.001f, std::make_pair(0.000f, 2147483.647f) },
		{ 344, "Maximum Grain Kernel Cracker Gap", "mm", "Length", 0.001f, std::make_pair(0.000f, 2147483.647f) },
		{ 345, "Setpoint Swathing Width", "mm", "Length", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 346, "Actual Swathing Width", "mm", "Length", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 347, "Minimum Swathing Width", "mm", "Length", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 348, "Maximum Swathing Width", "mm", "Length", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 349, "Nozzle Drift Reduction", "ppm", "Parts per million", 1.0f, std::make_pair(0.0f, 100.0f) },
		{ 350, "Function or Operation Technique", "", "n.a.", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 351, "Application Total Volume in [ml]", "ml", "Capacity large", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 352, "Application Total Mass in gram [g]", "g", "Mass large", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 353, "Total Application of Nitrogen [N2]", "g", "Mass large", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 354, "Total Application of Ammonium", "g", "Mass large", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 355, "Total Application of Phosphor", "g", "Mass large", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 356, "Total Application of Potassium", "g", "Mass large", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 357, "Total Application of Dry Matter", "kg", "Mass", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 358, "Average Dry Yield Mass Per Time", "mg/s", "Mass flow", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 359, "Average Dry Yield Mass Per Area", "mg/m²", "Mass per area unit", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 360, "Last Bale Size", "mm", "Length", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 361, "Last Bale Density", "mg/l", "Mass per capacity unit", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 362, "Total Bale Length", "mm", "Length", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 363, "Last Bale Dry Mass", "g", "Mass large", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 364, "Actual Flake Size", "mm", "Length", 1.0f, std::make_pair(0.0f, 1000.0f) },
		{ 365, "Setpoint Downforce Pressure", "Pa", "Pressure", 1.0f, std::make_pair(-2147483648.0f, 2147483647.0f) },
		{ 366, "Actual Downforce Pressure", "Pa", "Pressure", 1.0f, std::make_pair(-2147483648.0f, 2147483647.0f) },
		{ 367, "Condensed Section Override State (1-16)", "", "n.a.", 1.0f, std::make_pair(0.0f, 4294967295.0f) },
		{ 368, "Condensed Section Override State (17-32)", "", "n.a.", 1.0f, std::make_pair(0.0f, 4294967295.0f) },
		{ 369, "Condensed Section Override State (33-48)", "", "n.a.", 1.0f, std::make_pair(0.0f, 4294967295.0f) },
		{ 370, "Condensed Section Override State (49-64)", "", "n.a.", 1.0f, std::make_pair(0.0f, 4294967295.0f) },
		{ 371, "Condensed Section Override State (65-80)", "", "n.a.", 1.0f, std::make_pair(0.0f, 4294967295.0f) },
		{ 372, "Condensed Section Override State (81-96)", "", "n.a.", 1.0f, std::make_pair(0.0f, 4294967295.0f) },
		{ 373, "Condensed Section Override State (97-112)", "", "n.a.", 1.0f, std::make_pair(0.0f, 4294967295.0f) },
		{ 374, "Condensed Section Override State (113-128)", "", "n.a.", 1.0f, std::make_pair(0.0f, 4294967295.0f) },
		{ 375, "Condensed Section Override State (129-144)", "", "n.a.", 1.0f, std::make_pair(0.0f, 4294967295.0f) },
		{ 376, "Condensed Section Override State (145-160)", "", "n.a.", 1.0f, std::make_pair(0.0f, 4294967295.0f) },
		{ 377, "Condensed Section Override State (161-176)", "", "n.a.", 1.0f, std::make_pair(0.0f, 4294967295.0f) },
		{ 378, "Condensed Section Override State (177-192)", "", "n.a.", 1.0f, std::make_pair(0.0f, 4294967295.0f) },
		{ 379, "Condensed Section Override State (193-208)", "", "n.a.", 1.0f, std::make_pair(0.0f, 4294967295.0f) },
		{ 380, "Condensed Section Override State (209-224)", "", "n.a.", 1.0f, std::make_pair(0.0f, 4294967295.0f) },
		{ 381, "Condensed Section Override State (225-240)", "", "n.a.", 1.0f, std::make_pair(0.0f, 4294967295.0f) },
		{ 382, "Condensed Section Override State (241-256)", "", "n.a.", 1.0f, std::make_pair(0.0f, 4294967295.0f) },
		{ 383, "Apparent Wind Direction", "°", "Angle", 1.0f, std::make_pair(0.0f, 359.0f) },
		{ 384, "Apparent Wind Speed", "mm/s", "Speed", 1.0f, std::make_pair(0.0f, 100000000.0f) },
		{ 385, "MSL Atmospheric Pressure", "Pa", "Pressure", 0.1f, std::make_pair(0.0f, 200000.0f) },
		{ 386, "Actual Atmospheric Pressure", "Pa", "Pressure", 0.1f, std::make_pair(0.0f, 200000.0f) },
		{ 387, "Total Revolutions in Fractional Revolutions", "#", "Quantity/Count", 0.0001f, std::make_pair(-214748.3648f, 214748.3647f) },
		{ 388, "Total Revolutions in Complete Revolutions", "#", "Quantity/Count", 1.0f, std::make_pair(-2147483648.0f, 2147483647.0f) },
		{ 389, "Setpoint Revolutions specified as count per time", "r/min", "Revolutions per minute", 0.0001f, std::make_pair(-214748.3648f, 214748.3647f) },
		{ 390, "Actual Revolutions Per Time", "r/min", "Revolutions per minute", 0.0001f, std::make_pair(-214748.3648f, 214748.3647f) },
		{ 391, "Default Revolutions Per Time", "r/min", "Revolutions per minute", 0.0001f, std::make_pair(-214748.3648f, 214748.3647f) },
		{ 392, "Minimum Revolutions Per Time", "r/min", "Revolutions per minute", 0.0001f, std::make_pair(-214748.3648f, 214748.3647f) },
		{ 393, "Maximum Revolutions Per Time", "r/min", "Revolutions per minute", 0.0001f, std::make_pair(-214748.3648f, 214748.3647f) },
		{ 394, "Actual Fuel Tank Content", "ml", "Capacity large", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 395, "Actual Diesel Exhaust Fluid Tank Content", "ml", "Capacity large", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 396, "Setpoint Speed", "mm/s", "Speed", 1.0f, std::make_pair(-2147483648.0f, 2147483647.0f) },
		{ 397, "Actual Speed", "mm/s", "Speed", 1.0f, std::make_pair(-2147483648.0f, 2147483647.0f) },
		{ 398, "Minimum Speed", "mm/s", "Speed", 1.0f, std::make_pair(-2147483648.0f, 2147483647.0f) },
		{ 399, "Maximum Speed", "mm/s", "Speed", 1.0f, std::make_pair(-2147483648.0f, 2147483647.0f) },
		{ 400, "Speed Source", "", "n.a.", 1.0f, std::make_pair(0.0f, 255.0f) },
		{ 401, "Actual Application of Nitrogen [N2] as [mg/l]", "mg/l", "Mass per capacity unit", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 402, "Actual application of Ammonium", "mg/l", "Mass per capacity unit", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 403, "Actual application of Phosphor", "mg/l", "Mass per capacity unit", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 404, "Actual application of Potassium", "mg/l", "Mass per capacity unit", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 405, "Actual application of Dry Matter", "mg/l", "Mass per capacity unit", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 406, "Actual Protein Content", "ppm", "Parts per million", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 407, "Average Protein Content", "ppm", "Parts per million", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 408, "Average Crop Contamination", "ppm", "Parts per million", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 409, "Total Diesel Exhaust Fluid Consumption", "ml", "Capacity large", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 410, "Instantaneous Diesel Exhaust Fluid Consumption per Time", "mm³/s", "Flow", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 411, "Instantaneous Diesel Exhaust Fluid Consumption per Area", "mm³/m²", "Capacity per area unit", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 412, "Lifetime Diesel Exhaust Fluid Consumption", "L", "Capacity count", 0.5f, std::make_pair(0.0f, 1073741823.5f) },
		{ 413, "Lifetime Average Diesel Exhaust Fluid Consumption per Time", "mm³/s", "Flow", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 414, "Lifetime Average Diesel Exhaust Fluid Consumption per Area", "mm³/m²", "Capacity per area unit", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 415, "Actual Seed Singulation Percentage", "ppm", "Parts per million", 1.0f, std::make_pair(0.0f, 1000000.0f) },
		{ 416, "Average Seed Singulation Percentage", "ppm", "Parts per million", 1.0f, std::make_pair(0.0f, 1000000.0f) },
		{ 417, "Actual Seed Skip Percentage", "ppm", "Parts per million", 1.0f, std::make_pair(0.0f, 1000000.0f) },
		{ 418, "Average Seed Skip Percentage", "ppm", "Parts per million", 1.0f, std::make_pair(0.0f, 1000000.0f) },
		{ 419, "Actual Seed Multiple Percentage", "ppm", "Parts per million", 1.0f, std::make_pair(0.0f, 1000000.0f) },
		{ 420, "Average Seed Multiple Percentage", "ppm", "Parts per million", 1.0f, std::make_pair(0.0f, 1000000.0f) },
		{ 421, "Actual Seed Spacing Deviation", "mm", "Length", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 422, "Average Seed Spacing Deviation", "mm", "Length", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 423, "Actual Coefficient of Variation of Seed Spacing Percentage", "ppm", "Parts per million", 1.0f, std::make_pair(0.0f, 1000000.0f) },
		{ 424, "Average Coefficient of Variation of Seed Spacing Percentage", "ppm", "Parts per million", 1.0f, std::make_pair(0.0f, 1000000.0f) },
		{ 425, "Setpoint Maximum Allowed Seed Spacing Deviation", "mm", "Length", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 426, "Setpoint Downforce as Force", "N", "Newton", 1.0f, std::make_pair(-2147483648.0f, 2147483647.0f) },
		{ 427, "Actual Downforce as Force", "N", "Newton", 1.0f, std::make_pair(-2147483648.0f, 2147483647.0f) },
		{ 428, "Loaded Total Mass", "kg", "Mass", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 429, "Unloaded Total Mass", "kg", "Mass", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 430, "Lifetime Loaded Total Mass", "kg", "Mass", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 431, "Lifetime Unloaded Total Mass", "kg", "Mass", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 432, "Setpoint Application Rate of Nitrogen [N2]", "mg/m²", "Mass per area unit", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 433, "Actual  Application Rate of Nitrogen [N2]", "mg/m²", "Mass per area unit", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 434, "Minimum Application Rate of Nitrogen [N2]", "mg/m²", "Mass per area unit", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 435, "Maximum  Application Rate of Nitrogen [N2]", "mg/m²", "Mass per area unit", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 436, "Setpoint  Application Rate of Ammonium", "mg/m²", "Mass per area unit", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 437, "Actual  Application Rate of Ammonium", "mg/m²", "Mass per area unit", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 438, "Minimum  Application Rate of Ammonium", "mg/m²", "Mass per area unit", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 439, "Maximum  Application Rate of Ammonium", "mg/m²", "Mass per area unit", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 440, "Setpoint  Application Rate of Phosphor", "mg/m²", "Mass per area unit", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 441, "Actual  Application Rate of Phosphor", "mg/m²", "Mass per area unit", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 442, "Minimum  Application Rate of Phosphor", "mg/m²", "Mass per area unit", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 443, "Maximum  Application Rate of Phosphor", "mg/m²", "Mass per area unit", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 444, "Setpoint  Application Rate of Potassium", "mg/m²", "Mass per area unit", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 445, "Actual  Application Rate of Potassium", "mg/m²", "Mass per area unit", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 446, "Minimum Application Rate of Potassium", "mg/m²", "Mass per area unit", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 447, "Maximum Application Rate of Potassium", "mg/m²", "Mass per area unit", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 448, "Setpoint Application Rate of Dry Matter", "ppm", "Parts per million", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 449, "Actual  Application Rate of Dry Matter", "ppm", "Parts per million", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 450, "Minimum Application Rate of Dry Matter", "ppm", "Parts per million", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 451, "Maximum Application Rate of Dry Matter", "ppm", "Parts per million", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 452, "Loaded Total Volume", "ml", "Capacity large", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 453, "Unloaded Total Volume", "ml", "Capacity large", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 454, "Lifetime loaded Total Volume", "L", "Capacity count", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 455, "Lifetime Unloaded Total Volume", "L", "Capacity count", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 456, "Last loaded Volume", "ml", "Capacity large", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 457, "Last unloaded Volume", "ml", "Capacity large", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 458, "Loaded Total Count", "#", "Quantity/Count", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 459, "Unloaded Total Count", "#", "Quantity/Count", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 460, "Lifetime Loaded Total Count", "#", "Quantity/Count", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 461, "Lifetime Unloaded Total Count", "#", "Quantity/Count", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 462, "Last loaded Count", "#", "Quantity/Count", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 463, "Last unloaded Count", "#", "Quantity/Count", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 464, "Haul Counter", "#", "Quantity/Count", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 465, "Lifetime Haul Counter", "#", "Quantity/Count", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 466, "Actual relative connector angle", "°", "Angle", 0.001f, std::make_pair(-180.000f, 180.000f) },
		{ 467, "Actual Percentage Content", "%", "Percent", 0.01f, std::make_pair(0.00f, 100.00f) },
		{ 468, "Soil Snow/Frozen Condtion", "", "n.a.", 1.0f, std::make_pair(0.0f, 3.0f) },
		{ 469, "Estimated Soil Water Condtion", "", "n.a.", 1.0f, std::make_pair(0.0f, 6.0f) },
		{ 470, "Soil Compaction", "", "n.a.", 1.0f, std::make_pair(0.0f, 4.0f) },
		{ 471, "Setpoint Cultural Practice", "", "n.a.", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 472, "Setpoint Length of Cut", "mm", "Length", 0.001f, std::make_pair(0.000f, 2147483.647f) },
		{ 473, "Minimum length of cut", "mm", "Length", 0.001f, std::make_pair(0.000f, 2147483.647f) },
		{ 474, "Maximum Length of Cut", "mm", "Length", 0.001f, std::make_pair(0.001f, 2147483.647f) },
		{ 475, "Setpoint Bale Hydraulic Pressure", "Pa", "Pressure", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 476, "Minimum Bale Hydraulic Pressure", "Pa", "Pressure", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 477, "Maximum Bale Hydraulic Pressure", "Pa", "Pressure", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 478, "Setpoint Flake Size", "mm", "Length", 1.0f, std::make_pair(0.0f, 1000.0f) },
		{ 479, "Minimum Flake Size", "mm", "Length", 1.0f, std::make_pair(0.0f, 1000.0f) },
		{ 480, "Maximum Flake Size", "mm", "Length", 1.0f, std::make_pair(0.0f, 1000.0f) },
		{ 481, "Setpoint Number of Subbales", "", "n.a.", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 482, "Last Bale Number of Subbales", "", "n.a.", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 483, "Setpoint Engine Speed", "r/min", "Revolutions per minute", 0.0001f, std::make_pair(0.0000f, 214748.3647f) },
		{ 484, "Actual Engine Speed", "r/min", "Revolutions per minute", 0.0001f, std::make_pair(0.0000f, 214748.3647f) },
		{ 485, "Minimum Engine Speed", "r/min", "Revolutions per minute", 0.0001f, std::make_pair(0.0000f, 214748.3647f) },
		{ 486, "Maximum Engine Speed", "r/min", "Revolutions per minute", 0.0001f, std::make_pair(0.0000f, 214748.3647f) },
		{ 488, "Diesel Exhaust Fluid Tank Percentage Level", "%", "Percent", 0.01f, std::make_pair(0.00f, 100.00f) },
		{ 489, "Maximum Diesel Exhaust Fluid Tank Content", "ml", "Capacity large", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 490, "Maximum Fuel Tank Content", "ml", "Capacity large", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 491, "Fuel Percentage Level", "%", "Percent", 0.01f, std::make_pair(0.00f, 21474836.47f) },
		{ 492, "Total Engine Hours", "h", "Hour", 0.05f, std::make_pair(0.00f, 210554060.75f) },
		{ 493, "Lifetime Engine Hours", "h", "Hour", 0.1f, std::make_pair(0.0f, 214748364.7f) },
		{ 494, "Last Event Partner ID (Byte 1-4)", "", "n.a.", 1.0f, std::make_pair(0.0f, 4294967295.0f) },
		{ 495, "Last Event Partner ID (Byte 5-8)", "", "n.a.", 1.0f, std::make_pair(0.0f, 4294967295.0f) },
		{ 496, "Last Event Partner ID (Byte 9-12)", "", "n.a.", 1.0f, std::make_pair(0.0f, 4294967295.0f) },
		{ 497, "Last Event Partner ID (Byte 13-16)", "", "n.a.", 1.0f, std::make_pair(0.0f, 4294967295.0f) },
		{ 498, "Last Event Partner ID Type", "", "n.a.", 1.0f, std::make_pair(0.0f, 4294967295.0f) },
		{ 499, "Last Event Partner ID Manufacturer ID Code", "", "n.a.", 1.0f, std::make_pair(0.0f, 4294967295.0f) },
		{ 500, "Last Event Partner ID Device Class", "", "n.a.", 1.0f, std::make_pair(0.0f, 4294967295.0f) },
		{ 501, "Setpoint Engine Torque", "%", "Percent", 0.001f, std::make_pair(0.000f, 2147483.647f) },
		{ 502, "Actual Engine Torque", "%", "Percent", 0.001f, std::make_pair(0.000f, 2147483.647f) },
		{ 503, "Minimum Engine Torque", "%", "Percent", 0.001f, std::make_pair(0.0f, 2147483.647f) },
		{ 504, "Maximum Engine Torque", "%", "Percent", 0.001f, std::make_pair(0.0f, 2147483.647f) },
		{ 505, "Tramline Control Level", "", "n.a.", 1.0f, std::make_pair(0.0f, 7.0f) },
		{ 506, "Setpoint Tramline Control Level", "", "n.a.", 1.0f, std::make_pair(0.0f, 3.0f) },
		{ 507, "Tramline Sequence Number", "", "n.a.", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 508, "Unique A-B Guidance Reference Line ID", "", "n.a.", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 509, "Actual Track Number", "", "n.a.", 1.0f, std::make_pair(-2147483648.0f, 2147483647.0f) },
		{ 510, "Track Number to the right", "", "n.a.", 1.0f, std::make_pair(-2147483648.0f, 2147483647.0f) },
		{ 511, "Track Number to the left", "", "n.a.", 1.0f, std::make_pair(-2147483648.0f, 2147483647.0f) },
		{ 512, "Guidance Line Swath Width", "mm", "Length", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 513, "Guidance Line Deviation", "mm", "Length", 1.0f, std::make_pair(-2147483648.0f, 2147483647.0f) },
		{ 514, "GNSS Quality", "", "n.a.", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 515, "Tramline Control State", "", "n.a.", 1.0f, std::make_pair(0.0f, 3.0f) },
		{ 516, "Tramline Overdosing Rate", "ppm", "Parts per million", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 517, "Setpoint Tramline Condensed Work State (1-16)", "", "n.a.", 1.0f, std::make_pair(0.0f, 4294967295.0f) },
		{ 518, "Actual Tramline Condensed Work State (1-16)", "", "n.a.", 1.0f, std::make_pair(0.0f, 4294967295.0f) },
		{ 519, "Last Bale Lifetime Count", "", "n.a.", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 520, "Actual Canopy Height", "mm", "Length", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 521, "GNSS Installation Type", "", "n.a.", 1.0f, std::make_pair(0.0f, 100.0f) },
		{ 522, "Twine Bale Total Count", "#", "Quantity/Count", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 523, "Mesh Bale Total Count (Deprecated)", "#", "Quantity/Count", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 524, "Lifetime Twine Bale Total Count (Deprecated)", "#", "Quantity/Count", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 525, "Lifetime Mesh Bale Total Count (Deprecated)", "#", "Quantity/Count", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 526, "Actual Cooling Fluid Temperature", "mK", "Temperature", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 528, "Last Bale Capacity", "kg/h", "Mass per hour unit", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 529, "Setpoint Tillage Disc Gang Angle", "°", "Angle", 0.001f, std::make_pair(-180.0f, 180.0f) },
		{ 530, "Actual Tillage Disc Gang Angle", "°", "Angle", 0.001f, std::make_pair(-180.0f, 180.0f) },
		{ 531, "Actual Applied Preservative Per Yield Mass", "mm³/kg", "Capacity per mass unit", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 532, "Setpoint Applied Preservative Per Yield Mass", "mm³/kg", "Capacity per mass unit", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 533, "Default Applied Preservative Per Yield Mass", "mm³/kg", "Capacity per mass unit", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 534, "Minimum Applied Preservative Per Yield Mass", "mm³/kg", "Capacity per mass unit", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 535, "Maximum Applied Preservative Per Yield Mass", "mm³/kg", "Capacity per mass unit", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 536, "Total Applied Preservative", "ml", "Capacity large", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 537, "Lifetime Applied Preservative", "ml", "Capacity large", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 538, "Average Applied Preservative Per Yield Mass", "mm³/kg", "Capacity per mass unit", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 539, "Actual Preservative Tank Volume", "ml", "Capacity large", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 540, "Actual Preservative Tank Level", "ppm", "Parts per million", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 541, "Actual PTO Speed", "r/min", "Revolutions per minute", 0.0001f, std::make_pair(0.0f, 214748.3647f) },
		{ 542, "Setpoint PTO Speed", "r/min", "Revolutions per minute", 0.0001f, std::make_pair(0.0f, 214748.3647f) },
		{ 543, "Default PTO Speed", "r/min", "Revolutions per minute", 0.0001f, std::make_pair(0.0f, 214748.3647f) },
		{ 544, "Minimum PTO Speed", "r/min", "Revolutions per minute", 0.0001f, std::make_pair(0.0f, 214748.3647f) },
		{ 545, "Maximum PTO Speed", "r/min", "Revolutions per minute", 0.0001f, std::make_pair(0.0f, 214748.3647f) },
		{ 546, "Lifetime Chopping Engagement Total Time", "s", "Time count", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 547, "Setpoint Bale Compression Plunger Load (N)", "N", "Newton", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 548, "Actual Bale Compression Plunger Load (N)", "N", "Newton", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 549, "Last Bale Average Bale Compression Plunger Load (N)", "N", "Newton", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 550, "Ground Cover", "%", "Percent", 0.1f, std::make_pair(0.0f, 100.0f) },
		{ 551, "Actual PTO Torque", "N*m", "Newton metre", 0.0001f, std::make_pair(0.0f, 214748.3647f) },
		{ 552, "Setpoint PTO Torque", "N*m", "Newton metre", 0.0001f, std::make_pair(0.0f, 214748.3647f) },
		{ 553, "Default PTO Torque", "N*m", "Newton metre", 0.0001f, std::make_pair(0.0f, 214748.3647f) },
		{ 554, "Minimum PTO Torque", "N*m", "Newton metre", 0.0001f, std::make_pair(0.0f, 214748.3647f) },
		{ 555, "Maximum PTO Torque", "N*m", "Newton metre", 0.0001f, std::make_pair(0.0f, 214748.3647f) },
		{ 556, "Present Weather Conditions", "", "n.a.", 1.0f, std::make_pair(1.0f, 6.0f) },
		{ 557, "Setpoint Electrical Current", "A", "Electrical current", 0.005f, std::make_pair(0.0f, 10737418.235f) },
		{ 558, "Actual Electrical Current", "A", "Electrical current", 0.005f, std::make_pair(0.0f, 10737418.235f) },
		{ 559, "Minimum Electrical Current", "A", "Electrical current", 0.005f, std::make_pair(0.0f, 10737418.235f) },
		{ 560, "Maximum Electrical Current", "A", "Electrical current", 0.005f, std::make_pair(0.0f, 10737418.235f) },
		{ 561, "Default Electrical Current", "A", "Electrical current", 0.005f, std::make_pair(0.0f, 10737418.235f) },
		{ 562, "Setpoint Voltage", "V", "Electrical voltage", 0.001f, std::make_pair(-2147483.648f, 2147483.647f) },
		{ 563, "Default Voltage", "V", "Electrical voltage", 0.001f, std::make_pair(-2147483.648f, 2147483.647f) },
		{ 564, "Actual Voltage", "V", "Electrical voltage", 0.001f, std::make_pair(-2147483.648f, 2147483.647f) },
		{ 565, "Minimum Voltage", "V", "Electrical voltage", 0.001f, std::make_pair(-2147483.648f, 2147483.647f) },
		{ 566, "Maximum Voltage", "V", "Electrical voltage", 0.001f, std::make_pair(-2147483.648f, 2147483.647f) },
		{ 567, "Actual Electrical Resistance", "Ohm", "Electrical resistance", 0.01f, std::make_pair(0.0f, 21474836.47f) },
		{ 568, "Setpoint Electrical Power", "W", "Electrical Power", 0.001f, std::make_pair(0.0f, 2147483.647f) },
		{ 569, "Actual Electrical Power", "W", "Electrical Power", 0.001f, std::make_pair(0.0f, 2147483.647f) },
		{ 570, "Default Electrical Power", "W", "Electrical Power", 0.001f, std::make_pair(0.0f, 2147483.647f) },
		{ 571, "Maximum Electrical Power", "W", "Electrical Power", 0.001f, std::make_pair(0.0f, 2147483.647f) },
		{ 572, "Minimum Electrical Power", "W", "Electrical Power", 0.001f, std::make_pair(0.0f, 2147483.647f) },
		{ 573, "Total Electrical Energy", "kWh", "Electrical energy", 0.001f, std::make_pair(0.0f, 2147483.647f) },
		{ 574, "Setpoint Electrical Energy per Area Application Rate", "kWh/m²", "Electrical energy per area", 1.0E-7f, std::make_pair(0.0f, 214.7483647f) },
		{ 575, "Actual  Electrical Energy per Area Application Rate", "kWh/m²", "Electrical energy per area", 1.0E-7f, std::make_pair(0.0f, 214.7483647f) },
		{ 576, "Maximum  Electrical Energy  per Area Application Rate", "kWh/m²", "Electrical energy per area", 1.0E-7f, std::make_pair(0.0f, 214.7483647f) },
		{ 577, "Minimum  Electrical Energy per Area Application Rate", "kWh/m²", "Electrical energy per area", 1.0E-7f, std::make_pair(0.0f, 214.7483647f) },
		{ 578, "Setpoint Temperature", "mK", "Temperature", 1.0f, std::make_pair(0.0f, 1000000.0f) },
		{ 579, "Actual Temperature", "mK", "Temperature", 1.0f, std::make_pair(0.0f, 1000000.0f) },
		{ 580, "Minimum Temperature", "mK", "Temperature", 1.0f, std::make_pair(0.0f, 1000000.0f) },
		{ 581, "Maximum Temperature", "mK", "Temperature", 1.0f, std::make_pair(0.0f, 1000000.0f) },
		{ 582, "Default Temperature", "mK", "Temperature", 1.0f, std::make_pair(0.0f, 1000000.0f) },
		{ 583, "Setpoint Frequency", "Hz", "Electrical frequency", 0.001f, std::make_pair(0.0f, 2147483.647f) },
		{ 584, "Actual Frequency", "Hz", "Electrical frequency", 0.001f, std::make_pair(0.0f, 2147483.647f) },
		{ 585, "Minimum Frequency", "Hz", "Electrical frequency", 0.001f, std::make_pair(0.0f, 2147483.647f) },
		{ 586, "Maximum Frequency", "Hz", "Electrical frequency", 0.001f, std::make_pair(0.0f, 2147483.647f) },
		{ 587, "Previous Rainfall", "", "n.a.", 1.0f, std::make_pair(1.0f, 6.0f) },
		{ 588, "Setpoint Volume Per Area Application Rate as [ml/m²]", "ml/m²", "Capacity per area large", 0.1f, std::make_pair(0.0f, 214748364.7f) },
		{ 589, "Actual Volume Per Area Application Rate as [ml/m²]", "ml/m²", "Capacity per area large", 0.1f, std::make_pair(0.0f, 214748364.7f) },
		{ 590, "Minimum Volume Per Area Application Rate as [ml/m²]", "ml/m²", "Capacity per area large", 0.1f, std::make_pair(0.0f, 214748364.7f) },
		{ 591, "Maximum Volume Per Area Application Rate as [ml/m²]", "ml/m²", "Capacity per area large", 0.1f, std::make_pair(0.0f, 214748364.7f) },
		{ 592, "Default Volume Per Area Application Rate as [ml/m²]", "ml/m²", "Capacity per area large", 0.1f, std::make_pair(0.0f, 214748364.7f) },
		{ 593, "Traction Type", "", "n.a.", 1.0f, std::make_pair(0.0f, 5.0f) },
		{ 594, "Steering Type", "", "n.a.", 1.0f, std::make_pair(0.0f, 7.0f) },
		{ 595, "Machine Mode", "", "n.a.", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 596, "Cargo Area Cover State", "%", "Percent", 1.0f, std::make_pair(-1.0f, 100.0f) },
		{ 597, "Total Distance", "mm", "Length", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 598, "Lifetime Total Distance", "m", "Distance", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 599, "Total Distance Field", "mm", "Length", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 600, "Lifetime Total Distance Field", "m", "Distance", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 601, "Total Distance Street", "mm", "Length", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 602, "Lifetime Total Distance Street", "m", "Distance", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 603, "Actual Tramline Condensed Work State (17-32)", "", "n.a.", 1.0f, std::make_pair(0.0f, 4294967295.0f) },
		{ 604, "Actual Tramline Condensed Work State (33-48)", "", "n.a.", 1.0f, std::make_pair(0.0f, 4294967295.0f) },
		{ 605, "Actual Tramline Condensed Work State (49-64)", "", "n.a.", 1.0f, std::make_pair(0.0f, 4294967295.0f) },
		{ 606, "Actual Tramline Condensed Work State (65-80)", "", "n.a.", 1.0f, std::make_pair(0.0f, 4294967295.0f) },
		{ 607, "Actual Tramline Condensed Work State (81-96)", "", "n.a.", 1.0f, std::make_pair(0.0f, 4294967295.0f) },
		{ 608, "Actual Tramline Condensed Work State (97-112)", "", "n.a.", 1.0f, std::make_pair(0.0f, 4294967295.0f) },
		{ 609, "Actual Tramline Condensed Work State (113-128)", "", "n.a.", 1.0f, std::make_pair(0.0f, 4294967295.0f) },
		{ 610, "Actual Tramline Condensed Work State (129-144)", "", "n.a.", 1.0f, std::make_pair(0.0f, 4294967295.0f) },
		{ 611, "Actual Tramline Condensed Work State (145-160)", "", "n.a.", 1.0f, std::make_pair(0.0f, 4294967295.0f) },
		{ 612, "Actual Tramline Condensed Work State (161-176)", "", "n.a.", 1.0f, std::make_pair(0.0f, 4294967295.0f) },
		{ 613, "Actual Tramline Condensed Work State (177-192)", "", "n.a.", 1.0f, std::make_pair(0.0f, 4294967295.0f) },
		{ 614, "Actual Tramline Condensed Work State (193-208)", "", "n.a.", 1.0f, std::make_pair(0.0f, 4294967295.0f) },
		{ 615, "Actual Tramline Condensed Work State (209-224)", "", "n.a.", 1.0f, std::make_pair(0.0f, 4294967295.0f) },
		{ 616, "Actual Tramline Condensed Work State (225-240)", "", "n.a.", 1.0f, std::make_pair(0.0f, 4294967295.0f) },
		{ 617, "Actual Tramline Condensed Work State (241-256)", "", "n.a.", 1.0f, std::make_pair(0.0f, 4294967295.0f) },
		{ 618, "Setpoint Tramline Condensed Work State (17-32)", "", "n.a.", 1.0f, std::make_pair(0.0f, 4294967295.0f) },
		{ 619, "Setpoint Tramline Condensed Work State (33-48)", "", "n.a.", 1.0f, std::make_pair(0.0f, 4294967295.0f) },
		{ 620, "Setpoint Tramline Condensed Work State (49-64)", "", "n.a.", 1.0f, std::make_pair(0.0f, 4294967295.0f) },
		{ 621, "Setpoint Tramline Condensed Work State (65-80)", "", "n.a.", 1.0f, std::make_pair(0.0f, 4294967295.0f) },
		{ 622, "Setpoint Tramline Condensed Work State (81-96)", "", "n.a.", 1.0f, std::make_pair(0.0f, 4294967295.0f) },
		{ 623, "Setpoint Tramline Condensed Work State (97-112)", "", "n.a.", 1.0f, std::make_pair(0.0f, 4294967295.0f) },
		{ 624, "Setpoint Tramline Condensed Work State (113-128)", "", "n.a.", 1.0f, std::make_pair(0.0f, 4294967295.0f) },
		{ 625, "Setpoint Tramline Condensed Work State (129-144)", "", "n.a.", 1.0f, std::make_pair(0.0f, 4294967295.0f) },
		{ 626, "Setpoint Tramline Condensed Work State (145-160)", "", "n.a.", 1.0f, std::make_pair(0.0f, 4294967295.0f) },
		{ 627, "Setpoint Tramline Condensed Work State (161-176)", "", "n.a.", 1.0f, std::make_pair(0.0f, 4294967295.0f) },
		{ 628, "Setpoint Tramline Condensed Work State (177-192)", "", "n.a.", 1.0f, std::make_pair(0.0f, 4294967295.0f) },
		{ 629, "Setpoint Tramline Condensed Work State (193-208)", "", "n.a.", 1.0f, std::make_pair(0.0f, 4294967295.0f) },
		{ 630, "Setpoint Tramline Condensed Work State (209-224)", "", "n.a.", 1.0f, std::make_pair(0.0f, 4294967295.0f) },
		{ 631, "Setpoint Tramline Condensed Work State (225-240)", "", "n.a.", 1.0f, std::make_pair(0.0f, 4294967295.0f) },
		{ 632, "Setpoint Tramline Condensed Work State (241-256)", "", "n.a.", 1.0f, std::make_pair(0.0f, 4294967295.0f) },
		{ 633, "Setpoint Volume per distance Application Rate", "ml/m", "Volume per distance", 0.001f, std::make_pair(0.0f, 2147483.647f) },
		{ 634, "Actual Volume per distance Application Rate", "ml/m", "Volume per distance", 0.001f, std::make_pair(0.0f, 2147483.647f) },
		{ 635, "Default Volume per distance Application Rate", "ml/m", "Volume per distance", 0.001f, std::make_pair(0.0f, 2147483.647f) },
		{ 636, "Minimum Volume per distance Application Rate", "ml/m", "Volume per distance", 0.001f, std::make_pair(0.0f, 2147483.647f) },
		{ 637, "Maximum Volume per distance Application Rate", "ml/m", "Volume per distance", 0.001f, std::make_pair(0.0f, 2147483.647f) },
		{ 638, "Setpoint Tire Pressure", "Pa", "Pressure", 0.1f, std::make_pair(0.0f, 214748364.7f) },
		{ 639, "Actual Tire Pressure", "Pa", "Pressure", 0.1f, std::make_pair(0.0f, 214748364.7f) },
		{ 640, "Default Tire Pressure", "Pa", "Pressure", 0.1f, std::make_pair(0.0f, 214748364.7f) },
		{ 641, "Minimum Tire Pressure", "Pa", "Pressure", 0.1f, std::make_pair(0.0f, 214748364.7f) },
		{ 642, "Maximum Tire Pressure", "Pa", "Pressure", 0.1f, std::make_pair(0.0f, 214748364.7f) },
		{ 643, "Actual Tire Temperature", "mK", "Temperature", 1.0f, std::make_pair(0.0f, 1000000.0f) },
		{ 644, "Binding Method", "", "n.a.", 1.0f, std::make_pair(0.0f, 5.0f) },
		{ 645, "Last Bale Number of Knives", "#", "Quantity/Count", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 646, "Last Bale Binding Twine Consumption", "mm", "Length", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 647, "Last Bale Binding Mesh Consumption", "mm", "Length", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 648, "Last Bale Binding Film Consumption", "mm", "Length", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 649, "Last Bale Binding Film Stretching", "%", "Percent", 0.001f, std::make_pair(0.0f, 2147483.647f) },
		{ 650, "Last Bale Wrapping Film Width", "mm", "Length", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 651, "Last Bale Wrapping Film Consumption", "mm", "Length", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 652, "Last Bale Wrapping Film Stretching", "%", "Percent", 0.001f, std::make_pair(0.0f, 2147483.647f) },
		{ 653, "Last Bale Wrapping Film Overlap Percentage", "%", "Percent", 0.001f, std::make_pair(0.0f, 2147483.647f) },
		{ 654, "Last Bale Wrapping Film Layers", "#", "Quantity/Count", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 655, "Electrical Apparent Soil Conductivity", "mS/m", "Milli Siemens per meter", 0.1f, std::make_pair(-3300.0f, 3300.0f) },
		{ 656, "SC Actual Turn On Time", "", "n.a.", 1.0f, std::make_pair(-2147483648.0f, 2147483647.0f) },
		{ 657, "SC Actual Turn Off Time", "", "n.a.", 1.0f, std::make_pair(-2147483648.0f, 2147483647.0f) },
		{ 658, "Actual CO2 equivalent specified as mass per area", "mg/m²", "Mass per area unit", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 659, "Actual CO2 equivalent specified as mass per time", "mg/s", "Mass flow", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 660, "Actual CO2 equivalent specified as mass per mass", "mg/kg", "Mass per mass unit", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 661, "Actual CO2 equivalent specified as mass per yield", "mg/kg", "Mass per mass unit", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 662, "Actual CO2 equivalent specified as mass per volume", "mg/l", "Mass per capacity unit", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 663, "Actual CO2 equivalent specified as mass per count", "", "n.a.", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 664, "Total CO2 equivalent", "kg", "Mass", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 665, "Lifetime total CO2 equivalent", "kg", "Mass", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 666, "Working Direction", "", "n.a.", 1.0f, std::make_pair(0.0f, 2.0f) },
		{ 667, "Distance between Guidance Track Number 0R and 1", "mm", "Length", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 668, "Distance between Guidance Track Number 0R and 0L", "mm", "Length", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 669, "Bout Track Number Shift", "", "n.a.", 1.0f, std::make_pair(-2147483648.0f, 2147483647.0f) },
		{ 670, "Tramline Crop protection/fertilization Working Width", "mm", "Length", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 671, "Tramline Tire Width", "mm", "Length", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 672, "Tramline Wheel Distance", "mm", "Length", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 673, "Tramline Irrigation Working Width", "mm", "Length", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 674, "Tramline Irrigation Tire Width", "mm", "Length", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 675, "Tramline Irrigation Wheel Distance", "mm", "Length", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 676, "Last Bale Binding Mesh Layers", "#", "Quantity/Count", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 677, "Last Bale Binding Film Layers", "#", "Quantity/Count", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 678, "Last Bale Binding Twine Layers", "#", "Quantity/Count", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 679, "Crop Contamination Total Mass", "kg", "Mass", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 680, "Crop Contamination Lifetime Total Mass", "kg", "Mass", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 681, "Film bale Total Count", "#", "Quantity/Count", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 682, "Mesh bale Total Count", "#", "Quantity/Count", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 683, "Twine bale Total Count", "#", "Quantity/Count", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 684, "Wrapping Film bale Total Count", "#", "Quantity/Count", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 685, "Lifetime Film Bale Total Count", "#", "Quantity/Count", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 686, "Lifetime Mesh Bale Total Count", "#", "Quantity/Count", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 687, "Lifetime Twine Bale Total Count", "#", "Quantity/Count", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 688, "Lifetime Wrapping Film Bale Total Count", "#", "Quantity/Count", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 689, "Effective Total Electrical Battery Energy Consumption", "kWh", "Electrical energy", 0.001f, std::make_pair(-2147483.648f, 2147483.647f) },
		{ 690, "Ineffective Total Electrical Battery Energy Consumption", "kWh", "Electrical energy", 0.001f, std::make_pair(-2147483.648f, 2147483.647f) },
		{ 691, "Instantaneous Electrical Battery Energy Consumption per Time", "W", "Electrical Power", 1.0f, std::make_pair(-2147483648.0f, 2147483647.0f) },
		{ 692, "Instantaneous Electrical Battery Energy Consumption per Area", "kWh/m²", "Electrical energy per area", 1.0E-5f, std::make_pair(-21474.83648f, 21474.836470000002f) },
		{ 32768, "Maximum Droplet Size", "", "n.a.", 1.0f, std::make_pair(0.0f, 255.0f) },
		{ 32769, "Maximum Crop Grade Diameter", "mm", "Length", 0.001f, std::make_pair(0.0f, 2147483647.0f) },
		{ 32770, "Maximum Crop Grade Length", "mm", "Length", 0.001f, std::make_pair(0.0f, 2147483647.0f) },
		{ 32771, "Maximum Crop Contamination Mass per Area", "mg/m²", "Mass per area unit", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 32772, "Maximum Crop Contamination Mass per Time", "mg/s", "Mass flow", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 32773, "Maximum Crop Conditioning Intensity", "%", "Percent", 0.01f, std::make_pair(0.0f, 100.00f) },
		{ 36864, "Minimum Droplet Size", "", "n.a.", 1.0f, std::make_pair(0.0f, 255.0f) },
		{ 36865, "Minimum Crop Grade Diameter", "mm", "Length", 0.001f, std::make_pair(0.0f, 2147483647.0f) },
		{ 36866, "Minimum Crop Grade Length", "mm", "Length", 0.001f, std::make_pair(0.0f, 2147483647.0f) },
		{ 36867, "Minimum Crop Contamination Mass per Area", "mg/m²", "Mass per area unit", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 36868, "Minimum Crop Contamination Mass per Time", "mg/s", "Mass flow", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 36869, "Minimum Crop Conditioning Intensity", "%", "Percent", 0.01f, std::make_pair(0.0f, 100.00f) },
		{ 40960, "Default Droplet Size", "", "n.a.", 1.0f, std::make_pair(0.0f, 255.0f) },
		{ 40961, "Default Crop Grade Diameter", "mm", "Length", 0.001f, std::make_pair(0.0f, 2147483647.0f) },
		{ 40962, "Default Crop Grade Length", "mm", "Length", 0.001f, std::make_pair(0.0f, 2147483647.0f) },
		{ 40963, "Default Crop Contamination Mass per Area", "mg/m²", "Mass per area unit", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 40964, "Default Crop Contamination Mass per Time", "mg/s", "Mass flow", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 40965, "Default Crop Conditioning Intensity", "%", "Percent", 0.01f, std::make_pair(0.0f, 100.00f) },
		{ 45056, "Actual Droplet Size", "", "n.a.", 1.0f, std::make_pair(0.0f, 255.0f) },
		{ 45057, "Actual Crop Grade Diameter", "mm", "Length", 0.001f, std::make_pair(0.0f, 2147483647.0f) },
		{ 45058, "Actual Crop Grade Length", "mm", "Length", 0.001f, std::make_pair(0.0f, 2147483647.0f) },
		{ 45059, "Actual Crop Contamination Mass per Area", "mg/m²", "Mass per area unit", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 45060, "Actual Crop Contamination Mass per Time", "mg/s", "Mass flow", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 45061, "Actual Crop Conditioning Intensity", "%", "Percent", 0.01f, std::make_pair(0.0f, 100.00f) },
		{ 49152, "Setpoint Droplet Size", "", "n.a.", 1.0f, std::make_pair(0.0f, 255.0f) },
		{ 49153, "Setpoint Crop Grade Diameter", "mm", "Length", 0.001f, std::make_pair(0.0f, 2147483647.0f) },
		{ 49154, "Setpoint Crop Grade Length", "mm", "Length", 0.001f, std::make_pair(0.0f, 2147483647.0f) },
		{ 49155, "Setpoint Crop Contamination Mass per Area", "mg/m²", "Mass per area unit", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 49156, "Setpoint Crop Contamination Mass per Time", "mg/s", "Mass flow", 1.0f, std::make_pair(0.0f, 2147483647.0f) },
		{ 49157, "Setpoint Crop Conditioning Intensity", "%", "Percent", 0.01f, std::make_pair(0.0f, 100.00f) },
		{ 57342, "PGN Based Data", "", "n.a.", 1.0f, std::make_pair(-2147483648.0f, 2147483647.0f) },
		{ 57343, "Request Default Process Data", "", "n.a.", 1.0f, std::make_pair(0.0f, 0.0f) },
		{ 57344, "65534 Proprietary DDI Range", "", "n.a.", 0.0f, std::make_pair(0.0f, 0.0f) },
		{ 65535, "Reserved", "", "n.a.", 0.0f, std::make_pair(0.0f, 0.0f) },
	};
#endif

} // namespace isobus
