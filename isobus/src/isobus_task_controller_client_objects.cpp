//================================================================================================
/// @file isobus_task_controller_client_objects.cpp
///
/// @brief Implements the base functionality of the basic task controller objects.
/// @author Adrian Del Grosso
///
/// @copyright 2023 The Open-Agriculture Developers
//================================================================================================
#include "isobus/isobus/isobus_task_controller_client_objects.hpp"

#include "isobus/isobus/can_constants.hpp"
#include "isobus/utility/platform_endianness.hpp"

#include <algorithm>
#include <array>
#include <cstring>

namespace isobus
{
	namespace task_controller_object
	{
		Object::Object(std::string objectDesignator, std::uint16_t uniqueID) :
		  designator(objectDesignator),
		  objectID(uniqueID)
		{
		}

		std::string Object::get_designator() const
		{
			return designator;
		}

		void Object::set_designator(const std::string &newDesignator)
		{
			designator = newDesignator;
		}

		std::uint16_t Object::get_object_id() const
		{
			return objectID;
		}

		void Object::set_object_id(std::uint16_t id)
		{
			objectID = id;
		}

		const std::string DeviceObject::tableID = "DVC";

		DeviceObject::DeviceObject(std::string deviceDesignator,
		                           std::string deviceSoftwareVersion,
		                           std::string deviceSerialNumber,
		                           std::string deviceStructureLabel,
		                           std::array<std::uint8_t, task_controller_object::DeviceObject::MAX_STRUCTURE_AND_LOCALIZATION_LABEL_LENGTH> deviceLocalizationLabel,
		                           std::vector<std::uint8_t> deviceExtendedStructureLabel,
		                           std::uint64_t clientIsoNAME,
		                           bool shouldUseExtendedStructureLabel) :
		  Object(deviceDesignator, 0),
		  serialNumber(deviceSerialNumber),
		  softwareVersion(deviceSoftwareVersion),
		  structureLabel(deviceStructureLabel),
		  localizationLabel(deviceLocalizationLabel),
		  extendedStructureLabel(deviceExtendedStructureLabel),
		  NAME(clientIsoNAME),
		  useExtendedStructureLabel(shouldUseExtendedStructureLabel)
		{
		}

		std::string DeviceObject::get_table_id() const
		{
			return tableID;
		}

		ObjectTypes DeviceObject::get_object_type() const
		{
			return ObjectTypes::Device;
		}

		std::vector<std::uint8_t> DeviceObject::get_binary_object() const
		{
			std::vector<std::uint8_t> retVal;

			retVal.reserve(31 +
			               designator.size() +
			               softwareVersion.size() +
			               serialNumber.size() +
			               extendedStructureLabel.size());

			retVal.push_back(tableID[0]);
			retVal.push_back(tableID[1]);
			retVal.push_back(tableID[2]);
			retVal.push_back(static_cast<std::uint8_t>(get_object_id() & 0xFF));
			retVal.push_back(static_cast<std::uint8_t>((get_object_id() >> 8) & 0xFF));
			retVal.push_back(static_cast<std::uint8_t>(designator.size()));

			for (const auto &character : designator)
			{
				retVal.push_back(character);
			}
			retVal.push_back(static_cast<std::uint8_t>(softwareVersion.size()));
			for (const auto &character : softwareVersion)
			{
				retVal.push_back(character);
			}
			retVal.push_back(static_cast<std::uint8_t>(NAME & 0xFF));
			retVal.push_back(static_cast<std::uint8_t>((NAME >> 8) & 0xFF));
			retVal.push_back(static_cast<std::uint8_t>((NAME >> 16) & 0xFF));
			retVal.push_back(static_cast<std::uint8_t>((NAME >> 24) & 0xFF));
			retVal.push_back(static_cast<std::uint8_t>((NAME >> 32) & 0xFF));
			retVal.push_back(static_cast<std::uint8_t>((NAME >> 40) & 0xFF));
			retVal.push_back(static_cast<std::uint8_t>((NAME >> 48) & 0xFF));
			retVal.push_back(static_cast<std::uint8_t>((NAME >> 56) & 0xFF));
			retVal.push_back(static_cast<std::uint8_t>(serialNumber.size()));
			for (std::size_t i = 0; i < serialNumber.size(); i++)
			{
				retVal.push_back(serialNumber[i]);
			}
			for (std::uint_fast8_t i = 0; i < MAX_STRUCTURE_AND_LOCALIZATION_LABEL_LENGTH; i++)
			{
				if (i < structureLabel.size())
				{
					retVal.push_back(structureLabel[i]);
				}
				else
				{
					retVal.push_back(' ');
				}
			}
			for (std::uint_fast8_t i = 0; i < MAX_STRUCTURE_AND_LOCALIZATION_LABEL_LENGTH; i++)
			{
				if (i < localizationLabel.size())
				{
					retVal.push_back(localizationLabel[i]);
				}
				else
				{
					retVal.push_back(' ');
				}
			}
			if (useExtendedStructureLabel)
			{
				retVal.push_back(static_cast<std::uint8_t>(extendedStructureLabel.size()));
				for (std::size_t i = 0; i < extendedStructureLabel.size(); i++)
				{
					retVal.push_back(extendedStructureLabel[i]);
				}
			}
			return retVal;
		}

		std::string DeviceObject::get_software_version() const
		{
			return softwareVersion;
		}

		void DeviceObject::set_software_version(const std::string &version)
		{
			softwareVersion = version;
		}

		std::string DeviceObject::get_serial_number() const
		{
			return serialNumber;
		}

		void DeviceObject::set_serial_number(const std::string &serial)
		{
			serialNumber = serial;
		}

		std::string DeviceObject::get_structure_label() const
		{
			return structureLabel;
		}

		void DeviceObject::set_structure_label(const std::string &label)
		{
			structureLabel = label;
		}

		std::array<std::uint8_t, task_controller_object::DeviceObject::MAX_STRUCTURE_AND_LOCALIZATION_LABEL_LENGTH> DeviceObject::get_localization_label() const
		{
			return localizationLabel;
		}

		void DeviceObject::set_localization_label(std::array<std::uint8_t, 7> label)
		{
			localizationLabel = label;
		}

		std::vector<std::uint8_t> DeviceObject::get_extended_structure_label() const
		{
			return extendedStructureLabel;
		}

		void DeviceObject::set_extended_structure_label(const std::vector<std::uint8_t> &label)
		{
			extendedStructureLabel = label;
		}

		std::uint64_t DeviceObject::get_iso_name() const
		{
			return NAME;
		}

		void DeviceObject::set_iso_name(std::uint64_t name)
		{
			NAME = name;
		}

		bool DeviceObject::get_use_extended_structure_label() const
		{
			return useExtendedStructureLabel;
		}

		void DeviceObject::set_use_extended_structure_label(bool shouldUseExtendedStructureLabel)
		{
			useExtendedStructureLabel = shouldUseExtendedStructureLabel;
		}

		const std::string DeviceElementObject::tableID = "DET";

		DeviceElementObject::DeviceElementObject(std::string deviceElementDesignator,
		                                         std::uint16_t deviceElementNumber,
		                                         std::uint16_t parentObjectID,
		                                         Type deviceEelementType,
		                                         std::uint16_t uniqueID) :
		  Object(deviceElementDesignator, uniqueID),
		  elementNumber(deviceElementNumber),
		  parentObject(parentObjectID),
		  elementType(deviceEelementType)
		{
		}

		std::string DeviceElementObject::get_table_id() const
		{
			return tableID;
		}

		ObjectTypes DeviceElementObject::get_object_type() const
		{
			return ObjectTypes::DeviceElement;
		}

		std::vector<std::uint8_t> DeviceElementObject::get_binary_object() const
		{
			std::vector<std::uint8_t> retVal;

			retVal.reserve(14 +
			               designator.size() +
			               2 * referenceList.size());

			retVal.push_back(tableID[0]);
			retVal.push_back(tableID[1]);
			retVal.push_back(tableID[2]);
			retVal.push_back(static_cast<std::uint8_t>(get_object_id() & 0xFF));
			retVal.push_back(static_cast<std::uint8_t>((get_object_id() >> 8) & 0xFF));
			retVal.push_back(static_cast<std::uint8_t>(elementType));
			retVal.push_back(static_cast<std::uint8_t>(designator.size()));
			for (std::size_t i = 0; i < designator.size(); i++)
			{
				retVal.push_back(designator[i]);
			}
			retVal.push_back(static_cast<std::uint8_t>(elementNumber & 0xFF));
			retVal.push_back(static_cast<std::uint8_t>((elementNumber >> 8) & 0xFF));
			retVal.push_back(static_cast<std::uint8_t>(parentObject & 0xFF));
			retVal.push_back(static_cast<std::uint8_t>((parentObject >> 8) & 0xFF));
			std::uint16_t tempSize = static_cast<std::uint16_t>(referenceList.size());
			retVal.push_back(tempSize & 0xFF);
			retVal.push_back((tempSize >> 8) & 0xFF);
			for (std::size_t i = 0; i < tempSize; i++)
			{
				retVal.push_back(static_cast<std::uint8_t>(referenceList[i] & 0xFF));
				retVal.push_back(static_cast<std::uint8_t>((referenceList[i] >> 8) & 0xFF));
			}
			return retVal;
		}

		std::uint16_t DeviceElementObject::get_element_number() const
		{
			return elementNumber;
		}

		void DeviceElementObject::set_element_number(std::uint16_t newElementNumber)
		{
			elementNumber = newElementNumber;
		}

		std::uint16_t DeviceElementObject::get_parent_object() const
		{
			return parentObject;
		}

		void DeviceElementObject::set_parent_object(std::uint16_t parentObjectID)
		{
			parentObject = parentObjectID;
		}

		DeviceElementObject::Type DeviceElementObject::get_type() const
		{
			return elementType;
		}

		void DeviceElementObject::add_reference_to_child_object(std::uint16_t childID)
		{
			referenceList.push_back(childID);
		}

		bool DeviceElementObject::remove_reference_to_child_object(std::uint16_t childID)
		{
			bool retVal = false;

			auto result = std::find(referenceList.begin(), referenceList.end(), childID);
			if (result != referenceList.end())
			{
				retVal = true;
				referenceList.erase(result);
			}
			return retVal;
		}

		DataSpan<const std::uint16_t> DeviceElementObject::get_child_object_ids() const
		{
			return DataSpan<const std::uint16_t>(referenceList.data(), referenceList.size());
		}

		std::uint16_t DeviceElementObject::get_number_child_objects() const
		{
			return static_cast<std::uint16_t>(referenceList.size());
		}

		std::uint16_t DeviceElementObject::get_child_object_id(std::size_t index)
		{
			std::uint16_t retVal = NULL_OBJECT_ID;

			if (index < get_number_child_objects())
			{
				retVal = referenceList[index];
			}
			return retVal;
		}

		const std::string DeviceProcessDataObject::tableID = "DPD";

		DeviceProcessDataObject::DeviceProcessDataObject(std::string processDataDesignator,
		                                                 std::uint16_t processDataDDI,
		                                                 std::uint16_t deviceValuePresentationObjectID,
		                                                 std::uint8_t processDataProperties,
		                                                 std::uint8_t processDataTriggerMethods,
		                                                 std::uint16_t uniqueID) :
		  Object(processDataDesignator, uniqueID),
		  ddi(processDataDDI),
		  deviceValuePresentationObject(deviceValuePresentationObjectID),
		  propertiesBitfield(processDataProperties),
		  triggerMethodsBitfield(processDataTriggerMethods)
		{
		}

		std::string DeviceProcessDataObject::get_table_id() const
		{
			return tableID;
		}

		ObjectTypes DeviceProcessDataObject::get_object_type() const
		{
			return ObjectTypes::DeviceProcessData;
		}

		std::vector<std::uint8_t> DeviceProcessDataObject::get_binary_object() const
		{
			std::vector<std::uint8_t> retVal;

			retVal.reserve(11 + designator.size());

			retVal.push_back(tableID[0]);
			retVal.push_back(tableID[1]);
			retVal.push_back(tableID[2]);
			retVal.push_back(static_cast<std::uint8_t>(get_object_id() & 0xFF));
			retVal.push_back(static_cast<std::uint8_t>((get_object_id() >> 8) & 0xFF));
			retVal.push_back(static_cast<std::uint8_t>(ddi & 0xFF));
			retVal.push_back(static_cast<std::uint8_t>((ddi >> 8) & 0xFF));
			retVal.push_back(propertiesBitfield);
			retVal.push_back(triggerMethodsBitfield);
			retVal.push_back(static_cast<std::uint8_t>(designator.size()));
			for (std::size_t i = 0; i < designator.size(); i++)
			{
				retVal.push_back(designator[i]);
			}
			retVal.push_back(deviceValuePresentationObject & 0xFF);
			retVal.push_back((deviceValuePresentationObject >> 8) & 0xFF);
			return retVal;
		}

		std::uint16_t DeviceProcessDataObject::get_ddi() const
		{
			return ddi;
		}

		void DeviceProcessDataObject::set_ddi(std::uint16_t newDDI)
		{
			ddi = newDDI;
		}

		std::uint16_t DeviceProcessDataObject::get_device_value_presentation_object_id() const
		{
			return deviceValuePresentationObject;
		}

		void DeviceProcessDataObject::set_device_value_presentation_object_id(std::uint16_t id)
		{
			deviceValuePresentationObject = id;
		}

		std::uint8_t DeviceProcessDataObject::get_properties_bitfield() const
		{
			return propertiesBitfield;
		}

		void DeviceProcessDataObject::set_properties_bitfield(std::uint8_t properties)
		{
			propertiesBitfield = properties;
		}

		bool DeviceProcessDataObject::has_property(DeviceProcessDataObject::PropertiesBit property)
		{
			return (0 != (propertiesBitfield & static_cast<std::uint8_t>(property)));
		}

		std::uint8_t DeviceProcessDataObject::get_trigger_methods_bitfield() const
		{
			return triggerMethodsBitfield;
		}

		void DeviceProcessDataObject::set_trigger_methods_bitfield(std::uint8_t methods)
		{
			triggerMethodsBitfield = methods;
		}

		bool DeviceProcessDataObject::has_trigger_method(DeviceProcessDataObject::AvailableTriggerMethods method)
		{
			return (0 != (triggerMethodsBitfield & static_cast<std::uint8_t>(method)));
		}

		const std::string DevicePropertyObject::tableID = "DPT";

		DevicePropertyObject::DevicePropertyObject(std::string propertyDesignator,
		                                           std::int32_t propertyValue,
		                                           std::uint16_t propertyDDI,
		                                           std::uint16_t valuePresentationObject,
		                                           std::uint16_t uniqueID) :
		  Object(propertyDesignator, uniqueID),
		  value(propertyValue),
		  ddi(propertyDDI),
		  deviceValuePresentationObject(valuePresentationObject)
		{
		}

		std::string DevicePropertyObject::get_table_id() const
		{
			return tableID;
		}

		ObjectTypes DevicePropertyObject::get_object_type() const
		{
			return ObjectTypes::DeviceProperty;
		}

		std::vector<std::uint8_t> DevicePropertyObject::get_binary_object() const
		{
			std::vector<std::uint8_t> retVal;

			retVal.reserve(13 + designator.size());

			retVal.push_back(tableID[0]);
			retVal.push_back(tableID[1]);
			retVal.push_back(tableID[2]);
			retVal.push_back(static_cast<std::uint8_t>(get_object_id() & 0xFF));
			retVal.push_back(static_cast<std::uint8_t>((get_object_id() >> 8) & 0xFF));
			retVal.push_back(static_cast<std::uint8_t>(ddi & 0xFF));
			retVal.push_back(static_cast<std::uint8_t>((ddi >> 8) & 0xFF));
			retVal.push_back(static_cast<std::uint8_t>(value & 0xFF));
			retVal.push_back(static_cast<std::uint8_t>((value >> 8) & 0xFF));
			retVal.push_back(static_cast<std::uint8_t>((value >> 16) & 0xFF));
			retVal.push_back(static_cast<std::uint8_t>((value >> 24) & 0xFF));
			retVal.push_back(static_cast<std::uint8_t>(designator.size()));
			for (std::size_t i = 0; i < designator.size(); i++)
			{
				retVal.push_back(designator[i]);
			}
			retVal.push_back(deviceValuePresentationObject & 0xFF);
			retVal.push_back((deviceValuePresentationObject >> 8) & 0xFF);
			return retVal;
		}

		std::int32_t DevicePropertyObject::get_value() const
		{
			return value;
		}

		void DevicePropertyObject::set_value(std::int32_t newValue)
		{
			value = newValue;
		}

		std::uint16_t DevicePropertyObject::get_ddi() const
		{
			return ddi;
		}

		void DevicePropertyObject::set_ddi(std::uint16_t newDDI)
		{
			ddi = newDDI;
		}

		std::uint16_t DevicePropertyObject::get_device_value_presentation_object_id() const
		{
			return deviceValuePresentationObject;
		}

		void DevicePropertyObject::set_device_value_presentation_object_id(std::uint16_t id)
		{
			deviceValuePresentationObject = id;
		}

		const std::string DeviceValuePresentationObject::tableID = "DVP";

		DeviceValuePresentationObject::DeviceValuePresentationObject(std::string unitDesignator,
		                                                             std::int32_t offsetValue,
		                                                             float scaleFactor,
		                                                             std::uint8_t numberDecimals,
		                                                             std::uint16_t uniqueID) :
		  Object(unitDesignator, uniqueID),
		  offset(offsetValue),
		  scale(scaleFactor),
		  numberOfDecimals(numberDecimals)
		{
		}

		std::string DeviceValuePresentationObject::get_table_id() const
		{
			return tableID;
		}

		ObjectTypes DeviceValuePresentationObject::get_object_type() const
		{
			return ObjectTypes::DeviceValuePresentation;
		}

		std::vector<std::uint8_t> DeviceValuePresentationObject::get_binary_object() const
		{
			std::vector<std::uint8_t> retVal;

			retVal.reserve(16 + designator.size());

			retVal.push_back(tableID[0]);
			retVal.push_back(tableID[1]);
			retVal.push_back(tableID[2]);
			retVal.push_back(static_cast<std::uint8_t>(get_object_id() & 0xFF));
			retVal.push_back(static_cast<std::uint8_t>((get_object_id() >> 8) & 0xFF));
			retVal.push_back(static_cast<std::uint8_t>(offset & 0xFF));
			retVal.push_back(static_cast<std::uint8_t>((offset >> 8) & 0xFF));
			retVal.push_back(static_cast<std::uint8_t>((offset >> 16) & 0xFF));
			retVal.push_back(static_cast<std::uint8_t>((offset >> 24) & 0xFF));
			static_assert(sizeof(float) == 4, "Float must be 4 bytes");
			std::array<std::uint8_t, sizeof(float)> floatBytes = { 0 };
			memcpy(floatBytes.data(), &scale, sizeof(float));

			if (is_big_endian())
			{
				std::reverse(floatBytes.begin(), floatBytes.end());
			}

			for (std::uint_fast8_t i = 0; i < 4; i++)
			{
				retVal.push_back(floatBytes[i]);
			}
			retVal.push_back(numberOfDecimals);
			retVal.push_back(static_cast<std::uint8_t>(designator.size()));
			for (std::size_t i = 0; i < designator.size(); i++)
			{
				retVal.push_back(designator[i]);
			}
			return retVal;
		}

		std::int32_t DeviceValuePresentationObject::get_offset() const
		{
			return offset;
		}

		void DeviceValuePresentationObject::set_offset(std::int32_t newOffset)
		{
			offset = newOffset;
		}

		float DeviceValuePresentationObject::get_scale() const
		{
			return scale;
		}

		void DeviceValuePresentationObject::set_scale(float newScale)
		{
			scale = newScale;
		}

		std::uint8_t DeviceValuePresentationObject::get_number_of_decimals() const
		{
			return numberOfDecimals;
		}

		void DeviceValuePresentationObject::set_number_of_decimals(std::uint8_t decimals)
		{
			numberOfDecimals = decimals;
		}

	} // namespace task_controller_object
} // namespace isobus
