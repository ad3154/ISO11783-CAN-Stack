//================================================================================================
/// @file isobus_virtual_terminal_objects.cpp
///
/// @brief Implements VT server object pool objects.
/// @author Adrian Del Grosso
///
/// @copyright 2023 The Open-Agriculture Developers
//================================================================================================
#include "isobus/isobus/isobus_virtual_terminal_objects.hpp"
#include <iostream>
#include <ostream>
#include "isobus/isobus/isobus_virtual_terminal_server_managed_working_set.hpp"

namespace isobus
{
	VTColourTable::VTColourTable()
	{
		// The table can be altered at runtime. Init here to VT standard
		colourTable[0] = VTColourVector(0.0f, 0.0f, 0.0f); // Black
		colourTable[1] = VTColourVector(1.0f, 1.0f, 1.0f); // White
		colourTable[2] = VTColourVector(0.0f, (153.0f / 255.0f), 0.0f); // Green
		colourTable[3] = VTColourVector(0.0f, (153.0f / 255.0f), (153.0f / 255.0f)); // Teal
		colourTable[4] = VTColourVector((153.0f / 255.0f), 0.0f, 0.0f); // Maroon
		colourTable[5] = VTColourVector((153.0f / 255.0f), 0.0f, (153.0f / 255.0f)); // Purple
		colourTable[6] = VTColourVector((153.0f / 255.0f), (153.0f / 255.0f), 0.0f); // Olive
		colourTable[7] = VTColourVector((204.0f / 255.0f), (204.0f / 255.0f), (204.0f / 255.0f)); // Silver
		colourTable[8] = VTColourVector((153.0f / 255.0f), (153.0f / 255.0f), (153.0f / 255.0f)); // Grey
		colourTable[9] = VTColourVector(0.0f, 0.0f, 1.0f); // Blue
		colourTable[10] = VTColourVector(0.0f, 1.0f, 0.0f); // Lime
		colourTable[11] = VTColourVector(0.0f, 1.0f, 1.0f); // Cyan
		colourTable[12] = VTColourVector(1.0f, 0.0f, 0.0f); // Red
		colourTable[13] = VTColourVector(1.0f, 0.0f, 1.0f); // Magenta
		colourTable[14] = VTColourVector(1.0f, 1.0f, 0.0f); // Yellow
		colourTable[15] = VTColourVector(0.0f, 0.0f, (153.0f / 255.0f)); // Navy

		// This section of the table increases with a pattern
		for (std::uint8_t i = 16; i <= 231; i++)
		{
			std::uint8_t index = i - 16;

			std::uint32_t redCounter = (index / 36);
			std::uint32_t greenCounter = ((index / 6) % 6);
			std::uint32_t blueCounter = (index % 6);

			colourTable[i] = VTColourVector((51.0f * (redCounter) / 255.0f), ((51.0f * (greenCounter)) / 255.0f), ((51.0f * blueCounter) / 255.0f));
		}

		// Proprietary colors copied it over manually from the Jetter Iso Designer color palette
		colourTable[232] = VTColourVector(0.039f, 0.039f, 0.039f);
		colourTable[233] = VTColourVector(0.082f, 0.082f, 0.082f);
		colourTable[234] = VTColourVector(0.122f, 0.122f, 0.122f);
		colourTable[235] = VTColourVector(0.165f, 0.165f, 0.165f);
		colourTable[236] = VTColourVector(0.216f, 0.216f, 0.216f);
		colourTable[237] = VTColourVector(0.243f, 0.243f, 0.243f);
		colourTable[238] = VTColourVector(0.294f, 0.294f, 0.294f);
		colourTable[239] = VTColourVector(0.333f, 0.333f, 0.333f);
		colourTable[240] = VTColourVector(0.369f, 0.369f, 0.369f);
		colourTable[241] = VTColourVector(0.416f, 0.416f, 0.416f);
		colourTable[242] = VTColourVector(0.459f, 0.459f, 0.459f);
		colourTable[243] = VTColourVector(0.498f, 0.498f, 0.498f);
		colourTable[244] = VTColourVector(0.545f, 0.545f, 0.545f);
		colourTable[245] = VTColourVector(0.584f, 0.584f, 0.584f);
		colourTable[246] = VTColourVector(0.627f, 0.627f, 0.627f);
		colourTable[247] = VTColourVector(0.667f, 0.667f, 0.667f);
		colourTable[248] = VTColourVector(0.710f, 0.710f, 0.710f);
		colourTable[249] = VTColourVector(0.745f, 0.745f, 0.745f);
		colourTable[250] = VTColourVector(0.788f, 0.788f, 0.788f);
		colourTable[251] = VTColourVector(0.831f, 0.831f, 0.831f);
		colourTable[252] = VTColourVector(0.875f, 0.875f, 0.875f);
		colourTable[253] = VTColourVector(0.914f, 0.914f, 0.914f);
		colourTable[254] = VTColourVector(0.957f, 0.957f, 0.957f);
		colourTable[255] = VTColourVector(0.996f, 0.996f, 0.996f);
	}

	VTColourVector VTColourTable::get_colour(std::uint8_t colourIndex) const
	{
		return colourTable.at(colourIndex);
	}

	void VTColourTable::set_colour(std::uint8_t colourIndex, VTColourVector newColour)
	{
		colourTable.at(colourIndex) = newColour;
	}

	std::uint16_t VTObject::get_id() const
	{
		return objectID;
	}

	void VTObject::set_id(std::uint16_t value)
	{
		objectID = value;
	}

	std::uint16_t VTObject::get_width() const
	{
		return width;
	}

	void VTObject::set_width(std::uint16_t value)
	{
		width = value;
	}

	std::uint16_t VTObject::get_height() const
	{
		return height;
	}

	void VTObject::set_height(std::uint16_t value)
	{
		height = value;
	}

	std::uint8_t VTObject::get_background_color() const
	{
		return backgroundColor;
	}

	void VTObject::set_background_color(std::uint8_t value)
	{
		backgroundColor = value;
	}

	std::uint16_t VTObject::get_number_children() const
	{
		return static_cast<std::uint16_t>(children.size());
	}

	void VTObject::add_child(std::uint16_t objectID, std::int16_t relativeXLocation, std::int16_t relativeYLocation)
	{
		children.push_back(ChildObjectData(objectID, relativeXLocation, relativeYLocation));
	}

	std::uint16_t VTObject::get_child_id(std::uint16_t index) const
	{
		std::uint16_t retVal = NULL_OBJECT_ID;

		if (index < children.size())
		{
			retVal = children[index].id;
		}
		return retVal;
	}

	std::int16_t VTObject::get_child_x(std::uint16_t index) const
	{
		std::int16_t retVal = 0;

		if (index < children.size())
		{
			retVal = children[index].xLocation;
		}
		return retVal;
	}

	std::int16_t VTObject::get_child_y(std::uint16_t index) const
	{
		std::int16_t retVal = 0;

		if (index < children.size())
		{
			retVal = children[index].yLocation;
		}
		return retVal;
	}

	void VTObject::set_child_x(std::uint16_t index, std::int16_t xOffset)
	{
		if (index < children.size())
		{
			children.at(index).xLocation = xOffset;
		}
	}

	void VTObject::set_child_y(std::uint16_t index, std::int16_t yOffset)
	{
		if (index < children.size())
		{
			children.at(index).yLocation = yOffset;
		}
	}

	bool VTObject::offset_all_children_with_id(std::uint16_t childObjectID, std::int8_t xOffset, std::int8_t yOffset)
	{
		bool retVal = false;

		for (auto &child : children)
		{
			if (child.id == childObjectID)
			{
				child.xLocation += xOffset;
				child.yLocation += yOffset;
				retVal = true;
			}
		}
		return retVal;
	}

	void VTObject::remove_child(std::uint16_t objectIDToRemove, std::int16_t relativeXLocation, std::int16_t relativeYLocation)
	{
		for (auto child = children.begin(); child != children.end(); child++)
		{
			if ((child->id == objectIDToRemove) && (child->xLocation == relativeXLocation) && (child->yLocation == relativeYLocation))
			{
				children.erase(child);
				break;
			}
		}
	}

	void VTObject::pop_child()
	{
		if (!children.empty())
		{
			children.pop_back();
		}
	}

	std::uint8_t VTObject::get_number_macros() const
	{
		return static_cast<std::uint8_t>(macros.size());
	}

	void VTObject::add_macro(MacroMetadata macroToAdd)
	{
		macros.push_back(macroToAdd);
	}

	MacroMetadata VTObject::get_macro(std::uint8_t index) const
	{
		if (index < macros.size())
		{
			return macros[index];
		}
		else
		{
			return { EventID::Reserved, NULL_OBJECT_ID };
		}
	}

	std::shared_ptr<VTObject> VTObject::get_object_by_id(std::uint16_t objectID, const std::map<std::uint16_t, std::shared_ptr<VTObject>> &objectPool)
	{
		std::shared_ptr<VTObject> retVal = nullptr;

		if ((NULL_OBJECT_ID != objectID) && (objectPool.find(objectID) != objectPool.end()))
		{
			auto object = objectPool.at(objectID);
			if (nullptr != object)
			{
				retVal = object;
			}
		}
		return retVal;
	}

	VTObject::ChildObjectData::ChildObjectData(std::uint16_t objectId,
	                                           std::int16_t x,
	                                           std::int16_t y) :
	  id(objectId),
	  xLocation(x),
	  yLocation(y)
	{
	}

	VirtualTerminalObjectType WorkingSet::get_object_type() const
	{
		return VirtualTerminalObjectType::WorkingSet;
	}

	std::uint32_t WorkingSet::get_minumum_object_length() const
	{
		return MIN_OBJECT_LENGTH;
	}

	bool WorkingSet::get_is_valid(const std::map<std::uint16_t, std::shared_ptr<VTObject>> &objectPool) const
	{
		bool anyWrongChildType = false;

		for (const auto &child : children)
		{
			auto childObject = get_object_by_id(child.id, objectPool);
			if (nullptr != childObject)
			{
				switch (childObject->get_object_type())
				{
					case VirtualTerminalObjectType::OutputList:
					case VirtualTerminalObjectType::Container:
					case VirtualTerminalObjectType::OutputString:
					case VirtualTerminalObjectType::OutputNumber:
					case VirtualTerminalObjectType::OutputLine:
					case VirtualTerminalObjectType::OutputRectangle:
					case VirtualTerminalObjectType::OutputEllipse:
					case VirtualTerminalObjectType::OutputPolygon:
					case VirtualTerminalObjectType::OutputMeter:
					case VirtualTerminalObjectType::OutputLinearBarGraph:
					case VirtualTerminalObjectType::OutputArchedBarGraph:
					case VirtualTerminalObjectType::GraphicsContext:
					case VirtualTerminalObjectType::PictureGraphic:
					case VirtualTerminalObjectType::ObjectPointer:
					{
						// Valid Objects
					}
					break;

					default:
					{
						anyWrongChildType = true;
					}
					break;
				}
			}
		}
		return ((!anyWrongChildType) &&
		        (NULL_OBJECT_ID != objectID));
	}

	bool WorkingSet::set_attribute(std::uint8_t attributeID, std::uint32_t rawAttributeData, const std::map<std::uint16_t, std::shared_ptr<VTObject>> &, AttributeError &returnedError)
	{
		bool retVal = false;

		if (attributeID < static_cast<std::uint8_t>(AttributeName::NumberOfAttributes))
		{
			switch (static_cast<AttributeName>(attributeID))
			{
				case AttributeName::BackgroundColour:
				{
					set_background_color(static_cast<std::uint8_t>(rawAttributeData));
					retVal = true;
				}
				break;

				case AttributeName::Selectable:
				{
					set_selectable(0 != rawAttributeData);
					retVal = true;
				}
				break;

				case AttributeName::ActiveMask:
				{
					set_active_mask(static_cast<std::uint16_t>(rawAttributeData));
					retVal = true;
				}
				break;

				default:
				{
					returnedError = AttributeError::InvalidAttributeID;
				}
				break;
			}
		}
		else
		{
			returnedError = AttributeError::InvalidAttributeID;
		}
		return retVal;
	}

	bool WorkingSet::get_attribute(std::uint8_t attributeID, std::uint32_t &returnedAttributeData) const
	{
		bool retVal = false;

		if (attributeID < static_cast<std::uint8_t>(AttributeName::NumberOfAttributes))
		{
			switch (attributeID)
			{
				case static_cast<std::uint8_t>(AttributeName::Type):
				{
					returnedAttributeData = static_cast<std::uint8_t>(get_object_type());
					retVal = true;
				}
				break;

				case static_cast<std::uint8_t>(AttributeName::BackgroundColour):
				{
					returnedAttributeData = get_background_color();
					retVal = true;
				}
				break;

				case static_cast<std::uint8_t>(AttributeName::Selectable):
				{
					returnedAttributeData = get_selectable();
					retVal = true;
				}
				break;

				case static_cast<std::uint8_t>(AttributeName::ActiveMask):
				{
					returnedAttributeData = get_active_mask();
					retVal = true;
				}
				break;

				default:
				{
					// Do nothing, return false
				}
				break;
			}
		}
		return retVal;
	}

	bool WorkingSet::get_selectable() const
	{
		return selectable;
	}

	void WorkingSet::set_selectable(bool value)
	{
		selectable = value;
	}

	std::uint16_t WorkingSet::get_active_mask() const
	{
		return activeMask;
	}

	void WorkingSet::set_active_mask(std::uint16_t value)
	{
		activeMask = value;
	}

	VirtualTerminalObjectType DataMask::get_object_type() const
	{
		return VirtualTerminalObjectType::DataMask;
	}

	std::uint32_t DataMask::get_minumum_object_length() const
	{
		return MIN_OBJECT_LENGTH;
	}

	bool DataMask::get_is_valid(const std::map<std::uint16_t, std::shared_ptr<VTObject>> &objectPool) const
	{
		bool anyWrongChildType = false;
		std::uint8_t numberOfSoftKeyMasks = 0;

		for (auto &child : children)
		{
			auto childObject = get_object_by_id(child.id, objectPool);
			if (nullptr != childObject)
			{
				switch (childObject->get_object_type())
				{
					case VirtualTerminalObjectType::WorkingSet:
					case VirtualTerminalObjectType::Button:
					case VirtualTerminalObjectType::InputBoolean:
					case VirtualTerminalObjectType::InputString:
					case VirtualTerminalObjectType::InputNumber:
					case VirtualTerminalObjectType::OutputString:
					case VirtualTerminalObjectType::InputList:
					case VirtualTerminalObjectType::OutputNumber:
					case VirtualTerminalObjectType::OutputList:
					case VirtualTerminalObjectType::OutputLine:
					case VirtualTerminalObjectType::OutputRectangle:
					case VirtualTerminalObjectType::OutputEllipse:
					case VirtualTerminalObjectType::OutputPolygon:
					case VirtualTerminalObjectType::OutputMeter:
					case VirtualTerminalObjectType::OutputLinearBarGraph:
					case VirtualTerminalObjectType::OutputArchedBarGraph:
					case VirtualTerminalObjectType::GraphicsContext:
					case VirtualTerminalObjectType::Animation:
					case VirtualTerminalObjectType::PictureGraphic:
					case VirtualTerminalObjectType::ObjectPointer:
					case VirtualTerminalObjectType::ExternalObjectPointer:
					case VirtualTerminalObjectType::AuxiliaryFunctionType2:
					case VirtualTerminalObjectType::AuxiliaryInputType2:
					case VirtualTerminalObjectType::AuxiliaryControlDesignatorType2:
					case VirtualTerminalObjectType::Macro:
					{
						// Valid Objects
					}
					break;

					case VirtualTerminalObjectType::SoftKeyMask:
					{
						// Valid Objects
						numberOfSoftKeyMasks++;
					}
					break;

					default:
					{
						anyWrongChildType = true;
					}
					break;
				}

				if (numberOfSoftKeyMasks > 1)
				{
					anyWrongChildType = true;
				}
			}
		}

		return ((!anyWrongChildType) &&
		        (NULL_OBJECT_ID != objectID));
	}

	bool DataMask::set_attribute(std::uint8_t attributeID, std::uint32_t rawAttributeData, const std::map<std::uint16_t, std::shared_ptr<VTObject>> &objectPool, AttributeError &returnedError)
	{
		bool retVal = false;

		if (attributeID < static_cast<std::uint8_t>(DataMask::AttributeName::NumberOfAttributes))
		{
			switch (static_cast<DataMask::AttributeName>(attributeID))
			{
				case DataMask::AttributeName::BackgroundColour:
				{
					set_background_color(static_cast<std::uint8_t>(rawAttributeData));
					retVal = true;
				}
				break;

				case DataMask::AttributeName::SoftKeyMask:
				{
					returnedError = AttributeError::InvalidAttributeID;
					retVal = change_soft_key_mask(static_cast<std::uint16_t>(rawAttributeData), objectPool);
				}
				break;

				default:
				{
					returnedError = AttributeError::InvalidAttributeID;
				}
				break;
			}
		}
		else
		{
			returnedError = AttributeError::InvalidAttributeID;
		}
		return retVal;
	}

	bool DataMask::get_attribute(std::uint8_t attributeID, std::uint32_t &returnedAttributeData) const
	{
		bool retVal = false;

		if (attributeID < static_cast<std::uint8_t>(AttributeName::NumberOfAttributes))
		{
			switch (attributeID)
			{
				case static_cast<std::uint8_t>(AttributeName::Type):
				{
					returnedAttributeData = static_cast<std::uint8_t>(get_object_type());
					retVal = true;
				}
				break;

				case static_cast<std::uint8_t>(AttributeName::BackgroundColour):
				{
					returnedAttributeData = get_background_color();
					retVal = true;
				}
				break;

				case static_cast<std::uint8_t>(AttributeName::SoftKeyMask):
				{
					returnedAttributeData = get_soft_key_mask();
					retVal = true;
				}
				break;

				default:
				{
					// Do nothing, return false
				}
				break;
			}
		}
		return retVal;
	}

	bool DataMask::change_soft_key_mask(std::uint16_t newMaskID, const std::map<std::uint16_t, std::shared_ptr<VTObject>> &objectPool)
	{
		bool retVal = false;

		if (NULL_OBJECT_ID == newMaskID)
		{
			set_soft_key_mask(newMaskID);
			retVal = true;
		}
		else if ((objectPool.end() != objectPool.find(newMaskID)) &&
		         (nullptr != objectPool.at(newMaskID)) &&
		         (VirtualTerminalObjectType::SoftKeyMask == objectPool.at(newMaskID)->get_object_type()))
		{
			set_soft_key_mask(newMaskID);
			retVal = true;
		}

		return retVal;
	}

	void DataMask::set_soft_key_mask(std::uint16_t newMaskID)
	{
		softKeyMaskObjectID = newMaskID;
	}

	std::uint16_t DataMask::get_soft_key_mask() const
	{
		return softKeyMaskObjectID;
	}

	VirtualTerminalObjectType AlarmMask::get_object_type() const
	{
		return VirtualTerminalObjectType::AlarmMask;
	}

	std::uint32_t AlarmMask::get_minumum_object_length() const
	{
		return MIN_OBJECT_LENGTH;
	}

	bool AlarmMask::get_is_valid(const std::map<std::uint16_t, std::shared_ptr<VTObject>> &objectPool) const
	{
		bool anyWrongChildType = false;

		for (auto &child : children)
		{
			auto childObject = get_object_by_id(child.id, objectPool);
			if (nullptr != childObject)
			{
				switch (childObject->get_object_type())
				{
					case VirtualTerminalObjectType::WorkingSet:
					case VirtualTerminalObjectType::Button:
					case VirtualTerminalObjectType::InputBoolean:
					case VirtualTerminalObjectType::InputString:
					case VirtualTerminalObjectType::InputNumber:
					case VirtualTerminalObjectType::OutputString:
					case VirtualTerminalObjectType::InputList:
					case VirtualTerminalObjectType::OutputNumber:
					case VirtualTerminalObjectType::OutputList:
					case VirtualTerminalObjectType::OutputLine:
					case VirtualTerminalObjectType::OutputRectangle:
					case VirtualTerminalObjectType::OutputEllipse:
					case VirtualTerminalObjectType::OutputPolygon:
					case VirtualTerminalObjectType::OutputMeter:
					case VirtualTerminalObjectType::OutputLinearBarGraph:
					case VirtualTerminalObjectType::OutputArchedBarGraph:
					case VirtualTerminalObjectType::GraphicsContext:
					case VirtualTerminalObjectType::Animation:
					case VirtualTerminalObjectType::PictureGraphic:
					case VirtualTerminalObjectType::ObjectPointer:
					case VirtualTerminalObjectType::ExternalObjectPointer:
					case VirtualTerminalObjectType::AuxiliaryFunctionType2:
					case VirtualTerminalObjectType::AuxiliaryInputType2:
					case VirtualTerminalObjectType::AuxiliaryControlDesignatorType2:
					case VirtualTerminalObjectType::Macro:
					case VirtualTerminalObjectType::SoftKeyMask:
					{
						// Valid Objects
					}
					break;

					default:
					{
						anyWrongChildType = true;
					}
					break;
				}
			}
		}
		return ((!anyWrongChildType) &&
		        (NULL_OBJECT_ID != objectID));
	}

	bool AlarmMask::set_attribute(std::uint8_t attributeID, std::uint32_t rawAttributeData, const std::map<std::uint16_t, std::shared_ptr<VTObject>> &objectPool, AttributeError &returnedError)
	{
		bool retVal = false;

		if (attributeID < static_cast<std::uint8_t>(AlarmMask::AttributeName::NumberOfAttributes))
		{
			switch (static_cast<AlarmMask::AttributeName>(attributeID))
			{
				case AlarmMask::AttributeName::BackgroundColour:
				{
					set_background_color(static_cast<std::uint8_t>(rawAttributeData));
					retVal = true;
				}
				break;

				case AlarmMask::AttributeName::SoftKeyMask:
				{
					returnedError = AttributeError::AnyOtherError;
					retVal = change_soft_key_mask(static_cast<std::uint16_t>(rawAttributeData), objectPool);
				}
				break;

				case AlarmMask::AttributeName::Priority:
				{
					if (rawAttributeData <= static_cast<std::uint8_t>(AlarmMask::Priority::Low))
					{
						set_mask_priority(static_cast<AlarmMask::Priority>(rawAttributeData));
						retVal = true;
					}
					else
					{
						returnedError = AttributeError::AnyOtherError;
					}
				}
				break;

				case AlarmMask::AttributeName::AcousticSignal:
				{
					if (rawAttributeData <= static_cast<std::uint8_t>(AlarmMask::AcousticSignal::None))
					{
						set_signal_priority(static_cast<AlarmMask::AcousticSignal>(rawAttributeData));
						retVal = true;
					}
					else
					{
						returnedError = AttributeError::AnyOtherError;
					}
				}
				break;

				default:
				{
					returnedError = AttributeError::InvalidAttributeID;
				}
				break;
			}
		}
		else
		{
			returnedError = AttributeError::InvalidAttributeID;
		}
		return retVal;
	}

	bool AlarmMask::get_attribute(std::uint8_t attributeID, std::uint32_t &returnedAttributeData) const
	{
		bool retVal = false;

		if (attributeID < static_cast<std::uint8_t>(AttributeName::NumberOfAttributes))
		{
			switch (attributeID)
			{
				case static_cast<std::uint8_t>(AttributeName::Type):
				{
					returnedAttributeData = static_cast<std::uint8_t>(get_object_type());
					retVal = true;
				}
				break;

				case static_cast<std::uint8_t>(AttributeName::BackgroundColour):
				{
					returnedAttributeData = get_background_color();
					retVal = true;
				}
				break;

				case static_cast<std::uint8_t>(AttributeName::SoftKeyMask):
				{
					returnedAttributeData = get_soft_key_mask();
					retVal = true;
				}
				break;

				case static_cast<std::uint8_t>(AttributeName::Priority):
				{
					returnedAttributeData = static_cast<std::uint8_t>(get_mask_priority());
					retVal = true;
				}
				break;

				case static_cast<std::uint8_t>(AttributeName::AcousticSignal):
				{
					returnedAttributeData = static_cast<std::uint8_t>(get_signal_priority());
					retVal = true;
				}
				break;

				default:
				{
					// Do nothing, return false
				}
				break;
			}
		}
		return retVal;
	}

	AlarmMask::Priority AlarmMask::get_mask_priority() const
	{
		return maskPriority;
	}

	void AlarmMask::set_mask_priority(Priority value)
	{
		maskPriority = value;
	}

	AlarmMask::AcousticSignal AlarmMask::get_signal_priority() const
	{
		return signalPriority;
	}

	void AlarmMask::set_signal_priority(AcousticSignal value)
	{
		signalPriority = value;
	}

	bool AlarmMask::change_soft_key_mask(std::uint16_t newMaskID, const std::map<std::uint16_t, std::shared_ptr<VTObject>> &objectPool)
	{
		bool retVal = false;

		if (NULL_OBJECT_ID == newMaskID)
		{
			set_soft_key_mask(newMaskID);
			retVal = true;
		}
		else if ((nullptr != objectPool.at(newMaskID)) &&
		         (VirtualTerminalObjectType::SoftKeyMask == objectPool.at(newMaskID)->get_object_type()))
		{
			set_soft_key_mask(newMaskID);
			retVal = true;
		}
		return retVal;
	}

	void AlarmMask::set_soft_key_mask(std::uint16_t newMaskID)
	{
		softKeyMask = newMaskID;
	}

	std::uint16_t AlarmMask::get_soft_key_mask() const
	{
		return softKeyMask;
	}

	VirtualTerminalObjectType Container::get_object_type() const
	{
		return VirtualTerminalObjectType::Container;
	}

	std::uint32_t Container::get_minumum_object_length() const
	{
		return MIN_OBJECT_LENGTH;
	}

	bool Container::get_is_valid(const std::map<std::uint16_t, std::shared_ptr<VTObject>> &objectPool) const
	{
		bool anyWrongChildType = false;

		for (auto &child : children)
		{
			auto childObject = get_object_by_id(child.id, objectPool);
			if (nullptr != childObject)
			{
				switch (childObject->get_object_type())
				{
					case VirtualTerminalObjectType::WorkingSet:
					case VirtualTerminalObjectType::Container:
					case VirtualTerminalObjectType::Button:
					case VirtualTerminalObjectType::InputBoolean:
					case VirtualTerminalObjectType::InputString:
					case VirtualTerminalObjectType::InputNumber:
					case VirtualTerminalObjectType::InputList:
					case VirtualTerminalObjectType::OutputString:
					case VirtualTerminalObjectType::OutputNumber:
					case VirtualTerminalObjectType::OutputList:
					case VirtualTerminalObjectType::OutputLine:
					case VirtualTerminalObjectType::OutputRectangle:
					case VirtualTerminalObjectType::OutputEllipse:
					case VirtualTerminalObjectType::OutputPolygon:
					case VirtualTerminalObjectType::OutputMeter:
					case VirtualTerminalObjectType::GraphicsContext:
					case VirtualTerminalObjectType::OutputArchedBarGraph:
					case VirtualTerminalObjectType::OutputLinearBarGraph:
					case VirtualTerminalObjectType::Animation:
					case VirtualTerminalObjectType::PictureGraphic:
					case VirtualTerminalObjectType::ObjectPointer:
					case VirtualTerminalObjectType::ExternalObjectPointer:
					case VirtualTerminalObjectType::AuxiliaryFunctionType2:
					case VirtualTerminalObjectType::AuxiliaryInputType2:
					case VirtualTerminalObjectType::AuxiliaryControlDesignatorType2:
					case VirtualTerminalObjectType::Macro:
					{
						// Valid Child Object
					}
					break;

					default:
					{
						anyWrongChildType = true;
					}
					break;
				}
			}
		}
		return ((!anyWrongChildType) &&
		        (NULL_OBJECT_ID != objectID));
	}

	bool Container::set_attribute(std::uint8_t, std::uint32_t, const std::map<std::uint16_t, std::shared_ptr<VTObject>> &, AttributeError &returnedError)
	{
		// All attributes are read only
		returnedError = AttributeError::InvalidAttributeID;
		return false;
	}

	bool Container::get_attribute(std::uint8_t attributeID, std::uint32_t &returnedAttributeData) const
	{
		bool retVal = false;

		if (attributeID < static_cast<std::uint8_t>(AttributeName::NumberOfAttributes))
		{
			switch (attributeID)
			{
				case static_cast<std::uint8_t>(AttributeName::Type):
				{
					returnedAttributeData = static_cast<std::uint8_t>(get_object_type());
					retVal = true;
				}
				break;

				case static_cast<std::uint8_t>(AttributeName::Width):
				{
					returnedAttributeData = get_width();
					retVal = true;
				}
				break;

				case static_cast<std::uint8_t>(AttributeName::Height):
				{
					returnedAttributeData = get_height();
					retVal = true;
				}
				break;

				case static_cast<std::uint8_t>(AttributeName::Hidden):
				{
					returnedAttributeData = get_hidden();
					retVal = true;
				}
				break;

				default:
				{
					// Do nothing, return false
				}
				break;
			}
		}
		return retVal;
	}

	bool Container::get_hidden() const
	{
		return hidden;
	}

	void Container::set_hidden(bool value)
	{
		hidden = value;
	}

	VirtualTerminalObjectType SoftKeyMask::get_object_type() const
	{
		return VirtualTerminalObjectType::SoftKeyMask;
	}

	std::uint32_t SoftKeyMask::get_minumum_object_length() const
	{
		return MIN_OBJECT_LENGTH;
	}

	bool SoftKeyMask::get_is_valid(const std::map<std::uint16_t, std::shared_ptr<VTObject>> &objectPool) const
	{
		bool anyWrongChildType = false;

		for (auto &child : children)
		{
			auto childObject = get_object_by_id(child.id, objectPool);
			if (nullptr != childObject)
			{
				switch (childObject->get_object_type())
				{
					case VirtualTerminalObjectType::ObjectPointer:
					case VirtualTerminalObjectType::ExternalObjectPointer:
					case VirtualTerminalObjectType::Key:
					case VirtualTerminalObjectType::Macro:
					{
						// Valid Child Object
					}
					break;

					default:
					{
						anyWrongChildType = true;
					}
					break;
				}
			}
		}
		return ((!anyWrongChildType) &&
		        (NULL_OBJECT_ID != objectID));
	}

	bool SoftKeyMask::set_attribute(std::uint8_t attributeID, std::uint32_t rawAttributeData, const std::map<std::uint16_t, std::shared_ptr<VTObject>> &, AttributeError &returnedError)
	{
		bool retVal = false;

		if (attributeID < static_cast<std::uint8_t>(AttributeName::NumberOfAttributes))
		{
			switch (static_cast<AttributeName>(attributeID))
			{
				case AttributeName::BackgroundColour:
				{
					set_background_color(static_cast<std::uint8_t>(rawAttributeData));
					retVal = true;
				}
				break;

				default:
				{
					returnedError = AttributeError::InvalidAttributeID;
				}
				break;
			}
		}
		else
		{
			returnedError = AttributeError::InvalidAttributeID;
		}
		return retVal;
	}

	bool SoftKeyMask::get_attribute(std::uint8_t attributeID, std::uint32_t &returnedAttributeData) const
	{
		bool retVal = false;

		if (attributeID < static_cast<std::uint8_t>(AttributeName::NumberOfAttributes))
		{
			switch (attributeID)
			{
				case static_cast<std::uint8_t>(AttributeName::Type):
				{
					returnedAttributeData = static_cast<std::uint8_t>(get_object_type());
					retVal = true;
				}
				break;

				case static_cast<std::uint8_t>(AttributeName::BackgroundColour):
				{
					returnedAttributeData = get_background_color();
					retVal = true;
				}
				break;

				default:
				{
					// Do nothing, return false
				}
				break;
			}
		}
		return retVal;
	}

	VirtualTerminalObjectType Key::get_object_type() const
	{
		return VirtualTerminalObjectType::Key;
	}

	std::uint32_t Key::get_minumum_object_length() const
	{
		return MIN_OBJECT_LENGTH;
	}

	bool Key::get_is_valid(const std::map<std::uint16_t, std::shared_ptr<VTObject>> &objectPool) const
	{
		bool anyWrongChildType = false;

		for (auto &child : children)
		{
			auto childObject = get_object_by_id(child.id, objectPool);
			if (nullptr != childObject)
			{
				switch (childObject->get_object_type())
				{
					case VirtualTerminalObjectType::WorkingSet:
					case VirtualTerminalObjectType::Container:
					case VirtualTerminalObjectType::OutputString:
					case VirtualTerminalObjectType::OutputNumber:
					case VirtualTerminalObjectType::OutputList:
					case VirtualTerminalObjectType::OutputLine:
					case VirtualTerminalObjectType::OutputRectangle:
					case VirtualTerminalObjectType::OutputEllipse:
					case VirtualTerminalObjectType::OutputPolygon:
					case VirtualTerminalObjectType::OutputMeter:
					case VirtualTerminalObjectType::GraphicsContext:
					case VirtualTerminalObjectType::OutputArchedBarGraph:
					case VirtualTerminalObjectType::OutputLinearBarGraph:
					case VirtualTerminalObjectType::Animation:
					case VirtualTerminalObjectType::PictureGraphic:
					case VirtualTerminalObjectType::ObjectPointer:
					case VirtualTerminalObjectType::ExternalObjectPointer:
					case VirtualTerminalObjectType::Macro:
					{
						// Valid Child Object
					}
					break;

					default:
					{
						anyWrongChildType = true;
					}
					break;
				}
			}
		}
		return ((!anyWrongChildType) &&
		        (NULL_OBJECT_ID != objectID));
	}

	bool Key::set_attribute(std::uint8_t attributeID, std::uint32_t rawAttributeData, const std::map<std::uint16_t, std::shared_ptr<VTObject>> &, AttributeError &returnedError)
	{
		bool retVal = false;

		if (attributeID < static_cast<std::uint8_t>(AttributeName::NumberOfAttributes))
		{
			switch (static_cast<AttributeName>(attributeID))
			{
				case AttributeName::BackgroundColour:
				{
					set_background_color(static_cast<std::uint8_t>(rawAttributeData));
					retVal = true;
				}
				break;

				case AttributeName::KeyCode:
				{
					set_key_code(static_cast<std::uint8_t>(rawAttributeData));
					retVal = true;
				}
				break;

				default:
				{
					returnedError = AttributeError::InvalidAttributeID;
				}
				break;
			}
		}
		else
		{
			returnedError = AttributeError::InvalidAttributeID;
		}
		return retVal;
	}

	bool Key::get_attribute(std::uint8_t attributeID, std::uint32_t &returnedAttributeData) const
	{
		bool retVal = false;

		if (attributeID < static_cast<std::uint8_t>(AttributeName::NumberOfAttributes))
		{
			switch (attributeID)
			{
				case static_cast<std::uint8_t>(AttributeName::Type):
				{
					returnedAttributeData = static_cast<std::uint8_t>(get_object_type());
					retVal = true;
				}
				break;

				case static_cast<std::uint8_t>(AttributeName::BackgroundColour):
				{
					returnedAttributeData = get_background_color();
					retVal = true;
				}
				break;

				case static_cast<std::uint8_t>(AttributeName::KeyCode):
				{
					returnedAttributeData = get_key_code();
					retVal = true;
				}
				break;

				default:
				{
					// Do nothing, return false
				}
				break;
			}
		}
		return retVal;
	}

	std::uint8_t Key::get_key_code() const
	{
		return keyCode;
	}

	void Key::set_key_code(std::uint8_t value)
	{
		keyCode = value;
	}

	VirtualTerminalObjectType KeyGroup::get_object_type() const
	{
		return VirtualTerminalObjectType::KeyGroup;
	}

	std::uint32_t KeyGroup::get_minumum_object_length() const
	{
		return MIN_OBJECT_LENGTH;
	}

	bool KeyGroup::get_is_valid(const std::map<std::uint16_t, std::shared_ptr<VTObject>> &objectPool) const
	{
		bool anyWrongChildType = false;

		if (NULL_OBJECT_ID != get_name_object_id())
		{
			for (auto &child : children)
			{
				auto childObject = get_object_by_id(child.id, objectPool);
				if (nullptr != childObject)
				{
					switch (childObject->get_object_type())
					{
						case VirtualTerminalObjectType::Key:
						case VirtualTerminalObjectType::Macro:
						{
							// Key and macro are always valid
						}
						break;

						case VirtualTerminalObjectType::ObjectPointer:
						{
							auto objectPointer = std::static_pointer_cast<ObjectPointer>(childObject);

							if (NULL_OBJECT_ID != objectPointer->get_value())
							{
								if ((nullptr != get_object_by_id(objectPointer->get_value(), objectPool)) &&
								    (VirtualTerminalObjectType::Key == get_object_by_id(objectPointer->get_value(), objectPool)->get_object_type()))
								{
									// Valid Child Object
								}
								else
								{
									anyWrongChildType = true;
								}
							}
							else
							{
								// If there's no children, then it's valid
							}
						}
						break;

						default:
						{
							anyWrongChildType = true;
						}
						break;
					}
				}
			}

			if (!validate_name(nameID, objectPool))
			{
				anyWrongChildType = true;
			}
		}
		else
		{
			anyWrongChildType = true;
		}
		return ((!anyWrongChildType) &&
		        (NULL_OBJECT_ID != objectID));
	}

	bool KeyGroup::set_attribute(std::uint8_t attributeID, std::uint32_t rawAttributeData, const std::map<std::uint16_t, std::shared_ptr<VTObject>> &objectPool, AttributeError &returnedError)
	{
		bool retVal = false;

		if (attributeID < static_cast<std::uint8_t>(AttributeName::NumberOfAttributes))
		{
			switch (static_cast<AttributeName>(attributeID))
			{
				case AttributeName::Options:
				{
					set_options(static_cast<std::uint8_t>(rawAttributeData));
					retVal = true;
				}
				break;

				case AttributeName::Name:
				{
					returnedError = AttributeError::InvalidValue;

					if (objectPool.find(objectID) != objectPool.end())
					{
						auto newName = static_cast<std::uint16_t>(rawAttributeData);
						auto newNameObject = objectPool.at(newName);

						if (validate_name(newName, objectPool))
						{
							nameID = newName;
							retVal = true;
						}
					}
				}
				break;

				default:
				{
					returnedError = AttributeError::InvalidAttributeID;
				}
				break;
			}
		}
		else
		{
			returnedError = AttributeError::InvalidAttributeID;
		}
		return retVal;
	}

	bool KeyGroup::get_attribute(std::uint8_t attributeID, std::uint32_t &returnedAttributeData) const
	{
		bool retVal = false;

		if (attributeID < static_cast<std::uint8_t>(AttributeName::NumberOfAttributes))
		{
			switch (attributeID)
			{
				case static_cast<std::uint8_t>(AttributeName::Type):
				{
					returnedAttributeData = static_cast<std::uint8_t>(get_object_type());
					retVal = true;
				}
				break;

				case static_cast<std::uint8_t>(AttributeName::Options):
				{
					returnedAttributeData = optionsBitfield;
					retVal = true;
				}
				break;

				case static_cast<std::uint8_t>(AttributeName::Name):
				{
					returnedAttributeData = get_name_object_id();
					retVal = true;
				}
				break;

				default:
				{
					// Do nothing, return false
				}
				break;
			}
		}
		return retVal;
	}

	std::uint16_t KeyGroup::get_key_group_icon() const
	{
		return keyGroupIcon;
	}

	void KeyGroup::set_key_group_icon(std::uint16_t value)
	{
		keyGroupIcon = value;
	}

	bool KeyGroup::get_option(Options option) const
	{
		return (0 != ((1 << static_cast<std::uint8_t>(option)) & optionsBitfield));
	}

	void KeyGroup::set_options(std::uint8_t value)
	{
		optionsBitfield = value;
	}

	void KeyGroup::set_option(Options option, bool value)
	{
		if (value)
		{
			optionsBitfield |= (1 << static_cast<std::uint8_t>(option));
		}
		else
		{
			optionsBitfield &= ~(1 << static_cast<std::uint8_t>(option));
		}
	}

	std::uint16_t KeyGroup::get_name_object_id() const
	{
		return nameID;
	}

	void KeyGroup::set_name_object_id(std::uint16_t value)
	{
		// This value cannot be NULL_OBJECT_ID after it has been set!
		if (NULL_OBJECT_ID != value)
		{
			nameID = value;
		}
	}

	bool KeyGroup::validate_name(std::uint16_t nameIDToValidate, const std::map<std::uint16_t, std::shared_ptr<VTObject>> &objectPool) const
	{
		auto newNameObject = objectPool.at(nameIDToValidate);
		bool retVal = false;

		if ((NULL_OBJECT_ID != nameIDToValidate) &&
		    (nullptr != newNameObject) &&
		    ((VirtualTerminalObjectType::OutputString == newNameObject->get_object_type()) ||
		     (VirtualTerminalObjectType::ObjectPointer == newNameObject->get_object_type())))
		{
			if (VirtualTerminalObjectType::ObjectPointer == newNameObject->get_object_type())
			{
				if (newNameObject->get_number_children() > 0)
				{
					auto label = objectPool.at(std::static_pointer_cast<ObjectPointer>(newNameObject)->get_child_id(0));

					if ((nullptr != label) &&
					    (VirtualTerminalObjectType::OutputString == label->get_object_type()))
					{
						retVal = true;
					}
				}
			}
			else
			{
				retVal = true;
			}
		}
		return retVal;
	}

	VirtualTerminalObjectType Button::get_object_type() const
	{
		return VirtualTerminalObjectType::Button;
	}

	std::uint32_t Button::get_minumum_object_length() const
	{
		return MIN_OBJECT_LENGTH;
	}

	bool Button::get_is_valid(const std::map<std::uint16_t, std::shared_ptr<VTObject>> &objectPool) const
	{
		bool anyWrongChildType = false;

		for (auto &child : children)
		{
			auto childObject = get_object_by_id(child.id, objectPool);
			if (nullptr != childObject)
			{
				switch (childObject->get_object_type())
				{
					case VirtualTerminalObjectType::WorkingSet:
					case VirtualTerminalObjectType::OutputList:
					case VirtualTerminalObjectType::Container:
					case VirtualTerminalObjectType::OutputString:
					case VirtualTerminalObjectType::OutputNumber:
					case VirtualTerminalObjectType::OutputLine:
					case VirtualTerminalObjectType::OutputRectangle:
					case VirtualTerminalObjectType::OutputEllipse:
					case VirtualTerminalObjectType::OutputPolygon:
					case VirtualTerminalObjectType::OutputMeter:
					case VirtualTerminalObjectType::OutputLinearBarGraph:
					case VirtualTerminalObjectType::OutputArchedBarGraph:
					case VirtualTerminalObjectType::GraphicsContext:
					case VirtualTerminalObjectType::PictureGraphic:
					case VirtualTerminalObjectType::ObjectPointer:
					case VirtualTerminalObjectType::Animation:
					case VirtualTerminalObjectType::Macro:
					{
						// Valid Child Object
					}
					break;

					default:
					{
						anyWrongChildType = true;
					}
					break;
				}
			}
		}
		return ((!anyWrongChildType) &&
		        (NULL_OBJECT_ID != objectID));
	}

	bool Button::set_attribute(std::uint8_t attributeID, std::uint32_t rawAttributeData, const std::map<std::uint16_t, std::shared_ptr<VTObject>> &, AttributeError &returnedError)
	{
		bool retVal = false;

		if (attributeID < static_cast<std::uint8_t>(AttributeName::NumberOfAttributes))
		{
			switch (static_cast<AttributeName>(attributeID))
			{
				case AttributeName::Width:
				{
					set_width(static_cast<std::uint16_t>(rawAttributeData));
					retVal = true;
				}
				break;

				case AttributeName::Height:
				{
					set_height(static_cast<std::uint16_t>(rawAttributeData));
					retVal = true;
				}
				break;

				case AttributeName::BackgroundColour:
				{
					set_background_color(static_cast<std::uint8_t>(rawAttributeData));
					retVal = true;
				}
				break;

				case AttributeName::BorderColour:
				{
					set_border_colour(static_cast<std::uint8_t>(rawAttributeData));
					retVal = true;
				}
				break;

				case AttributeName::KeyCode:
				{
					set_key_code(static_cast<std::uint8_t>(rawAttributeData));
					retVal = true;
				}
				break;

				case AttributeName::Options:
				{
					set_options(static_cast<std::uint8_t>(rawAttributeData));
					retVal = true;
				}
				break;

				default:
				{
					returnedError = AttributeError::InvalidAttributeID;
				}
				break;
			}
		}
		else
		{
			returnedError = AttributeError::InvalidAttributeID;
		}
		return retVal;
	}

	bool Button::get_attribute(std::uint8_t attributeID, std::uint32_t &returnedAttributeData) const
	{
		bool retVal = false;

		if (attributeID < static_cast<std::uint8_t>(AttributeName::NumberOfAttributes))
		{
			switch (attributeID)
			{
				case static_cast<std::uint8_t>(AttributeName::Type):
				{
					returnedAttributeData = static_cast<std::uint8_t>(get_object_type());
					retVal = true;
				}
				break;

				case static_cast<std::uint8_t>(AttributeName::Width):
				{
					returnedAttributeData = get_width();
					retVal = true;
				}
				break;

				case static_cast<std::uint8_t>(AttributeName::Height):
				{
					returnedAttributeData = get_height();
					retVal = true;
				}
				break;

				case static_cast<std::uint8_t>(AttributeName::BackgroundColour):
				{
					returnedAttributeData = get_background_color();
					retVal = true;
				}
				break;

				case static_cast<std::uint8_t>(AttributeName::BorderColour):
				{
					returnedAttributeData = get_border_colour();
					retVal = true;
				}
				break;

				case static_cast<std::uint8_t>(AttributeName::KeyCode):
				{
					returnedAttributeData = get_key_code();
					retVal = true;
				}
				break;

				case static_cast<std::uint8_t>(AttributeName::Options):
				{
					returnedAttributeData = optionsBitfield;
					retVal = true;
				}
				break;

				default:
				{
					// Do nothing, return false
				}
				break;
			}
		}
		return retVal;
	}

	std::uint8_t Button::get_key_code() const
	{
		return keyCode;
	}

	void Button::set_key_code(std::uint8_t value)
	{
		keyCode = value;
	}

	std::uint8_t Button::get_border_colour() const
	{
		return borderColour;
	}

	void Button::set_border_colour(std::uint8_t value)
	{
		borderColour = value;
	}

	bool Button::get_option(Options option) const
	{
		return (0 != ((1 << static_cast<std::uint8_t>(option)) & optionsBitfield));
	}

	void Button::set_options(std::uint8_t value)
	{
		optionsBitfield = value;
	}

	void Button::set_option(Options option, bool value)
	{
		if (value)
		{
			optionsBitfield |= (1 << static_cast<std::uint8_t>(option));
		}
		else
		{
			optionsBitfield &= ~(1 << static_cast<std::uint8_t>(option));
		}
	}

	VirtualTerminalObjectType InputBoolean::get_object_type() const
	{
		return VirtualTerminalObjectType::InputBoolean;
	}

	std::uint32_t InputBoolean::get_minumum_object_length() const
	{
		return MIN_OBJECT_LENGTH;
	}

	bool InputBoolean::get_is_valid(const std::map<std::uint16_t, std::shared_ptr<VTObject>> &objectPool) const
	{
		bool anyWrongChildType = false;

		// Verify the variable reference is a number variable or NULL_OBJECT_ID
		if (NULL_OBJECT_ID != get_variable_reference())
		{
			auto variableReference = get_object_by_id(get_variable_reference(), objectPool);

			if (nullptr != variableReference)
			{
				if (VirtualTerminalObjectType::NumberVariable != variableReference->get_object_type())
				{
					anyWrongChildType = true;
				}
			}
			else
			{
				anyWrongChildType = true;
			}
		}

		// Verify that the foreground colour is a font attribute or NULL_OBJECT_ID
		if (NULL_OBJECT_ID != get_foreground_colour_object_id())
		{
			auto foregroundColour = get_object_by_id(get_foreground_colour_object_id(), objectPool);

			if (nullptr != foregroundColour)
			{
				if (VirtualTerminalObjectType::FontAttributes != foregroundColour->get_object_type())
				{
					anyWrongChildType = true;
				}
			}
			else
			{
				anyWrongChildType = true;
			}
		}

		for (const auto &child : children)
		{
			auto childObject = get_object_by_id(child.id, objectPool);

			if ((nullptr == childObject) ||
			    (VirtualTerminalObjectType::Macro != childObject->get_object_type()))
			{
				anyWrongChildType = true;
				break;
			}
		}
		return (!anyWrongChildType);
	}

	bool InputBoolean::set_attribute(std::uint8_t attributeID, std::uint32_t rawAttributeData, const std::map<std::uint16_t, std::shared_ptr<VTObject>> &objectPool, AttributeError &returnedError)
	{
		bool retVal = false;

		if (attributeID < static_cast<std::uint8_t>(AttributeName::NumberOfAttributes))
		{
			switch (static_cast<AttributeName>(attributeID))
			{
				case AttributeName::BackgroundColour:
				{
					set_background_color(static_cast<std::uint8_t>(rawAttributeData));
					retVal = true;
				}
				break;

				case AttributeName::Width:
				{
					set_width(static_cast<std::uint16_t>(rawAttributeData));
					retVal = true;
				}
				break;

				case AttributeName::ForegroundColour:
				{
					returnedError = AttributeError::AnyOtherError;

					if (NULL_OBJECT_ID == rawAttributeData)
					{
						set_foreground_colour_object_id(static_cast<std::uint16_t>(rawAttributeData));
						retVal = true;
					}
					else if (objectPool.find(static_cast<std::uint16_t>(rawAttributeData)) != objectPool.end())
					{
						if (nullptr != objectPool.at(static_cast<std::uint16_t>(rawAttributeData)) &&
						    (VirtualTerminalObjectType::FontAttributes == objectPool.at(static_cast<std::uint16_t>(rawAttributeData))->get_object_type()))
						{
							set_foreground_colour_object_id(static_cast<std::uint16_t>(rawAttributeData));
							retVal = true;
						}
						else
						{
							returnedError = AttributeError::InvalidValue;
						}
					}
				}
				break;

				case AttributeName::VariableReference:
				{
					returnedError = AttributeError::AnyOtherError;

					if (NULL_OBJECT_ID == rawAttributeData)
					{
						set_variable_reference(static_cast<std::uint16_t>(rawAttributeData));
						retVal = true;
					}
					else if (objectPool.find(static_cast<std::uint16_t>(rawAttributeData)) != objectPool.end())
					{
						if (nullptr != objectPool.at(static_cast<std::uint16_t>(rawAttributeData)) &&
						    (VirtualTerminalObjectType::NumberVariable == objectPool.at(static_cast<std::uint16_t>(rawAttributeData))->get_object_type()))
						{
							set_variable_reference(static_cast<std::uint16_t>(rawAttributeData));
							retVal = true;
						}
						else
						{
							returnedError = AttributeError::InvalidValue;
						}
					}
				}
				break;

				case AttributeName::Value:
				{
					set_value(static_cast<std::uint8_t>(rawAttributeData));
					retVal = true;
				}
				break;

				case AttributeName::Enabled:
				{
					set_enabled(0 != rawAttributeData);
					retVal = true;
				}
				break;

				default:
				{
					returnedError = AttributeError::InvalidAttributeID;
				}
				break;
			}
		}
		else
		{
			returnedError = AttributeError::InvalidAttributeID;
		}
		return retVal;
	}

	bool InputBoolean::get_attribute(std::uint8_t attributeID, std::uint32_t &returnedAttributeData) const
	{
		bool retVal = false;

		if (attributeID < static_cast<std::uint8_t>(AttributeName::NumberOfAttributes))
		{
			switch (attributeID)
			{
				case static_cast<std::uint8_t>(AttributeName::Type):
				{
					returnedAttributeData = static_cast<std::uint8_t>(get_object_type());
					retVal = true;
				}
				break;

				case static_cast<std::uint8_t>(AttributeName::BackgroundColour):
				{
					returnedAttributeData = get_background_color();
					retVal = true;
				}
				break;

				case static_cast<std::uint8_t>(AttributeName::Width):
				{
					returnedAttributeData = get_width();
					retVal = true;
				}
				break;

				case static_cast<std::uint8_t>(AttributeName::ForegroundColour):
				{
					returnedAttributeData = get_foreground_colour_object_id();
					retVal = true;
				}
				break;

				case static_cast<std::uint8_t>(AttributeName::VariableReference):
				{
					returnedAttributeData = get_variable_reference();
					retVal = true;
				}
				break;

				case static_cast<std::uint8_t>(AttributeName::Value):
				{
					returnedAttributeData = get_value();
					retVal = true;
				}
				break;

				case static_cast<std::uint8_t>(AttributeName::Enabled):
				{
					returnedAttributeData = get_enabled();
					retVal = true;
				}
				break;

				default:
				{
					// Do nothing, return false
				}
				break;
			}
		}
		return retVal;
	}

	std::uint8_t InputBoolean::get_value() const
	{
		return value;
	}

	void InputBoolean::set_value(std::uint8_t inputValue)
	{
		value = inputValue;
	}

	bool InputBoolean::get_enabled() const
	{
		return enabled;
	}

	void InputBoolean::set_enabled(bool isEnabled)
	{
		enabled = isEnabled;
	}

	std::uint16_t InputBoolean::get_foreground_colour_object_id() const
	{
		return foregroundColourObjectID;
	}

	void InputBoolean::set_foreground_colour_object_id(std::uint16_t fontAttributeValue)
	{
		foregroundColourObjectID = fontAttributeValue;
	}

	std::uint16_t VTObjectWithVariableReference::get_variable_reference() const
	{
		return variableReference;
	}

	void VTObjectWithVariableReference::set_variable_reference(std::uint16_t variableValue)
	{
		variableReference = variableValue;
	}

	bool StringVTObject::get_option(Options option) const
	{
		return (0 != ((1 << static_cast<std::uint8_t>(option)) & optionsBitfield));
	}

	void StringVTObject::set_option(Options option, bool value)
	{
		if (value)
		{
			optionsBitfield |= (1 << static_cast<std::uint8_t>(option));
		}
		else
		{
			optionsBitfield &= ~(1 << static_cast<std::uint8_t>(option));
		}
	}

	VirtualTerminalObjectType InputString::get_object_type() const
	{
		return VirtualTerminalObjectType::InputString;
	}

	std::uint32_t InputString::get_minumum_object_length() const
	{
		return MIN_OBJECT_LENGTH;
	}

	bool InputString::get_is_valid(const std::map<std::uint16_t, std::shared_ptr<VTObject>> &objectPool) const
	{
		bool anyWrongChildType = false;

		if (NULL_OBJECT_ID != get_variable_reference())
		{
			auto objectToCheck = get_object_by_id(get_variable_reference(), objectPool);

			if (nullptr != objectToCheck)
			{
				if (VirtualTerminalObjectType::StringVariable != objectToCheck->get_object_type())
				{
					anyWrongChildType = true;
				}
			}
			else
			{
				anyWrongChildType = true;
			}
		}

		if (NULL_OBJECT_ID != get_input_attributes())
		{
			auto objectToCheck = get_object_by_id(get_input_attributes(), objectPool);

			if (nullptr != objectToCheck)
			{
				if (VirtualTerminalObjectType::InputAttributes != objectToCheck->get_object_type())
				{
					anyWrongChildType = true;
				}
			}
			else
			{
				anyWrongChildType = true;
			}
		}

		if (NULL_OBJECT_ID != get_font_attributes())
		{
			auto objectToCheck = get_object_by_id(get_font_attributes(), objectPool);

			if (nullptr != objectToCheck)
			{
				if (VirtualTerminalObjectType::FontAttributes != objectToCheck->get_object_type())
				{
					anyWrongChildType = true;
				}
			}
			else
			{
				anyWrongChildType = true;
			}
		}

		for (const auto &child : children)
		{
			auto childObject = get_object_by_id(child.id, objectPool);

			if ((nullptr == childObject) ||
			    (VirtualTerminalObjectType::Macro != childObject->get_object_type()))
			{
				anyWrongChildType = true;
				break;
			}
		}
		return (!anyWrongChildType);
	}

	bool InputString::set_attribute(std::uint8_t attributeID, std::uint32_t rawAttributeData, const std::map<std::uint16_t, std::shared_ptr<VTObject>> &objectPool, AttributeError &returnedError)
	{
		bool retVal = false;

		if (attributeID < static_cast<std::uint8_t>(AttributeName::NumberOfAttributes))
		{
			switch (static_cast<AttributeName>(attributeID))
			{
				case AttributeName::Width:
				{
					set_width(static_cast<std::uint16_t>(rawAttributeData));
					retVal = true;
				}
				break;

				case AttributeName::Height:
				{
					set_height(static_cast<std::uint16_t>(rawAttributeData));
					retVal = true;
				}
				break;

				case AttributeName::BackgroundColour:
				{
					set_background_color(static_cast<std::uint8_t>(rawAttributeData));
					retVal = true;
				}
				break;

				case AttributeName::FontAttributes:
				{
					auto fontAttributesObject = get_object_by_id(static_cast<std::uint16_t>(rawAttributeData), objectPool);

					if ((NULL_OBJECT_ID == rawAttributeData) ||
					    ((nullptr != fontAttributesObject) &&
					     (VirtualTerminalObjectType::FontAttributes == fontAttributesObject->get_object_type())))
					{
						set_font_attributes(static_cast<std::uint16_t>(rawAttributeData));
						retVal = true;
					}
					else
					{
						returnedError = AttributeError::InvalidValue;
					}
				}
				break;

				case AttributeName::InputAttributes:
				{
					auto inputAttributesObject = get_object_by_id(static_cast<std::uint16_t>(rawAttributeData), objectPool);

					if ((NULL_OBJECT_ID == rawAttributeData) ||
					    ((nullptr != inputAttributesObject) &&
					     (VirtualTerminalObjectType::InputAttributes == inputAttributesObject->get_object_type())))
					{
						set_input_attributes(static_cast<std::uint16_t>(rawAttributeData));
						retVal = true;
					}
					else
					{
						returnedError = AttributeError::InvalidValue;
					}
				}
				break;

				case AttributeName::Options:
				{
					set_options(static_cast<std::uint8_t>(rawAttributeData));
					retVal = true;
				}
				break;

				case AttributeName::VariableReference:
				{
					auto variableReferenceObject = get_object_by_id(static_cast<std::uint16_t>(rawAttributeData), objectPool);

					if ((NULL_OBJECT_ID == rawAttributeData) ||
					    ((nullptr != variableReferenceObject) &&
					     (VirtualTerminalObjectType::StringVariable == variableReferenceObject->get_object_type())))
					{
						set_variable_reference(static_cast<std::uint16_t>(rawAttributeData));
						retVal = true;
					}
					else
					{
						returnedError = AttributeError::InvalidValue;
					}
				}
				break;

				case AttributeName::Justification:
				{
					set_justification_bitfield(static_cast<std::uint8_t>(rawAttributeData));
					retVal = true;
				}
				break;

				case AttributeName::Enabled:
				{
					set_enabled(0 != rawAttributeData);
					retVal = true;
				}
				break;

				default:
				{
					returnedError = AttributeError::InvalidAttributeID;
				}
				break;
			}
		}
		else
		{
			returnedError = AttributeError::InvalidAttributeID;
		}
		return retVal;
	}

	bool InputString::get_attribute(std::uint8_t attributeID, std::uint32_t &returnedAttributeData) const
	{
		bool retVal = false;

		if (attributeID < static_cast<std::uint8_t>(AttributeName::NumberOfAttributes))
		{
			switch (attributeID)
			{
				case static_cast<std::uint8_t>(AttributeName::Type):
				{
					returnedAttributeData = static_cast<std::uint8_t>(get_object_type());
					retVal = true;
				}
				break;

				case static_cast<std::uint8_t>(AttributeName::Width):
				{
					returnedAttributeData = get_width();
					retVal = true;
				}
				break;

				case static_cast<std::uint8_t>(AttributeName::Height):
				{
					returnedAttributeData = get_height();
					retVal = true;
				}
				break;

				case static_cast<std::uint8_t>(AttributeName::BackgroundColour):
				{
					returnedAttributeData = get_background_color();
					retVal = true;
				}
				break;

				case static_cast<std::uint8_t>(AttributeName::FontAttributes):
				{
					returnedAttributeData = get_font_attributes();
					retVal = true;
				}
				break;

				case static_cast<std::uint8_t>(AttributeName::InputAttributes):
				{
					returnedAttributeData = get_input_attributes();
					retVal = true;
				}
				break;

				case static_cast<std::uint8_t>(AttributeName::Options):
				{
					returnedAttributeData = optionsBitfield;
					retVal = true;
				}
				break;

				case static_cast<std::uint8_t>(AttributeName::VariableReference):
				{
					returnedAttributeData = get_variable_reference();
					retVal = true;
				}
				break;

				case static_cast<std::uint8_t>(AttributeName::Justification):
				{
					returnedAttributeData = justificationBitfield;
					retVal = true;
				}
				break;

				case static_cast<std::uint8_t>(AttributeName::Enabled):
				{
					returnedAttributeData = get_enabled();
					retVal = true;
				}
				break;

				default:
				{
					// Do nothing, return false
				}
				break;
			}
		}
		return retVal;
	}

	bool InputString::get_enabled() const
	{
		return enabled;
	}

	void InputString::set_enabled(bool value)
	{
		enabled = value;
	}

	bool InputString::get_option(Options option) const
	{
		return (0 != ((1 << static_cast<std::uint8_t>(option)) & optionsBitfield));
	}

	void InputString::set_option(Options option, bool value)
	{
		if (value)
		{
			optionsBitfield |= (1 << static_cast<std::uint8_t>(option));
		}
		else
		{
			optionsBitfield &= ~(1 << static_cast<std::uint8_t>(option));
		}
	}

	std::string InputString::get_value() const
	{
		return stringValue;
	}

	void InputString::set_value(const std::string &value)
	{
		stringValue = value;
	}

	std::uint16_t InputString::get_input_attributes() const
	{
		return inputAttributes;
	}

	void InputString::set_input_attributes(std::uint16_t inputAttributesValue)
	{
		inputAttributes = inputAttributesValue;
	}

	VirtualTerminalObjectType InputNumber::get_object_type() const
	{
		return VirtualTerminalObjectType::InputNumber;
	}

	std::uint32_t InputNumber::get_minumum_object_length() const
	{
		return MIN_OBJECT_LENGTH;
	}

	bool InputNumber::get_is_valid(const std::map<std::uint16_t, std::shared_ptr<VTObject>> &objectPool) const
	{
		bool anyWrongChildType = false;

		// Verify the variable reference is a number variable or NULL_OBJECT_ID
		if (NULL_OBJECT_ID != get_variable_reference())
		{
			auto variableReference = get_object_by_id(get_variable_reference(), objectPool);

			if (nullptr != variableReference)
			{
				if (VirtualTerminalObjectType::NumberVariable != variableReference->get_object_type())
				{
					anyWrongChildType = true;
				}
			}
			else
			{
				anyWrongChildType = true;
			}
		}

		// Verify that the font attributes is a font attribute or NULL_OBJECT_ID
		if (NULL_OBJECT_ID != get_font_attributes())
		{
			auto fontAttributes = get_object_by_id(get_font_attributes(), objectPool);

			if (nullptr != fontAttributes)
			{
				if (VirtualTerminalObjectType::FontAttributes != fontAttributes->get_object_type())
				{
					anyWrongChildType = true;
				}
			}
			else
			{
				anyWrongChildType = true;
			}
		}

		for (const auto &child : children)
		{
			auto childObject = get_object_by_id(child.id, objectPool);

			if ((nullptr == childObject) ||
			    (VirtualTerminalObjectType::Macro != childObject->get_object_type()))
			{
				anyWrongChildType = true;
				break;
			}
		}
		return (!anyWrongChildType);
	}

	bool InputNumber::set_attribute(std::uint8_t attributeID, std::uint32_t rawAttributeData, const std::map<std::uint16_t, std::shared_ptr<VTObject>> &objectPool, AttributeError &returnedError)
	{
		bool retVal = false;

		if (attributeID < static_cast<std::uint8_t>(AttributeName::NumberOfAttributes))
		{
			switch (static_cast<AttributeName>(attributeID))
			{
				case AttributeName::Width:
				{
					set_width(static_cast<std::uint16_t>(rawAttributeData));
					retVal = true;
				}
				break;

				case AttributeName::Height:
				{
					set_height(static_cast<std::uint16_t>(rawAttributeData));
					retVal = true;
				}
				break;

				case AttributeName::BackgroundColour:
				{
					set_background_color(static_cast<std::uint8_t>(rawAttributeData));
					retVal = true;
				}
				break;

				case AttributeName::FontAttributes:
				{
					auto fontAttributesObject = get_object_by_id(static_cast<std::uint16_t>(rawAttributeData), objectPool);

					if ((NULL_OBJECT_ID == rawAttributeData) ||
					    ((nullptr != fontAttributesObject) &&
					     (VirtualTerminalObjectType::FontAttributes == fontAttributesObject->get_object_type())))
					{
						set_font_attributes(static_cast<std::uint16_t>(rawAttributeData));
						retVal = true;
					}
					else
					{
						returnedError = AttributeError::InvalidValue;
					}
				}
				break;

				case AttributeName::Options:
				{
					set_options(static_cast<std::uint8_t>(rawAttributeData));
					retVal = true;
				}
				break;

				case AttributeName::VariableReference:
				{
					auto variableObject = get_object_by_id(static_cast<std::uint16_t>(rawAttributeData), objectPool);

					if ((NULL_OBJECT_ID == rawAttributeData) ||
					    ((nullptr != variableObject) &&
					     (VirtualTerminalObjectType::NumberVariable == variableObject->get_object_type())))
					{
						set_variable_reference(static_cast<std::uint16_t>(rawAttributeData));
						retVal = true;
					}
					else
					{
						returnedError = AttributeError::InvalidValue;
					}
				}
				break;

				case AttributeName::MinValue:
				{
					set_minimum_value(rawAttributeData);
					retVal = true;
				}
				break;

				case AttributeName::MaxValue:
				{
					set_maximum_value(rawAttributeData);
					retVal = true;
				}
				break;

				case AttributeName::Offset:
				{
					auto temp = reinterpret_cast<std::int32_t *>(&rawAttributeData);
					set_offset(*temp);
					retVal = true;
				}
				break;

				case AttributeName::Scale:
				{
					auto temp = reinterpret_cast<float *>(&rawAttributeData);
					set_scale(*temp);
					retVal = true;
				}
				break;

				case AttributeName::NumberOfDecimals:
				{
					set_number_of_decimals(static_cast<std::uint8_t>(rawAttributeData));
					retVal = true;
				}
				break;

				case AttributeName::Format:
				{
					set_format(0 != rawAttributeData);
					retVal = true;
				}
				break;

				case AttributeName::Justification:
				{
					set_justification_bitfield(static_cast<std::uint8_t>(rawAttributeData));
					retVal = true;
				}
				break;

				default:
				{
					returnedError = AttributeError::InvalidAttributeID;
				}
				break;
			}
		}
		else
		{
			returnedError = AttributeError::InvalidAttributeID;
		}
		return retVal;
	}

	bool InputNumber::get_attribute(std::uint8_t attributeID, std::uint32_t &returnedAttributeData) const
	{
		bool retVal = false;

		if (attributeID < static_cast<std::uint8_t>(AttributeName::NumberOfAttributes))
		{
			switch (attributeID)
			{
				case static_cast<std::uint8_t>(AttributeName::Type):
				{
					returnedAttributeData = static_cast<std::uint8_t>(get_object_type());
					retVal = true;
				}
				break;

				case static_cast<std::uint8_t>(AttributeName::Width):
				{
					returnedAttributeData = get_width();
					retVal = true;
				}
				break;

				case static_cast<std::uint8_t>(AttributeName::Height):
				{
					returnedAttributeData = get_height();
					retVal = true;
				}
				break;

				case static_cast<std::uint8_t>(AttributeName::BackgroundColour):
				{
					returnedAttributeData = get_background_color();
					retVal = true;
				}
				break;

				case static_cast<std::uint8_t>(AttributeName::FontAttributes):
				{
					returnedAttributeData = get_font_attributes();
					retVal = true;
				}
				break;

				case static_cast<std::uint8_t>(AttributeName::Options):
				{
					returnedAttributeData = optionsBitfield;
					retVal = true;
				}
				break;

				case static_cast<std::uint8_t>(AttributeName::VariableReference):
				{
					returnedAttributeData = get_variable_reference();
					retVal = true;
				}
				break;

				case static_cast<std::uint8_t>(AttributeName::MinValue):
				{
					returnedAttributeData = get_minimum_value();
					retVal = true;
				}
				break;

				case static_cast<std::uint8_t>(AttributeName::MaxValue):
				{
					returnedAttributeData = get_maximum_value();
					retVal = true;
				}
				break;

				case static_cast<std::uint8_t>(AttributeName::Offset):
				{
					auto temp = reinterpret_cast<const std::uint32_t *>(&offset);
					returnedAttributeData = *temp;
					retVal = true;
				}
				break;

				case static_cast<std::uint8_t>(AttributeName::Scale):
				{
					auto temp = reinterpret_cast<const std::uint32_t *>(&scale);
					returnedAttributeData = *temp;
					retVal = true;
				}
				break;

				case static_cast<std::uint8_t>(AttributeName::NumberOfDecimals):
				{
					returnedAttributeData = get_number_of_decimals();
					retVal = true;
				}
				break;

				case static_cast<std::uint8_t>(AttributeName::Format):
				{
					returnedAttributeData = get_format();
					retVal = true;
				}
				break;

				case static_cast<std::uint8_t>(AttributeName::Justification):
				{
					returnedAttributeData = justificationBitfield;
					retVal = true;
				}
				break;

				case static_cast<std::uint8_t>(AttributeName::Value):
				{
					returnedAttributeData = value;
					retVal = true;
				}
				break;

				case static_cast<std::uint8_t>(AttributeName::Options2):
				{
					returnedAttributeData = options2;
					retVal = true;
				}
				break;

				default:
				{
					// Do nothing, return false
				}
				break;
			}
		}
		return retVal;
	}

	std::uint32_t InputNumber::get_maximum_value() const
	{
		return maximumValue;
	}

	void InputNumber::set_maximum_value(std::uint32_t newMax)
	{
		maximumValue = newMax;
	}

	std::uint32_t InputNumber::get_minimum_value() const
	{
		return minimumValue;
	}

	void InputNumber::set_minimum_value(std::uint32_t newMin)
	{
		minimumValue = newMin;
	}

	bool NumberVTObject::get_option(Options newOption) const
	{
		return (0 != ((1 << static_cast<std::uint8_t>(newOption)) & optionsBitfield));
	}

	void NumberVTObject::set_option(Options option, bool optionValue)
	{
		if (optionValue)
		{
			optionsBitfield |= (1 << static_cast<std::uint8_t>(option));
		}
		else
		{
			optionsBitfield &= ~(1 << static_cast<std::uint8_t>(option));
		}
	}

	bool InputNumber::get_option2(Options2 newOption) const
	{
		return (0 != ((1 << static_cast<std::uint8_t>(newOption)) & options2));
	}

	void InputNumber::set_options2(std::uint8_t newOptions)
	{
		options2 = newOptions;
	}

	void InputNumber::set_option2(Options2 option, bool newOption)
	{
		if (newOption)
		{
			options2 |= (1 << static_cast<std::uint8_t>(option));
		}
		else
		{
			options2 &= ~(1 << static_cast<std::uint8_t>(option));
		}
	}

	std::uint32_t NumberVTObject::get_value() const
	{
		return value;
	}

	void NumberVTObject::set_value(std::uint32_t inputValue)
	{
		value = inputValue;
	}

	TextualVTObject::HorizontalJustification TextualVTObject::get_horizontal_justification() const
	{
		return static_cast<HorizontalJustification>(justificationBitfield & 0x03);
	}

	TextualVTObject::VerticalJustification TextualVTObject::get_vertical_justification() const
	{
		return static_cast<VerticalJustification>((justificationBitfield >> 2) & 0x03);
	}

	void TextualVTObject::set_justification_bitfield(std::uint8_t value)
	{
		justificationBitfield = value;
	}

	std::uint16_t TextualVTObject::get_font_attributes() const
	{
		return fontAttributes;
	}

	void TextualVTObject::set_font_attributes(std::uint16_t fontAttributesValue)
	{
		fontAttributes = fontAttributesValue;
	}

	VirtualTerminalObjectType InputList::get_object_type() const
	{
		return VirtualTerminalObjectType::InputList;
	}

	std::uint32_t InputList::get_minumum_object_length() const
	{
		return MIN_OBJECT_LENGTH;
	}

	bool InputList::get_is_valid(const std::map<std::uint16_t, std::shared_ptr<VTObject>> &objectPool) const
	{
		bool anyWrongChildType = false;

		for (const auto &child : children)
		{
			auto childObject = get_object_by_id(child.id, objectPool);
			if (nullptr != childObject)
			{
				switch (childObject->get_object_type())
				{
					case VirtualTerminalObjectType::WorkingSet:
					case VirtualTerminalObjectType::Container:
					case VirtualTerminalObjectType::OutputString:
					case VirtualTerminalObjectType::OutputNumber:
					case VirtualTerminalObjectType::OutputList:
					case VirtualTerminalObjectType::OutputLine:
					case VirtualTerminalObjectType::OutputRectangle:
					case VirtualTerminalObjectType::OutputEllipse:
					case VirtualTerminalObjectType::OutputPolygon:
					case VirtualTerminalObjectType::OutputMeter:
					case VirtualTerminalObjectType::OutputLinearBarGraph:
					case VirtualTerminalObjectType::OutputArchedBarGraph:
					case VirtualTerminalObjectType::GraphicsContext:
					case VirtualTerminalObjectType::PictureGraphic:
					case VirtualTerminalObjectType::ScaledGraphic:
					case VirtualTerminalObjectType::ObjectPointer:
					case VirtualTerminalObjectType::ExternalObjectPointer:
					{
						// Valid Child Object
					}
					break;

					default:
					{
						anyWrongChildType = true;
					}
					break;
				}
			}
		}
		return ((!anyWrongChildType) &&
		        (NULL_OBJECT_ID != objectID));
	}

	bool InputList::set_attribute(std::uint8_t attributeID, std::uint32_t rawAttributeData, const std::map<std::uint16_t, std::shared_ptr<VTObject>> &objectPool, AttributeError &returnedError)
	{
		bool retVal = false;

		if (attributeID < static_cast<std::uint8_t>(AttributeName::NumberOfAttributes))
		{
			switch (static_cast<AttributeName>(attributeID))
			{
				case AttributeName::Width:
				{
					set_width(static_cast<std::uint16_t>(rawAttributeData));
					retVal = true;
				}
				break;

				case AttributeName::Height:
				{
					set_height(static_cast<std::uint16_t>(rawAttributeData));
					retVal = true;
				}
				break;

				case AttributeName::VariableReference:
				{
					auto variableObject = get_object_by_id(static_cast<std::uint16_t>(rawAttributeData), objectPool);

					if ((NULL_OBJECT_ID == rawAttributeData) ||
					    ((nullptr != variableObject) &&
					     (VirtualTerminalObjectType::NumberVariable == variableObject->get_object_type())))
					{
						set_variable_reference(static_cast<std::uint16_t>(rawAttributeData));
						retVal = true;
					}
					else
					{
						returnedError = AttributeError::InvalidValue;
					}
				}
				break;

				case AttributeName::Value:
				{
					set_value(rawAttributeData);
					retVal = true;
				}
				break;

				case AttributeName::Options:
				{
					set_options(static_cast<std::uint8_t>(rawAttributeData));
					retVal = true;
				}
				break;

				default:
				{
					returnedError = AttributeError::InvalidAttributeID;
				}
				break;
			}
		}
		else
		{
			returnedError = AttributeError::InvalidAttributeID;
		}
		return retVal;
	}

	bool InputList::get_attribute(std::uint8_t attributeID, std::uint32_t &returnedAttributeData) const
	{
		bool retVal = false;

		if (attributeID < static_cast<std::uint8_t>(AttributeName::NumberOfAttributes))
		{
			switch (attributeID)
			{
				case static_cast<std::uint8_t>(AttributeName::Type):
				{
					returnedAttributeData = static_cast<std::uint8_t>(get_object_type());
					retVal = true;
				}
				break;

				case static_cast<std::uint8_t>(AttributeName::Width):
				{
					returnedAttributeData = get_width();
					retVal = true;
				}
				break;

				case static_cast<std::uint8_t>(AttributeName::Height):
				{
					returnedAttributeData = get_height();
					retVal = true;
				}
				break;

				case static_cast<std::uint8_t>(AttributeName::VariableReference):
				{
					returnedAttributeData = get_variable_reference();
					retVal = true;
				}
				break;

				case static_cast<std::uint8_t>(AttributeName::Value):
				{
					returnedAttributeData = get_value();
					retVal = true;
				}
				break;

				case static_cast<std::uint8_t>(AttributeName::Options):
				{
					returnedAttributeData = optionsBitfield;
					retVal = true;
				}
				break;

				default:
				{
					// Do nothing, return false
				}
				break;
			}
		}
		return retVal;
	}

	bool InputList::get_option(Options option) const
	{
		return (0 != (optionsBitfield & (1 << static_cast<std::uint8_t>(option))));
	}

	void InputList::set_options(std::uint8_t options)
	{
		optionsBitfield = options;
	}

	void InputList::set_option(Options option, bool optionValue)
	{
		if (optionValue)
		{
			optionsBitfield |= (1 << static_cast<std::uint8_t>(option));
		}
		else
		{
			optionsBitfield &= ~(1 << static_cast<std::uint8_t>(option));
		}
	}

	bool InputList::change_list_item(std::uint8_t index, std::uint16_t newListItem, const std::map<std::uint16_t, std::shared_ptr<VTObject>> &objectPool)
	{
		bool retVal = false;

		if ((index < children.size()) &&
		    ((NULL_OBJECT_ID == newListItem) ||
		     (nullptr != get_object_by_id(newListItem, objectPool))))
		{
			if (nullptr != get_object_by_id(newListItem, objectPool))
			{
				switch (get_object_by_id(newListItem, objectPool)->get_object_type())
				{
					case VirtualTerminalObjectType::WorkingSet:
					case VirtualTerminalObjectType::Container:
					case VirtualTerminalObjectType::OutputString:
					case VirtualTerminalObjectType::OutputNumber:
					case VirtualTerminalObjectType::OutputList:
					case VirtualTerminalObjectType::OutputLine:
					case VirtualTerminalObjectType::OutputRectangle:
					case VirtualTerminalObjectType::OutputEllipse:
					case VirtualTerminalObjectType::OutputPolygon:
					case VirtualTerminalObjectType::OutputMeter:
					case VirtualTerminalObjectType::OutputLinearBarGraph:
					case VirtualTerminalObjectType::OutputArchedBarGraph:
					case VirtualTerminalObjectType::GraphicsContext:
					case VirtualTerminalObjectType::PictureGraphic:
					case VirtualTerminalObjectType::ScaledGraphic:
					case VirtualTerminalObjectType::ObjectPointer:
					case VirtualTerminalObjectType::ExternalObjectPointer:
					{
						children.at(index).id = newListItem;
						retVal = true;
					}
					break;

					default:
					{
						// Invalid child object type
					}
					break;
				}
			}
			else
			{
				children.at(index).id = newListItem;
				retVal = true;
			}
		}
		return retVal;
	}

	VirtualTerminalObjectType OutputString::get_object_type() const
	{
		return VirtualTerminalObjectType::OutputString;
	}

	std::uint32_t OutputString::get_minumum_object_length() const
	{
		return MIN_OBJECT_LENGTH;
	}

	bool OutputString::get_is_valid(const std::map<std::uint16_t, std::shared_ptr<VTObject>> &objectPool) const
	{
		bool anyWrongChildType = false;

		// Verify the variable reference is a number variable or NULL_OBJECT_ID
		if (NULL_OBJECT_ID != get_variable_reference())
		{
			auto variableReference = get_object_by_id(get_variable_reference(), objectPool);

			if (nullptr != variableReference)
			{
				if (VirtualTerminalObjectType::StringVariable != variableReference->get_object_type())
				{
					anyWrongChildType = true;
				}
			}
			else
			{
				anyWrongChildType = true;
			}
		}

		// Verify that the font attributes is a font attribute or NULL_OBJECT_ID
		if (NULL_OBJECT_ID != get_font_attributes())
		{
			auto fontAttributes = get_object_by_id(get_font_attributes(), objectPool);

			if (nullptr != fontAttributes)
			{
				if (VirtualTerminalObjectType::FontAttributes != fontAttributes->get_object_type())
				{
					anyWrongChildType = true;
				}
			}
			else
			{
				anyWrongChildType = true;
			}
		}

		for (const auto &child : children)
		{
			auto childObject = get_object_by_id(child.id, objectPool);

			if ((nullptr == childObject) ||
			    (VirtualTerminalObjectType::Macro != childObject->get_object_type()))
			{
				anyWrongChildType = true;
				break;
			}
		}
		return (!anyWrongChildType);
	}

	bool OutputString::set_attribute(std::uint8_t attributeID, std::uint32_t rawAttributeData, const std::map<std::uint16_t, std::shared_ptr<VTObject>> &objectPool, AttributeError &returnedError)
	{
		bool retVal = false;

		if (attributeID < static_cast<std::uint8_t>(AttributeName::NumberOfAttributes))
		{
			switch (static_cast<AttributeName>(attributeID))
			{
				case AttributeName::Width:
				{
					set_width(static_cast<std::uint16_t>(rawAttributeData));
					retVal = true;
				}
				break;

				case AttributeName::Height:
				{
					set_height(static_cast<std::uint16_t>(rawAttributeData));
					retVal = true;
				}
				break;

				case AttributeName::BackgroundColour:
				{
					set_background_color(static_cast<std::uint8_t>(rawAttributeData));
					retVal = true;
				}
				break;

				case AttributeName::FontAttributes:
				{
					auto fontAttributesObject = get_object_by_id(static_cast<std::uint16_t>(rawAttributeData), objectPool);

					if ((NULL_OBJECT_ID == rawAttributeData) ||
					    ((nullptr != fontAttributesObject) &&
					     (VirtualTerminalObjectType::FontAttributes == fontAttributesObject->get_object_type())))
					{
						set_font_attributes(static_cast<std::uint16_t>(rawAttributeData));
						retVal = true;
					}
					else
					{
						returnedError = AttributeError::InvalidValue;
					}
				}
				break;

				case AttributeName::Options:
				{
					set_options(static_cast<std::uint8_t>(rawAttributeData));
					retVal = true;
				}
				break;

				case AttributeName::VariableReference:
				{
					auto variableObject = get_object_by_id(static_cast<std::uint16_t>(rawAttributeData), objectPool);

					if ((NULL_OBJECT_ID == rawAttributeData) ||
					    ((nullptr != variableObject) &&
					     (VirtualTerminalObjectType::StringVariable == variableObject->get_object_type())))
					{
						set_variable_reference(static_cast<std::uint16_t>(rawAttributeData));
						retVal = true;
					}
					else
					{
						returnedError = AttributeError::InvalidValue;
					}
				}
				break;

				case AttributeName::Justification:
				{
					set_justification_bitfield(static_cast<std::uint8_t>(rawAttributeData));
					retVal = true;
				}
				break;

				default:
				{
					returnedError = AttributeError::InvalidAttributeID;
				}
				break;
			}
		}
		else
		{
			returnedError = AttributeError::InvalidAttributeID;
		}
		return retVal;
	}

	bool OutputString::get_attribute(std::uint8_t attributeID, std::uint32_t &returnedAttributeData) const
	{
		bool retVal = false;

		if (attributeID < static_cast<std::uint8_t>(AttributeName::NumberOfAttributes))
		{
			switch (attributeID)
			{
				case static_cast<std::uint8_t>(AttributeName::Type):
				{
					returnedAttributeData = static_cast<std::uint8_t>(get_object_type());
					retVal = true;
				}
				break;

				case static_cast<std::uint8_t>(AttributeName::Width):
				{
					returnedAttributeData = get_width();
					retVal = true;
				}
				break;

				case static_cast<std::uint8_t>(AttributeName::Height):
				{
					returnedAttributeData = get_height();
					retVal = true;
				}
				break;

				case static_cast<std::uint8_t>(AttributeName::BackgroundColour):
				{
					returnedAttributeData = get_background_color();
					retVal = true;
				}
				break;

				case static_cast<std::uint8_t>(AttributeName::FontAttributes):
				{
					returnedAttributeData = get_font_attributes();
					retVal = true;
				}
				break;

				case static_cast<std::uint8_t>(AttributeName::Options):
				{
					returnedAttributeData = optionsBitfield;
					retVal = true;
				}
				break;

				case static_cast<std::uint8_t>(AttributeName::VariableReference):
				{
					returnedAttributeData = get_variable_reference();
					retVal = true;
				}
				break;

				case static_cast<std::uint8_t>(AttributeName::Justification):
				{
					returnedAttributeData = justificationBitfield;
					retVal = true;
				}
				break;

				default:
				{
					// Do nothing, return false
				}
				break;
			}
		}
		return retVal;
	}

	bool OutputString::get_option(Options option) const
	{
		return (0 != (optionsBitfield & (1 << static_cast<std::uint8_t>(option))));
	}

	void OutputString::set_option(Options option, bool value)
	{
		if (value)
		{
			optionsBitfield |= (1 << static_cast<std::uint8_t>(option));
		}
		else
		{
			optionsBitfield &= ~(1 << static_cast<std::uint8_t>(option));
		}
	}

	std::string OutputString::get_value() const
	{
		return stringValue;
	}

	std::string OutputString::displayed_value(std::shared_ptr<isobus::VirtualTerminalServerManagedWorkingSet> parentWorkingSet) const
	{
		if (isobus::NULL_OBJECT_ID != get_variable_reference())
		{
			auto child = get_object_by_id(get_variable_reference(), parentWorkingSet->get_object_tree());

			if ((nullptr != child) && (isobus::VirtualTerminalObjectType::StringVariable == child->get_object_type()))
			{
				return std::static_pointer_cast<isobus::StringVariable>(child)->get_value();
			}
		}
		return get_value();
	}

	void OutputString::set_value(const std::string &value)
	{
		stringValue = value;
	}

	VirtualTerminalObjectType OutputNumber::get_object_type() const
	{
		return VirtualTerminalObjectType::OutputNumber;
	}

	std::uint32_t OutputNumber::get_minumum_object_length() const
	{
		return MIN_OBJECT_LENGTH;
	}

	bool OutputNumber::get_is_valid(const std::map<std::uint16_t, std::shared_ptr<VTObject>> &objectPool) const
	{
		bool anyWrongChildType = false;

		// Verify the variable reference is a number variable or NULL_OBJECT_ID
		if (NULL_OBJECT_ID != get_variable_reference())
		{
			auto variableReference = get_object_by_id(get_variable_reference(), objectPool);

			if (nullptr != variableReference)
			{
				if (VirtualTerminalObjectType::NumberVariable != variableReference->get_object_type())
				{
					anyWrongChildType = true;
				}
			}
			else
			{
				anyWrongChildType = true;
			}
		}

		// Verify that the font attributes is a font attribute or NULL_OBJECT_ID
		if (NULL_OBJECT_ID != get_font_attributes())
		{
			auto fontAttributes = get_object_by_id(get_font_attributes(), objectPool);

			if (nullptr != fontAttributes)
			{
				if (VirtualTerminalObjectType::FontAttributes != fontAttributes->get_object_type())
				{
					anyWrongChildType = true;
				}
			}
			else
			{
				anyWrongChildType = true;
			}
		}

		for (const auto &child : children)
		{
			auto childObject = get_object_by_id(child.id, objectPool);

			if ((nullptr == childObject) ||
			    (VirtualTerminalObjectType::Macro != childObject->get_object_type()))
			{
				anyWrongChildType = true;
				break;
			}
		}
		return (!anyWrongChildType);
	}

	bool OutputNumber::set_attribute(std::uint8_t attributeID, std::uint32_t rawAttributeData, const std::map<std::uint16_t, std::shared_ptr<VTObject>> &objectPool, AttributeError &returnedError)
	{
		bool retVal = false;

		if (attributeID < static_cast<std::uint8_t>(AttributeName::NumberOfAttributes))
		{
			switch (static_cast<AttributeName>(attributeID))
			{
				case AttributeName::Width:
				{
					set_width(static_cast<std::uint16_t>(rawAttributeData));
					retVal = true;
				}
				break;

				case AttributeName::Height:
				{
					set_height(static_cast<std::uint16_t>(rawAttributeData));
					retVal = true;
				}
				break;

				case AttributeName::BackgroundColour:
				{
					set_background_color(static_cast<std::uint8_t>(rawAttributeData));
					retVal = true;
				}
				break;

				case AttributeName::FontAttributes:
				{
					auto fontAttributesObject = get_object_by_id(static_cast<std::uint16_t>(rawAttributeData), objectPool);

					if ((NULL_OBJECT_ID == rawAttributeData) ||
					    ((nullptr != fontAttributesObject) &&
					     (VirtualTerminalObjectType::FontAttributes == fontAttributesObject->get_object_type())))
					{
						set_font_attributes(static_cast<std::uint16_t>(rawAttributeData));
						retVal = true;
					}
					else
					{
						returnedError = AttributeError::InvalidValue;
					}
				}
				break;

				case AttributeName::Options:
				{
					set_options(static_cast<std::uint8_t>(rawAttributeData));
					retVal = true;
				}
				break;

				case AttributeName::VariableReference:
				{
					auto variableObject = get_object_by_id(static_cast<std::uint16_t>(rawAttributeData), objectPool);

					if ((NULL_OBJECT_ID == rawAttributeData) ||
					    ((nullptr != variableObject) &&
					     (VirtualTerminalObjectType::NumberVariable == variableObject->get_object_type())))
					{
						set_variable_reference(static_cast<std::uint16_t>(rawAttributeData));
						retVal = true;
					}
					else
					{
						returnedError = AttributeError::InvalidValue;
					}
				}
				break;

				case AttributeName::Offset:
				{
					auto temp = reinterpret_cast<std::int32_t *>(&rawAttributeData);
					set_offset(*temp);
					retVal = true;
				}
				break;

				case AttributeName::Scale:
				{
					auto temp = reinterpret_cast<float *>(&rawAttributeData);
					set_scale(*temp);
					retVal = true;
				}
				break;

				case AttributeName::NumberOfDecimals:
				{
					set_number_of_decimals(static_cast<std::uint8_t>(rawAttributeData));
					retVal = true;
				}
				break;

				case AttributeName::Format:
				{
					set_format(0 != rawAttributeData);
					retVal = true;
				}
				break;

				case AttributeName::Justification:
				{
					set_justification_bitfield(static_cast<std::uint8_t>(rawAttributeData));
					retVal = true;
				}
				break;

				default:
				{
					returnedError = AttributeError::InvalidAttributeID;
				}
				break;
			}
		}
		else
		{
			returnedError = AttributeError::InvalidAttributeID;
		}
		return retVal;
	}

	bool OutputNumber::get_attribute(std::uint8_t attributeID, std::uint32_t &returnedAttributeData) const
	{
		bool retVal = false;

		if (attributeID < static_cast<std::uint8_t>(AttributeName::NumberOfAttributes))
		{
			switch (attributeID)
			{
				case static_cast<std::uint8_t>(AttributeName::Type):
				{
					returnedAttributeData = static_cast<std::uint8_t>(get_object_type());
					retVal = true;
				}
				break;

				case static_cast<std::uint8_t>(AttributeName::Width):
				{
					returnedAttributeData = get_width();
					retVal = true;
				}
				break;

				case static_cast<std::uint8_t>(AttributeName::Height):
				{
					returnedAttributeData = get_height();
					retVal = true;
				}
				break;

				case static_cast<std::uint8_t>(AttributeName::BackgroundColour):
				{
					returnedAttributeData = get_background_color();
					retVal = true;
				}
				break;

				case static_cast<std::uint8_t>(AttributeName::FontAttributes):
				{
					returnedAttributeData = get_font_attributes();
					retVal = true;
				}
				break;

				case static_cast<std::uint8_t>(AttributeName::Options):
				{
					returnedAttributeData = optionsBitfield;
					retVal = true;
				}
				break;

				case static_cast<std::uint8_t>(AttributeName::VariableReference):
				{
					returnedAttributeData = get_variable_reference();
					retVal = true;
				}
				break;

				case static_cast<std::uint8_t>(AttributeName::Offset):
				{
					auto temp = reinterpret_cast<const std::uint32_t *>(&offset);
					returnedAttributeData = *temp;
					retVal = true;
				}
				break;

				case static_cast<std::uint8_t>(AttributeName::Scale):
				{
					auto temp = reinterpret_cast<const std::uint32_t *>(&scale);
					returnedAttributeData = *temp;
					retVal = true;
				}
				break;

				case static_cast<std::uint8_t>(AttributeName::NumberOfDecimals):
				{
					returnedAttributeData = get_number_of_decimals();
					retVal = true;
				}
				break;

				case static_cast<std::uint8_t>(AttributeName::Format):
				{
					returnedAttributeData = get_format();
					retVal = true;
				}
				break;

				case static_cast<std::uint8_t>(AttributeName::Justification):
				{
					returnedAttributeData = justificationBitfield;
					retVal = true;
				}
				break;

				default:
				{
					// Do nothing, return false
				}
				break;
			}
		}
		return retVal;
	}

	void TextualVTObject::set_options(std::uint8_t value)
	{
		optionsBitfield = value;
	}

	float NumberVTObject::get_scale() const
	{
		return scale;
	}

	void NumberVTObject::set_scale(float scaleValue)
	{
		scale = scaleValue;
	}

	std::int32_t NumberVTObject::get_offset() const
	{
		return offset;
	}

	void NumberVTObject::set_offset(std::int32_t offsetValue)
	{
		offset = offsetValue;
	}

	std::uint8_t NumberVTObject::get_number_of_decimals() const
	{
		return numberOfDecimals;
	}

	void NumberVTObject::set_number_of_decimals(std::uint8_t decimalValue)
	{
		numberOfDecimals = decimalValue;
	}

	bool NumberVTObject::get_format() const
	{
		return format;
	}

	void NumberVTObject::set_format(bool shouldFormatAsExponential)
	{
		format = shouldFormatAsExponential;
	}

	VirtualTerminalObjectType OutputList::get_object_type() const
	{
		return VirtualTerminalObjectType::OutputList;
	}

	std::uint32_t OutputList::get_minumum_object_length() const
	{
		return MIN_OBJECT_LENGTH;
	}

	bool OutputList::get_is_valid(const std::map<std::uint16_t, std::shared_ptr<VTObject>> &objectPool) const
	{
		bool anyWrongChildType = false;

		// Verify the variable reference is a number variable or NULL_OBJECT_ID
		if (NULL_OBJECT_ID != get_variable_reference())
		{
			auto variableReference = get_object_by_id(get_variable_reference(), objectPool);

			if (nullptr != variableReference)
			{
				if (VirtualTerminalObjectType::NumberVariable != variableReference->get_object_type())
				{
					anyWrongChildType = true;
				}
			}
			else
			{
				anyWrongChildType = true;
			}
		}

		for (const auto &child : children)
		{
			auto childObject = get_object_by_id(child.id, objectPool);
			if (nullptr != childObject)
			{
				switch (childObject->get_object_type())
				{
					case VirtualTerminalObjectType::OutputString:
					case VirtualTerminalObjectType::Macro:
					case VirtualTerminalObjectType::WorkingSet:
					case VirtualTerminalObjectType::Container:
					case VirtualTerminalObjectType::Button:
					case VirtualTerminalObjectType::KeyGroup:
					case VirtualTerminalObjectType::InputBoolean:
					case VirtualTerminalObjectType::InputString:
					case VirtualTerminalObjectType::InputNumber:
					case VirtualTerminalObjectType::InputList:
					case VirtualTerminalObjectType::OutputLine:
					case VirtualTerminalObjectType::OutputRectangle:
					case VirtualTerminalObjectType::OutputEllipse:
					case VirtualTerminalObjectType::OutputPolygon:
					case VirtualTerminalObjectType::OutputMeter:
					case VirtualTerminalObjectType::OutputLinearBarGraph:
					case VirtualTerminalObjectType::OutputArchedBarGraph:
					case VirtualTerminalObjectType::Animation:
					case VirtualTerminalObjectType::PictureGraphic:
					case VirtualTerminalObjectType::ScaledGraphic:
					case VirtualTerminalObjectType::ObjectPointer:
					case VirtualTerminalObjectType::ExternalObjectPointer:
					case VirtualTerminalObjectType::AuxiliaryFunctionType2:
					case VirtualTerminalObjectType::AuxiliaryInputType2:
					case VirtualTerminalObjectType::AuxiliaryControlDesignatorType2:
					{
						// Valid Child Object
					}
					break;

					default:
					{
						anyWrongChildType = true;
					}
					break;
				}
			}
		}
		return ((!anyWrongChildType) &&
		        (NULL_OBJECT_ID != objectID));
	}

	bool OutputList::set_attribute(std::uint8_t attributeID, std::uint32_t rawAttributeData, const std::map<std::uint16_t, std::shared_ptr<VTObject>> &objectPool, AttributeError &returnedError)
	{
		bool retVal = false;

		if (attributeID < static_cast<std::uint8_t>(AttributeName::NumberOfAttributes))
		{
			switch (static_cast<AttributeName>(attributeID))
			{
				case AttributeName::Width:
				{
					set_width(static_cast<std::uint16_t>(rawAttributeData));
					retVal = true;
				}
				break;

				case AttributeName::Height:
				{
					set_height(static_cast<std::uint16_t>(rawAttributeData));
					retVal = true;
				}
				break;

				case AttributeName::VariableReference:
				{
					auto variableObject = get_object_by_id(static_cast<std::uint16_t>(rawAttributeData), objectPool);

					if ((NULL_OBJECT_ID == rawAttributeData) ||
					    ((nullptr != variableObject) &&
					     (VirtualTerminalObjectType::NumberVariable == variableObject->get_object_type())))
					{
						set_variable_reference(static_cast<std::uint16_t>(rawAttributeData));
						retVal = true;
					}
					else
					{
						returnedError = AttributeError::InvalidValue;
					}
				}
				break;

				case AttributeName::Value:
				{
					set_value(static_cast<std::uint8_t>(rawAttributeData));
					retVal = true;
				}
				break;

				default:
				{
					returnedError = AttributeError::InvalidAttributeID;
				}
				break;
			}
		}
		else
		{
			returnedError = AttributeError::InvalidAttributeID;
		}
		return retVal;
	}

	bool OutputList::get_attribute(std::uint8_t attributeID, std::uint32_t &returnedAttributeData) const
	{
		bool retVal = false;

		if (attributeID < static_cast<std::uint8_t>(AttributeName::NumberOfAttributes))
		{
			switch (attributeID)
			{
				case static_cast<std::uint8_t>(AttributeName::Type):
				{
					returnedAttributeData = static_cast<std::uint8_t>(get_object_type());
					retVal = true;
				}
				break;

				case static_cast<std::uint8_t>(AttributeName::Width):
				{
					returnedAttributeData = get_width();
					retVal = true;
				}
				break;

				case static_cast<std::uint8_t>(AttributeName::Height):
				{
					returnedAttributeData = get_height();
					retVal = true;
				}
				break;

				case static_cast<std::uint8_t>(AttributeName::VariableReference):
				{
					returnedAttributeData = get_variable_reference();
					retVal = true;
				}
				break;

				case static_cast<std::uint8_t>(AttributeName::Value):
				{
					returnedAttributeData = get_value();
					retVal = true;
				}
				break;

				default:
				{
					// Do nothing, return false
				}
				break;
			}
		}
		return retVal;
	}

	std::uint8_t ListVTObject::get_number_of_list_items() const
	{
		return numberOfListItems;
	}

	void ListVTObject::set_number_of_list_items(std::uint8_t value)
	{
		numberOfListItems = value;
	}

	std::uint8_t ListVTObject::get_value() const
	{
		return value;
	}

	void ListVTObject::set_value(std::uint8_t aValue)
	{
		value = aValue;
	}

	bool OutputList::change_list_item(std::uint8_t index, std::uint16_t newListItem, const std::map<std::uint16_t, std::shared_ptr<VTObject>> &objectPool)
	{
		bool retVal = false;

		if ((index < children.size()) &&
		    ((NULL_OBJECT_ID == newListItem) ||
		     ((nullptr != get_object_by_id(newListItem, objectPool)) &&
		      (VirtualTerminalObjectType::NumberVariable == get_object_by_id(newListItem, objectPool)->get_object_type()))))
		{
			children.at(index).id = newListItem;
			retVal = true;
		}
		return retVal;
	}

	VirtualTerminalObjectType OutputLine::get_object_type() const
	{
		return VirtualTerminalObjectType::OutputLine;
	}

	bool OutputLine::get_is_valid(const std::map<std::uint16_t, std::shared_ptr<VTObject>> &objectPool) const
	{
		bool anyWrongChildType = false;

		// Verify the line attributes is a line attribute or NULL_OBJECT_ID
		if (NULL_OBJECT_ID != get_line_attributes())
		{
			auto lineAttributesObject = get_object_by_id(get_line_attributes(), objectPool);

			if (nullptr != lineAttributesObject)
			{
				if (VirtualTerminalObjectType::LineAttributes != lineAttributesObject->get_object_type())
				{
					anyWrongChildType = true;
				}
			}
			else
			{
				anyWrongChildType = true;
			}
		}

		for (const auto &child : children)
		{
			auto childObject = get_object_by_id(child.id, objectPool);

			if ((nullptr == childObject) ||
			    (VirtualTerminalObjectType::Macro != childObject->get_object_type()))
			{
				anyWrongChildType = true;
				break;
			}
		}
		return (!anyWrongChildType);
	}

	bool OutputLine::set_attribute(std::uint8_t attributeID, std::uint32_t rawAttributeData, const std::map<std::uint16_t, std::shared_ptr<VTObject>> &objectPool, AttributeError &returnedError)
	{
		bool retVal = false;

		if (attributeID < static_cast<std::uint8_t>(AttributeName::NumberOfAttributes))
		{
			switch (static_cast<AttributeName>(attributeID))
			{
				case AttributeName::LineAttributes:
				{
					auto lineAttributeObject = get_object_by_id(static_cast<std::uint16_t>(rawAttributeData), objectPool);

					if ((NULL_OBJECT_ID == rawAttributeData) ||
					    ((nullptr != lineAttributeObject) &&
					     (VirtualTerminalObjectType::LineAttributes == lineAttributeObject->get_object_type())))
					{
						set_line_attributes(static_cast<std::uint16_t>(rawAttributeData));
						retVal = true;
					}
					else
					{
						returnedError = AttributeError::InvalidValue;
					}
				}
				break;

				case AttributeName::Width:
				{
					set_width(static_cast<std::uint16_t>(rawAttributeData));
					retVal = true;
				}
				break;

				case AttributeName::Height:
				{
					set_height(static_cast<std::uint16_t>(rawAttributeData));
					retVal = true;
				}
				break;

				case AttributeName::LineDirection:
				{
					if (rawAttributeData <= static_cast<std::uint8_t>(LineDirection::BottomLeftToTopRight))
					{
						set_line_direction(static_cast<LineDirection>(rawAttributeData));
						retVal = true;
					}
					else
					{
						returnedError = AttributeError::AnyOtherError;
					}
				}
				break;

				default:
				{
					returnedError = AttributeError::InvalidAttributeID;
				}
				break;
			}
		}
		else
		{
			returnedError = AttributeError::InvalidAttributeID;
		}
		return retVal;
	}

	bool OutputLine::get_attribute(std::uint8_t attributeID, std::uint32_t &returnedAttributeData) const
	{
		bool retVal = false;

		if (attributeID < static_cast<std::uint8_t>(AttributeName::NumberOfAttributes))
		{
			switch (attributeID)
			{
				case static_cast<std::uint8_t>(AttributeName::Type):
				{
					returnedAttributeData = static_cast<std::uint8_t>(get_object_type());
					retVal = true;
				}
				break;

				case static_cast<std::uint8_t>(AttributeName::Width):
				{
					returnedAttributeData = get_width();
					retVal = true;
				}
				break;

				case static_cast<std::uint8_t>(AttributeName::Height):
				{
					returnedAttributeData = get_height();
					retVal = true;
				}
				break;

				case static_cast<std::uint8_t>(AttributeName::LineAttributes):
				{
					returnedAttributeData = get_line_attributes();
					retVal = true;
				}
				break;

				case static_cast<std::uint8_t>(AttributeName::LineDirection):
				{
					returnedAttributeData = lineDirection;
					retVal = true;
				}
				break;

				default:
				{
					// Do nothing, return false
				}
				break;
			}
		}
		return retVal;
	}

	std::uint32_t OutputLine::get_minumum_object_length() const
	{
		return MIN_OBJECT_LENGTH;
	}

	OutputLine::LineDirection OutputLine::get_line_direction() const
	{
		return static_cast<LineDirection>(lineDirection);
	}

	void OutputLine::set_line_direction(LineDirection value)
	{
		lineDirection = static_cast<std::uint8_t>(value);
	}

	std::uint16_t OutputLine::get_line_attributes() const
	{
		return lineAttributes;
	}

	void OutputLine::set_line_attributes(std::uint16_t lineAttributesObject)
	{
		lineAttributes = lineAttributesObject;
	}

	VirtualTerminalObjectType OutputRectangle::get_object_type() const
	{
		return VirtualTerminalObjectType::OutputRectangle;
	}

	std::uint32_t OutputRectangle::get_minumum_object_length() const
	{
		return MIN_OBJECT_LENGTH;
	}

	bool OutputRectangle::get_is_valid(const std::map<std::uint16_t, std::shared_ptr<VTObject>> &objectPool) const
	{
		bool anyWrongChildType = false;

		// Verify the line attributes is a line attribute or NULL_OBJECT_ID
		if (NULL_OBJECT_ID != get_line_attributes())
		{
			auto lineAttributesObject = get_object_by_id(get_line_attributes(), objectPool);

			if (nullptr != lineAttributesObject)
			{
				if (VirtualTerminalObjectType::LineAttributes != lineAttributesObject->get_object_type())
				{
					anyWrongChildType = true;
				}
			}
			else
			{
				anyWrongChildType = true;
			}
		}

		// Verify the fill attributes is a fill attribute or NULL_OBJECT_ID
		if (NULL_OBJECT_ID != get_fill_attributes())
		{
			auto fillAttributesObject = get_object_by_id(get_fill_attributes(), objectPool);

			if (nullptr != fillAttributesObject)
			{
				if (VirtualTerminalObjectType::FillAttributes != fillAttributesObject->get_object_type())
				{
					anyWrongChildType = true;
				}
			}
			else
			{
				anyWrongChildType = true;
			}
		}

		for (const auto &child : children)
		{
			auto childObject = get_object_by_id(child.id, objectPool);

			if ((nullptr == childObject) ||
			    (VirtualTerminalObjectType::Macro != childObject->get_object_type()))
			{
				anyWrongChildType = true;
				break;
			}
		}
		return (!anyWrongChildType);
	}

	bool OutputRectangle::set_attribute(std::uint8_t attributeID, std::uint32_t rawAttributeData, const std::map<std::uint16_t, std::shared_ptr<VTObject>> &objectPool, AttributeError &returnedError)
	{
		bool retVal = false;

		if (attributeID < static_cast<std::uint8_t>(AttributeName::NumberOfAttributes))
		{
			switch (static_cast<AttributeName>(attributeID))
			{
				case AttributeName::LineAttributes:
				{
					auto lineAttributeObject = get_object_by_id(static_cast<std::uint16_t>(rawAttributeData), objectPool);

					if ((NULL_OBJECT_ID == rawAttributeData) ||
					    ((nullptr != lineAttributeObject) &&
					     (VirtualTerminalObjectType::LineAttributes == lineAttributeObject->get_object_type())))
					{
						set_line_attributes(static_cast<std::uint16_t>(rawAttributeData));
						retVal = true;
					}
					else
					{
						returnedError = AttributeError::InvalidValue;
					}
				}
				break;

				case AttributeName::Width:
				{
					set_width(static_cast<std::uint16_t>(rawAttributeData));
					retVal = true;
				}
				break;

				case AttributeName::Height:
				{
					set_height(static_cast<std::uint16_t>(rawAttributeData));
					retVal = true;
				}
				break;

				case AttributeName::LineSuppression:
				{
					set_line_suppression_bitfield(static_cast<std::uint8_t>(rawAttributeData));
					retVal = true;
				}
				break;

				case AttributeName::FillAttributes:
				{
					auto fillAttributeObject = get_object_by_id(static_cast<std::uint16_t>(rawAttributeData), objectPool);

					if ((NULL_OBJECT_ID == rawAttributeData) ||
					    ((nullptr != fillAttributeObject) &&
					     (VirtualTerminalObjectType::FillAttributes == fillAttributeObject->get_object_type())))
					{
						set_fill_attributes(static_cast<std::uint16_t>(rawAttributeData));
						retVal = true;
					}
					else
					{
						returnedError = AttributeError::InvalidValue;
					}
				}
				break;

				default:
				{
					returnedError = AttributeError::InvalidAttributeID;
				}
				break;
			}
		}
		else
		{
			returnedError = AttributeError::InvalidAttributeID;
		}
		return retVal;
	}

	bool OutputRectangle::get_attribute(std::uint8_t attributeID, std::uint32_t &returnedAttributeData) const
	{
		bool retVal = false;

		if (attributeID < static_cast<std::uint8_t>(AttributeName::NumberOfAttributes))
		{
			switch (attributeID)
			{
				case static_cast<std::uint8_t>(AttributeName::Type):
				{
					returnedAttributeData = static_cast<std::uint8_t>(get_object_type());
					retVal = true;
				}
				break;

				case static_cast<std::uint8_t>(AttributeName::Width):
				{
					returnedAttributeData = get_width();
					retVal = true;
				}
				break;

				case static_cast<std::uint8_t>(AttributeName::Height):
				{
					returnedAttributeData = get_height();
					retVal = true;
				}
				break;

				case static_cast<std::uint8_t>(AttributeName::LineAttributes):
				{
					returnedAttributeData = get_line_attributes();
					retVal = true;
				}
				break;

				case static_cast<std::uint8_t>(AttributeName::LineSuppression):
				{
					returnedAttributeData = lineSuppressionBitfield;
					retVal = true;
				}
				break;

				case static_cast<std::uint8_t>(AttributeName::FillAttributes):
				{
					returnedAttributeData = get_fill_attributes();
					retVal = true;
				}
				break;

				default:
				{
					// Do nothing, return false
				}
				break;
			}
		}
		return retVal;
	}

	std::uint8_t OutputRectangle::get_line_suppression_bitfield() const
	{
		return lineSuppressionBitfield;
	}

	void OutputRectangle::set_line_suppression_bitfield(std::uint8_t value)
	{
		lineSuppressionBitfield = value;
	}

	std::uint16_t OutputRectangle::get_line_attributes() const
	{
		return lineAttributes;
	}

	void OutputRectangle::set_line_attributes(std::uint16_t lineAttributesObject)
	{
		lineAttributes = lineAttributesObject;
	}

	std::uint16_t OutputRectangle::get_fill_attributes() const
	{
		return fillAttributes;
	}

	void OutputRectangle::set_fill_attributes(std::uint16_t fillAttributesObject)
	{
		fillAttributes = fillAttributesObject;
	}

	VirtualTerminalObjectType OutputEllipse::get_object_type() const
	{
		return VirtualTerminalObjectType::OutputEllipse;
	}

	std::uint32_t OutputEllipse::get_minumum_object_length() const
	{
		return MIN_OBJECT_LENGTH;
	}

	bool OutputEllipse::get_is_valid(const std::map<std::uint16_t, std::shared_ptr<VTObject>> &objectPool) const
	{
		bool anyWrongChildType = false;

		// Verify the line attributes is a line attribute or NULL_OBJECT_ID
		if (NULL_OBJECT_ID != get_line_attributes())
		{
			auto lineAttributesObject = get_object_by_id(get_line_attributes(), objectPool);

			if (nullptr != lineAttributesObject)
			{
				if (VirtualTerminalObjectType::LineAttributes != lineAttributesObject->get_object_type())
				{
					anyWrongChildType = true;
				}
			}
			else
			{
				anyWrongChildType = true;
			}
		}

		// Verify the fill attributes is a fill attribute or NULL_OBJECT_ID
		if (NULL_OBJECT_ID != get_fill_attributes())
		{
			auto fillAttributesObject = get_object_by_id(get_fill_attributes(), objectPool);

			if (nullptr != fillAttributesObject)
			{
				if (VirtualTerminalObjectType::FillAttributes != fillAttributesObject->get_object_type())
				{
					anyWrongChildType = true;
				}
			}
			else
			{
				anyWrongChildType = true;
			}
		}

		for (const auto &child : children)
		{
			auto childObject = get_object_by_id(child.id, objectPool);

			if ((nullptr == childObject) ||
			    (VirtualTerminalObjectType::Macro != childObject->get_object_type()))
			{
				anyWrongChildType = true;
				break;
			}
		}
		return (!anyWrongChildType);
	}

	bool OutputEllipse::set_attribute(std::uint8_t attributeID, std::uint32_t rawAttributeData, const std::map<std::uint16_t, std::shared_ptr<VTObject>> &objectPool, AttributeError &returnedError)
	{
		bool retVal = false;

		if (attributeID < static_cast<std::uint8_t>(AttributeName::NumberOfAttributes))
		{
			switch (static_cast<AttributeName>(attributeID))
			{
				case AttributeName::LineAttributes:
				{
					auto lineAttributeObject = get_object_by_id(static_cast<std::uint16_t>(rawAttributeData), objectPool);

					if ((NULL_OBJECT_ID == rawAttributeData) ||
					    ((nullptr != lineAttributeObject) &&
					     (VirtualTerminalObjectType::LineAttributes == lineAttributeObject->get_object_type())))
					{
						set_line_attributes(static_cast<std::uint16_t>(rawAttributeData));
						retVal = true;
					}
					else
					{
						returnedError = AttributeError::InvalidValue;
					}
				}
				break;

				case AttributeName::Width:
				{
					set_width(static_cast<std::uint16_t>(rawAttributeData));
					retVal = true;
				}
				break;

				case AttributeName::Height:
				{
					set_height(static_cast<std::uint16_t>(rawAttributeData));
					retVal = true;
				}
				break;

				case AttributeName::EllipseType:
				{
					if (rawAttributeData <= static_cast<std::uint8_t>(EllipseType::ClosedEllipseSection))
					{
						set_ellipse_type(static_cast<EllipseType>(rawAttributeData));
						retVal = true;
					}
					else
					{
						returnedError = AttributeError::AnyOtherError;
					}
				}
				break;

				case AttributeName::StartAngle:
				{
					set_start_angle(static_cast<std::uint8_t>(rawAttributeData));
					retVal = true;
				}
				break;

				case AttributeName::EndAngle:
				{
					set_end_angle(static_cast<std::uint8_t>(rawAttributeData));
					retVal = true;
				}
				break;

				case AttributeName::FillAttributes:
				{
					auto fillAttributeObject = get_object_by_id(static_cast<std::uint16_t>(rawAttributeData), objectPool);

					if ((NULL_OBJECT_ID == rawAttributeData) ||
					    ((nullptr != fillAttributeObject) &&
					     (VirtualTerminalObjectType::FillAttributes == fillAttributeObject->get_object_type())))
					{
						set_fill_attributes(static_cast<std::uint16_t>(rawAttributeData));
						retVal = true;
					}
					else
					{
						returnedError = AttributeError::InvalidValue;
					}
				}
				break;

				default:
				{
					returnedError = AttributeError::InvalidAttributeID;
				}
				break;
			}
		}
		else
		{
			returnedError = AttributeError::InvalidAttributeID;
		}
		return retVal;
	}

	bool OutputEllipse::get_attribute(std::uint8_t attributeID, std::uint32_t &returnedAttributeData) const
	{
		bool retVal = false;

		if (attributeID < static_cast<std::uint8_t>(AttributeName::NumberOfAttributes))
		{
			switch (attributeID)
			{
				case static_cast<std::uint8_t>(AttributeName::Type):
				{
					returnedAttributeData = static_cast<std::uint8_t>(get_object_type());
					retVal = true;
				}
				break;

				case static_cast<std::uint8_t>(AttributeName::Width):
				{
					returnedAttributeData = get_width();
					retVal = true;
				}
				break;

				case static_cast<std::uint8_t>(AttributeName::Height):
				{
					returnedAttributeData = get_height();
					retVal = true;
				}
				break;

				case static_cast<std::uint8_t>(AttributeName::LineAttributes):
				{
					returnedAttributeData = get_line_attributes();
					retVal = true;
				}
				break;

				case static_cast<std::uint8_t>(AttributeName::EllipseType):
				{
					returnedAttributeData = ellipseType;
					retVal = true;
				}
				break;

				case static_cast<std::uint8_t>(AttributeName::StartAngle):
				{
					returnedAttributeData = startAngle;
					retVal = true;
				}
				break;

				case static_cast<std::uint8_t>(AttributeName::EndAngle):
				{
					returnedAttributeData = endAngle;
					retVal = true;
				}
				break;

				case static_cast<std::uint8_t>(AttributeName::FillAttributes):
				{
					returnedAttributeData = get_fill_attributes();
					retVal = true;
				}
				break;

				default:
				{
					// Do nothing, return false
				}
				break;
			}
		}
		return retVal;
	}

	OutputEllipse::EllipseType OutputEllipse::get_ellipse_type() const
	{
		return static_cast<EllipseType>(ellipseType);
	}

	void OutputEllipse::set_ellipse_type(EllipseType value)
	{
		ellipseType = static_cast<std::uint8_t>(value);
	}

	std::uint8_t OutputEllipse::get_start_angle() const
	{
		return startAngle;
	}

	void OutputEllipse::set_start_angle(std::uint8_t value)
	{
		startAngle = value;
	}

	std::uint8_t OutputEllipse::get_end_angle() const
	{
		return endAngle;
	}

	void OutputEllipse::set_end_angle(std::uint8_t value)
	{
		endAngle = value;
	}

	std::uint16_t OutputEllipse::get_line_attributes() const
	{
		return lineAttributes;
	}

	void OutputEllipse::set_line_attributes(std::uint16_t lineAttributesObject)
	{
		lineAttributes = lineAttributesObject;
	}

	std::uint16_t OutputEllipse::get_fill_attributes() const
	{
		return fillAttributes;
	}

	void OutputEllipse::set_fill_attributes(std::uint16_t fillAttributesObject)
	{
		fillAttributes = fillAttributesObject;
	}

	VirtualTerminalObjectType OutputPolygon::get_object_type() const
	{
		return VirtualTerminalObjectType::OutputPolygon;
	}

	std::uint32_t OutputPolygon::get_minumum_object_length() const
	{
		return MIN_OBJECT_LENGTH;
	}

	bool OutputPolygon::get_is_valid(const std::map<std::uint16_t, std::shared_ptr<VTObject>> &objectPool) const
	{
		bool anyWrongChildType = false;

		// Verify the line attributes is a line attribute or NULL_OBJECT_ID
		if (NULL_OBJECT_ID != get_line_attributes())
		{
			auto lineAttributesObject = get_object_by_id(get_line_attributes(), objectPool);

			if (nullptr != lineAttributesObject)
			{
				if (VirtualTerminalObjectType::LineAttributes != lineAttributesObject->get_object_type())
				{
					anyWrongChildType = true;
				}
			}
			else
			{
				anyWrongChildType = true;
			}
		}

		// Verify the fill attributes is a fill attribute or NULL_OBJECT_ID
		if (NULL_OBJECT_ID != get_fill_attributes())
		{
			auto fillAttributesObject = get_object_by_id(get_fill_attributes(), objectPool);

			if (nullptr != fillAttributesObject)
			{
				if (VirtualTerminalObjectType::FillAttributes != fillAttributesObject->get_object_type())
				{
					anyWrongChildType = true;
				}
			}
			else
			{
				anyWrongChildType = true;
			}
		}

		for (const auto &child : children)
		{
			auto childObject = get_object_by_id(child.id, objectPool);

			if ((nullptr == childObject) ||
			    (VirtualTerminalObjectType::Macro != childObject->get_object_type()))
			{
				anyWrongChildType = true;
				break;
			}
		}
		return (!anyWrongChildType);
	}

	bool OutputPolygon::set_attribute(std::uint8_t attributeID, std::uint32_t rawAttributeData, const std::map<std::uint16_t, std::shared_ptr<VTObject>> &objectPool, AttributeError &returnedError)
	{
		bool retVal = false;

		if (attributeID < static_cast<std::uint8_t>(AttributeName::NumberOfAttributes))
		{
			switch (static_cast<AttributeName>(attributeID))
			{
				case AttributeName::Width:
				{
					set_width(static_cast<std::uint16_t>(rawAttributeData));
					retVal = true;
				}
				break;

				case AttributeName::Height:
				{
					set_height(static_cast<std::uint16_t>(rawAttributeData));
					retVal = true;
				}
				break;

				case AttributeName::LineAttributes:
				{
					auto lineAttributeObject = get_object_by_id(static_cast<std::uint16_t>(rawAttributeData), objectPool);

					if ((NULL_OBJECT_ID == rawAttributeData) ||
					    ((nullptr != lineAttributeObject) &&
					     (VirtualTerminalObjectType::LineAttributes == lineAttributeObject->get_object_type())))
					{
						set_line_attributes(static_cast<std::uint16_t>(rawAttributeData));
						retVal = true;
					}
					else
					{
						returnedError = AttributeError::InvalidValue;
					}
				}
				break;

				case AttributeName::FillAttributes:
				{
					auto fillAttributeObject = get_object_by_id(static_cast<std::uint16_t>(rawAttributeData), objectPool);

					if ((NULL_OBJECT_ID == rawAttributeData) ||
					    ((nullptr != fillAttributeObject) &&
					     (VirtualTerminalObjectType::FillAttributes == fillAttributeObject->get_object_type())))
					{
						set_fill_attributes(static_cast<std::uint16_t>(rawAttributeData));
						retVal = true;
					}
					else
					{
						returnedError = AttributeError::InvalidValue;
					}
				}
				break;

				case AttributeName::PolygonType:
				{
					if (rawAttributeData <= static_cast<std::uint8_t>(PolygonType::Open))
					{
						set_type(static_cast<PolygonType>(rawAttributeData));
						retVal = true;
					}
					else
					{
						returnedError = AttributeError::AnyOtherError;
					}
				}
				break;

				default:
				{
					returnedError = AttributeError::InvalidAttributeID;
				}
				break;
			}
		}
		else
		{
			returnedError = AttributeError::InvalidAttributeID;
		}
		return retVal;
	}

	bool OutputPolygon::get_attribute(std::uint8_t attributeID, std::uint32_t &returnedAttributeData) const
	{
		bool retVal = false;

		if (attributeID < static_cast<std::uint8_t>(AttributeName::NumberOfAttributes))
		{
			switch (attributeID)
			{
				case static_cast<std::uint8_t>(AttributeName::Type):
				{
					returnedAttributeData = static_cast<std::uint8_t>(get_object_type());
					retVal = true;
				}
				break;

				case static_cast<std::uint8_t>(AttributeName::Width):
				{
					returnedAttributeData = get_width();
					retVal = true;
				}
				break;

				case static_cast<std::uint8_t>(AttributeName::Height):
				{
					returnedAttributeData = get_height();
					retVal = true;
				}
				break;

				case static_cast<std::uint8_t>(AttributeName::LineAttributes):
				{
					returnedAttributeData = get_line_attributes();
					retVal = true;
				}
				break;

				case static_cast<std::uint8_t>(AttributeName::FillAttributes):
				{
					returnedAttributeData = get_fill_attributes();
					retVal = true;
				}
				break;

				case static_cast<std::uint8_t>(AttributeName::PolygonType):
				{
					returnedAttributeData = polygonType;
					retVal = true;
				}
				break;

				default:
				{
					// Do nothing, return false
				}
				break;
			}
		}
		return retVal;
	}

	void OutputPolygon::add_point(std::uint16_t x, std::uint16_t y)
	{
		pointList.push_back({ x, y });
	}

	std::uint8_t OutputPolygon::get_number_of_points() const
	{
		return static_cast<std::uint8_t>(pointList.size());
	}

	OutputPolygon::PolygonPoint OutputPolygon::get_point(std::uint8_t index)
	{
		PolygonPoint retVal = { 0, 0 };

		if (index < pointList.size())
		{
			retVal = pointList[index];
		}
		return retVal;
	}

	bool OutputPolygon::change_point(std::uint8_t index, std::uint16_t x, std::uint16_t y)
	{
		bool retVal = false;

		if (index < pointList.size())
		{
			pointList.at(index).xValue = x;
			pointList.at(index).yValue = y;
			retVal = true;
		}
		return retVal;
	}

	OutputPolygon::PolygonType OutputPolygon::get_type() const
	{
		return static_cast<PolygonType>(polygonType);
	}

	void OutputPolygon::set_type(PolygonType value)
	{
		polygonType = static_cast<std::uint8_t>(value);
	}

	std::uint16_t OutputPolygon::get_line_attributes() const
	{
		return lineAttributes;
	}

	void OutputPolygon::set_line_attributes(std::uint16_t lineAttributesObject)
	{
		lineAttributes = lineAttributesObject;
	}

	std::uint16_t OutputPolygon::get_fill_attributes() const
	{
		return fillAttributes;
	}

	void OutputPolygon::set_fill_attributes(std::uint16_t fillAttributesObject)
	{
		fillAttributes = fillAttributesObject;
	}

	VirtualTerminalObjectType OutputMeter::get_object_type() const
	{
		return VirtualTerminalObjectType::OutputMeter;
	}

	std::uint32_t OutputMeter::get_minumum_object_length() const
	{
		return MIN_OBJECT_LENGTH;
	}

	bool OutputMeter::get_is_valid(const std::map<std::uint16_t, std::shared_ptr<VTObject>> &objectPool) const
	{
		bool anyWrongChildType = false;

		// Verify the variable reference is a number variable or NULL_OBJECT_ID
		if (NULL_OBJECT_ID != get_variable_reference())
		{
			auto variableObject = get_object_by_id(get_variable_reference(), objectPool);

			if (nullptr != variableObject)
			{
				if (VirtualTerminalObjectType::NumberVariable != variableObject->get_object_type())
				{
					anyWrongChildType = true;
				}
			}
			else
			{
				anyWrongChildType = true;
			}
		}

		for (const auto &child : children)
		{
			auto childObject = get_object_by_id(child.id, objectPool);

			if ((nullptr == childObject) ||
			    (VirtualTerminalObjectType::Macro != childObject->get_object_type()))
			{
				anyWrongChildType = true;
				break;
			}
		}
		return (!anyWrongChildType);
	}

	bool OutputMeter::set_attribute(std::uint8_t attributeID, std::uint32_t rawAttributeData, const std::map<std::uint16_t, std::shared_ptr<VTObject>> &objectPool, AttributeError &returnedError)
	{
		bool retVal = false;

		if (attributeID < static_cast<std::uint8_t>(AttributeName::NumberOfAttributes))
		{
			switch (static_cast<AttributeName>(attributeID))
			{
				case AttributeName::Width:
				{
					set_width(static_cast<std::uint16_t>(rawAttributeData));
					retVal = true;
				}
				break;

				case AttributeName::NeedleColour:
				{
					set_needle_colour(static_cast<std::uint8_t>(rawAttributeData));
					retVal = true;
				}
				break;

				case AttributeName::BorderColour:
				{
					set_border_colour(static_cast<std::uint8_t>(rawAttributeData));
					retVal = true;
				}
				break;

				case AttributeName::ArcAndTickColour:
				{
					set_arc_and_tick_colour(static_cast<std::uint8_t>(rawAttributeData));
					retVal = true;
				}
				break;

				case AttributeName::Options:
				{
					set_options(static_cast<std::uint8_t>(rawAttributeData));
					retVal = true;
				}
				break;

				case AttributeName::NumberOfTicks:
				{
					set_number_of_ticks(static_cast<std::uint8_t>(rawAttributeData));
					retVal = true;
				}
				break;

				case AttributeName::StartAngle:
				{
					set_start_angle(static_cast<std::uint8_t>(rawAttributeData));
					retVal = true;
				}
				break;

				case AttributeName::EndAngle:
				{
					set_end_angle(static_cast<std::uint8_t>(rawAttributeData));
					retVal = true;
				}
				break;

				case AttributeName::MinValue:
				{
					set_min_value(static_cast<std::uint16_t>(rawAttributeData));
					retVal = true;
				}
				break;

				case AttributeName::MaxValue:
				{
					set_max_value(static_cast<std::uint16_t>(rawAttributeData));
					retVal = true;
				}
				break;

				case AttributeName::VariableReference:
				{
					auto variableObject = get_object_by_id(static_cast<std::uint16_t>(rawAttributeData), objectPool);

					if ((NULL_OBJECT_ID == rawAttributeData) ||
					    ((nullptr != variableObject) &&
					     (VirtualTerminalObjectType::NumberVariable == variableObject->get_object_type())))
					{
						set_variable_reference(static_cast<std::uint16_t>(rawAttributeData));
						retVal = true;
					}
					else
					{
						returnedError = AttributeError::InvalidValue;
					}
				}
				break;

				case AttributeName::Value:
				{
					set_value(static_cast<std::uint16_t>(rawAttributeData));
					retVal = true;
				}
				break;

				default:
				{
					returnedError = AttributeError::InvalidAttributeID;
				}
				break;
			}
		}
		else
		{
			returnedError = AttributeError::InvalidAttributeID;
		}
		return retVal;
	}

	bool OutputMeter::get_attribute(std::uint8_t attributeID, std::uint32_t &returnedAttributeData) const
	{
		bool retVal = false;

		if (attributeID < static_cast<std::uint8_t>(AttributeName::NumberOfAttributes))
		{
			switch (attributeID)
			{
				case static_cast<std::uint8_t>(AttributeName::Type):
				{
					returnedAttributeData = static_cast<std::uint8_t>(get_object_type());
					retVal = true;
				}
				break;

				case static_cast<std::uint8_t>(AttributeName::Width):
				{
					returnedAttributeData = get_width();
					retVal = true;
				}
				break;

				case static_cast<std::uint8_t>(AttributeName::NeedleColour):
				{
					returnedAttributeData = get_needle_colour();
					retVal = true;
				}
				break;

				case static_cast<std::uint8_t>(AttributeName::BorderColour):
				{
					returnedAttributeData = get_border_colour();
					retVal = true;
				}
				break;

				case static_cast<std::uint8_t>(AttributeName::ArcAndTickColour):
				{
					returnedAttributeData = get_arc_and_tick_colour();
					retVal = true;
				}
				break;

				case static_cast<std::uint8_t>(AttributeName::Options):
				{
					returnedAttributeData = optionsBitfield;
					retVal = true;
				}
				break;

				case static_cast<std::uint8_t>(AttributeName::NumberOfTicks):
				{
					returnedAttributeData = get_number_of_ticks();
					retVal = true;
				}
				break;

				case static_cast<std::uint8_t>(AttributeName::StartAngle):
				{
					returnedAttributeData = get_start_angle();
					retVal = true;
				}
				break;

				case static_cast<std::uint8_t>(AttributeName::EndAngle):
				{
					returnedAttributeData = get_end_angle();
					retVal = true;
				}
				break;

				case static_cast<std::uint8_t>(AttributeName::MinValue):
				{
					returnedAttributeData = get_min_value();
					retVal = true;
				}
				break;

				case static_cast<std::uint8_t>(AttributeName::MaxValue):
				{
					returnedAttributeData = get_max_value();
					retVal = true;
				}
				break;

				case static_cast<std::uint8_t>(AttributeName::VariableReference):
				{
					returnedAttributeData = get_variable_reference();
					retVal = true;
				}
				break;

				case static_cast<std::uint8_t>(AttributeName::Value):
				{
					returnedAttributeData = get_value();
					retVal = true;
				}
				break;

				default:
				{
					// Do nothing, return false
				}
				break;
			}
		}
		return retVal;
	}

	std::uint16_t OutputMeter::get_min_value() const
	{
		return minValue;
	}

	void OutputMeter::set_min_value(std::uint16_t value)
	{
		minValue = value;
	}

	std::uint16_t OutputMeter::get_max_value() const
	{
		return maxValue;
	}

	void OutputMeter::set_max_value(std::uint16_t value)
	{
		maxValue = value;
	}

	std::uint16_t OutputMeter::get_value() const
	{
		return value;
	}

	void OutputMeter::set_value(std::uint16_t aValue)
	{
		value = aValue;
	}

	std::uint8_t OutputMeter::get_needle_colour() const
	{
		return needleColour;
	}

	void OutputMeter::set_needle_colour(std::uint8_t colourIndex)
	{
		needleColour = colourIndex;
	}

	std::uint8_t OutputMeter::get_border_colour() const
	{
		return borderColour;
	}

	void OutputMeter::set_border_colour(std::uint8_t colourIndex)
	{
		borderColour = colourIndex;
	}

	std::uint8_t OutputMeter::get_arc_and_tick_colour() const
	{
		return arcAndTickColour;
	}

	void OutputMeter::set_arc_and_tick_colour(std::uint8_t colourIndex)
	{
		arcAndTickColour = colourIndex;
	}

	std::uint8_t OutputMeter::get_number_of_ticks() const
	{
		return numberOfTicks;
	}

	void OutputMeter::set_number_of_ticks(std::uint8_t ticks)
	{
		numberOfTicks = ticks;
	}

	bool OutputMeter::get_option(Options option) const
	{
		return (0 != ((1 << static_cast<std::uint8_t>(option)) & optionsBitfield));
	}

	void OutputMeter::set_options(std::uint8_t options)
	{
		optionsBitfield = options;
	}

	void OutputMeter::set_option(Options option, bool optionValue)
	{
		if (optionValue)
		{
			optionsBitfield |= (1 << static_cast<std::uint8_t>(option));
		}
		else
		{
			optionsBitfield &= ~(1 << static_cast<std::uint8_t>(option));
		}
	}

	std::uint8_t OutputMeter::get_start_angle() const
	{
		return startAngle;
	}

	void OutputMeter::set_start_angle(std::uint8_t value)
	{
		startAngle = value;
	}

	std::uint8_t OutputMeter::get_end_angle() const
	{
		return endAngle;
	}

	void OutputMeter::set_end_angle(std::uint8_t value)
	{
		endAngle = value;
	}

	VirtualTerminalObjectType OutputLinearBarGraph::get_object_type() const
	{
		return VirtualTerminalObjectType::OutputLinearBarGraph;
	}

	std::uint32_t OutputLinearBarGraph::get_minumum_object_length() const
	{
		return MIN_OBJECT_LENGTH;
	}

	bool OutputLinearBarGraph::get_is_valid(const std::map<std::uint16_t, std::shared_ptr<VTObject>> &objectPool) const
	{
		bool anyWrongChildType = false;

		// Verify the variable reference is a number variable or NULL_OBJECT_ID
		if (NULL_OBJECT_ID != get_variable_reference())
		{
			auto variableObject = get_object_by_id(get_variable_reference(), objectPool);

			if (nullptr != variableObject)
			{
				if (VirtualTerminalObjectType::NumberVariable != variableObject->get_object_type())
				{
					anyWrongChildType = true;
				}
			}
			else
			{
				anyWrongChildType = true;
			}
		}

		// Verify the target value variable reference is a number variable or NULL_OBJECT_ID
		if (NULL_OBJECT_ID != get_target_value_reference())
		{
			auto variableObject = get_object_by_id(get_target_value_reference(), objectPool);

			if (nullptr != variableObject)
			{
				if (VirtualTerminalObjectType::NumberVariable != variableObject->get_object_type())
				{
					anyWrongChildType = true;
				}
			}
			else
			{
				anyWrongChildType = true;
			}
		}

		for (const auto &child : children)
		{
			auto childObject = get_object_by_id(child.id, objectPool);

			if ((nullptr == childObject) ||
			    (VirtualTerminalObjectType::Macro != childObject->get_object_type()))
			{
				anyWrongChildType = true;
				break;
			}
		}
		return (!anyWrongChildType);
	}

	bool OutputLinearBarGraph::set_attribute(std::uint8_t attributeID, std::uint32_t rawAttributeData, const std::map<std::uint16_t, std::shared_ptr<VTObject>> &objectPool, AttributeError &returnedError)
	{
		bool retVal = false;

		if (attributeID < static_cast<std::uint8_t>(AttributeName::NumberOfAttributes))
		{
			switch (static_cast<AttributeName>(attributeID))
			{
				case AttributeName::Width:
				{
					set_width(static_cast<std::uint16_t>(rawAttributeData));
					retVal = true;
				}
				break;

				case AttributeName::Height:
				{
					set_height(static_cast<std::uint16_t>(rawAttributeData));
					retVal = true;
				}
				break;

				case AttributeName::Colour:
				{
					set_colour(static_cast<std::uint8_t>(rawAttributeData));
					retVal = true;
				}
				break;

				case AttributeName::TargetLineColour:
				{
					set_target_line_colour(static_cast<std::uint8_t>(rawAttributeData));
					retVal = true;
				}
				break;

				case AttributeName::Options:
				{
					set_options(static_cast<std::uint8_t>(rawAttributeData));
					retVal = true;
				}
				break;

				case AttributeName::NumberOfTicks:
				{
					set_number_of_ticks(static_cast<std::uint8_t>(rawAttributeData));
					retVal = true;
				}
				break;

				case AttributeName::MinValue:
				{
					set_min_value(static_cast<std::uint16_t>(rawAttributeData));
					retVal = true;
				}
				break;

				case AttributeName::MaxValue:
				{
					set_max_value(static_cast<std::uint16_t>(rawAttributeData));
					retVal = true;
				}
				break;

				case AttributeName::VariableReference:
				{
					auto variableObject = get_object_by_id(static_cast<std::uint16_t>(rawAttributeData), objectPool);

					if ((NULL_OBJECT_ID == rawAttributeData) ||
					    ((nullptr != variableObject) &&
					     (VirtualTerminalObjectType::NumberVariable == variableObject->get_object_type())))
					{
						set_variable_reference(static_cast<std::uint16_t>(rawAttributeData));
						retVal = true;
					}
					else
					{
						returnedError = AttributeError::InvalidValue;
					}
				}
				break;

				case AttributeName::TargetValueVariableReference:
				{
					set_target_value_reference(static_cast<std::uint16_t>(rawAttributeData));
					retVal = true;
				}
				break;

				case AttributeName::TargetValue:
				{
					set_target_value(static_cast<std::uint16_t>(rawAttributeData));
					retVal = true;
				}
				break;

				case AttributeName::Value:
				{
					set_value(static_cast<std::uint16_t>(rawAttributeData));
					retVal = true;
				}
				break;

				default:
				{
					returnedError = AttributeError::InvalidAttributeID;
				}
				break;
			}
		}
		else
		{
			returnedError = AttributeError::InvalidAttributeID;
		}
		return retVal;
	}

	bool OutputLinearBarGraph::get_attribute(std::uint8_t attributeID, std::uint32_t &returnedAttributeData) const
	{
		bool retVal = false;

		if (attributeID < static_cast<std::uint8_t>(AttributeName::NumberOfAttributes))
		{
			switch (attributeID)
			{
				case static_cast<std::uint8_t>(AttributeName::Type):
				{
					returnedAttributeData = static_cast<std::uint8_t>(get_object_type());
					retVal = true;
				}
				break;

				case static_cast<std::uint8_t>(AttributeName::Width):
				{
					returnedAttributeData = get_width();
					retVal = true;
				}
				break;

				case static_cast<std::uint8_t>(AttributeName::Height):
				{
					returnedAttributeData = get_height();
					retVal = true;
				}
				break;

				case static_cast<std::uint8_t>(AttributeName::Colour):
				{
					returnedAttributeData = get_colour();
					retVal = true;
				}
				break;

				case static_cast<std::uint8_t>(AttributeName::TargetLineColour):
				{
					returnedAttributeData = get_target_line_colour();
					retVal = true;
				}
				break;

				case static_cast<std::uint8_t>(AttributeName::Options):
				{
					returnedAttributeData = optionsBitfield;
					retVal = true;
				}
				break;

				case static_cast<std::uint8_t>(AttributeName::NumberOfTicks):
				{
					returnedAttributeData = get_number_of_ticks();
					retVal = true;
				}
				break;

				case static_cast<std::uint8_t>(AttributeName::MinValue):
				{
					returnedAttributeData = get_min_value();
					retVal = true;
				}
				break;

				case static_cast<std::uint8_t>(AttributeName::MaxValue):
				{
					returnedAttributeData = get_max_value();
					retVal = true;
				}
				break;

				case static_cast<std::uint8_t>(AttributeName::VariableReference):
				{
					returnedAttributeData = get_variable_reference();
					retVal = true;
				}
				break;

				case static_cast<std::uint8_t>(AttributeName::TargetValueVariableReference):
				{
					returnedAttributeData = get_target_value_reference();
					retVal = true;
				}
				break;

				case static_cast<std::uint8_t>(AttributeName::TargetValue):
				{
					returnedAttributeData = get_target_value();
					retVal = true;
				}
				break;

				case static_cast<std::uint8_t>(AttributeName::Value):
				{
					returnedAttributeData = get_value();
					retVal = true;
				}
				break;

				default:
				{
					// Do nothing, return false
				}
				break;
			}
		}
		return retVal;
	}

	std::uint16_t OutputLinearBarGraph::get_min_value() const
	{
		return minValue;
	}

	void OutputLinearBarGraph::set_min_value(std::uint16_t value)
	{
		minValue = value;
	}

	std::uint16_t OutputLinearBarGraph::get_max_value() const
	{
		return maxValue;
	}

	void OutputLinearBarGraph::set_max_value(std::uint16_t value)
	{
		maxValue = value;
	}

	std::uint16_t OutputLinearBarGraph::get_value() const
	{
		return value;
	}

	void OutputLinearBarGraph::set_value(std::uint16_t aValue)
	{
		value = aValue;
	}

	std::uint16_t OutputLinearBarGraph::get_target_value() const
	{
		return targetValue;
	}

	void OutputLinearBarGraph::set_target_value(std::uint16_t valueTarget)
	{
		targetValue = valueTarget;
	}

	std::uint16_t OutputLinearBarGraph::get_target_value_reference() const
	{
		return targetValueReference;
	}

	void OutputLinearBarGraph::set_target_value_reference(std::uint16_t valueReferenceObjectID)
	{
		targetValueReference = valueReferenceObjectID;
	}

	std::uint8_t OutputLinearBarGraph::get_number_of_ticks() const
	{
		return numberOfTicks;
	}

	void OutputLinearBarGraph::set_number_of_ticks(std::uint8_t value)
	{
		numberOfTicks = value;
	}

	std::uint8_t OutputLinearBarGraph::get_colour() const
	{
		return colour;
	}

	void OutputLinearBarGraph::set_colour(std::uint8_t graphColour)
	{
		colour = graphColour;
	}

	std::uint8_t OutputLinearBarGraph::get_target_line_colour() const
	{
		return targetLineColour;
	}

	void OutputLinearBarGraph::set_target_line_colour(std::uint8_t lineColour)
	{
		targetLineColour = lineColour;
	}

	bool OutputLinearBarGraph::get_option(Options option) const
	{
		return (0 != ((1 << static_cast<std::uint8_t>(option)) & optionsBitfield));
	}

	void OutputLinearBarGraph::set_options(std::uint8_t options)
	{
		optionsBitfield = options;
	}

	void OutputLinearBarGraph::set_option(Options option, bool optionValue)
	{
		if (optionValue)
		{
			optionsBitfield |= (1 << static_cast<std::uint8_t>(option));
		}
		else
		{
			optionsBitfield &= ~(1 << static_cast<std::uint8_t>(option));
		}
	}

	VirtualTerminalObjectType OutputArchedBarGraph::get_object_type() const
	{
		return VirtualTerminalObjectType::OutputArchedBarGraph;
	}

	std::uint32_t OutputArchedBarGraph::get_minumum_object_length() const
	{
		return MIN_OBJECT_LENGTH;
	}

	bool OutputArchedBarGraph::get_is_valid(const std::map<std::uint16_t, std::shared_ptr<VTObject>> &objectPool) const
	{
		bool anyWrongChildType = false;

		// Verify the variable reference is a number variable or NULL_OBJECT_ID
		if (NULL_OBJECT_ID != get_variable_reference())
		{
			auto variableObject = get_object_by_id(get_variable_reference(), objectPool);

			if (nullptr != variableObject)
			{
				if (VirtualTerminalObjectType::NumberVariable != variableObject->get_object_type())
				{
					anyWrongChildType = true;
				}
			}
			else
			{
				anyWrongChildType = true;
			}
		}

		// Verify the target value variable reference is a number variable or NULL_OBJECT_ID
		if (NULL_OBJECT_ID != get_target_value_reference())
		{
			auto variableObject = get_object_by_id(get_target_value_reference(), objectPool);

			if (nullptr != variableObject)
			{
				if (VirtualTerminalObjectType::NumberVariable != variableObject->get_object_type())
				{
					anyWrongChildType = true;
				}
			}
			else
			{
				anyWrongChildType = true;
			}
		}

		for (const auto &child : children)
		{
			auto childObject = get_object_by_id(child.id, objectPool);

			if ((nullptr == childObject) ||
			    (VirtualTerminalObjectType::Macro != childObject->get_object_type()))
			{
				anyWrongChildType = true;
				break;
			}
		}
		return (!anyWrongChildType);
	}

	bool OutputArchedBarGraph::set_attribute(std::uint8_t attributeID, std::uint32_t rawAttributeData, const std::map<std::uint16_t, std::shared_ptr<VTObject>> &objectPool, AttributeError &returnedError)
	{
		bool retVal = false;

		if (attributeID < static_cast<std::uint8_t>(AttributeName::NumberOfAttributes))
		{
			switch (static_cast<AttributeName>(attributeID))
			{
				case AttributeName::Width:
				{
					set_width(static_cast<std::uint16_t>(rawAttributeData));
					retVal = true;
				}
				break;

				case AttributeName::Height:
				{
					set_height(static_cast<std::uint16_t>(rawAttributeData));
					retVal = true;
				}
				break;

				case AttributeName::Colour:
				{
					set_colour(static_cast<std::uint8_t>(rawAttributeData));
					retVal = true;
				}
				break;

				case AttributeName::TargetLineColour:
				{
					set_target_line_colour(static_cast<std::uint8_t>(rawAttributeData));
					retVal = true;
				}
				break;

				case AttributeName::Options:
				{
					set_options(static_cast<std::uint8_t>(rawAttributeData));
					retVal = true;
				}
				break;

				case AttributeName::StartAngle:
				{
					set_start_angle(static_cast<std::uint8_t>(rawAttributeData));
					retVal = true;
				}
				break;

				case AttributeName::EndAngle:
				{
					set_end_angle(static_cast<std::uint8_t>(rawAttributeData));
					retVal = true;
				}
				break;

				case AttributeName::BarGraphWidth:
				{
					set_bar_graph_width(static_cast<std::uint16_t>(rawAttributeData));
					retVal = true;
				}
				break;

				case AttributeName::MinValue:
				{
					set_min_value(static_cast<std::uint16_t>(rawAttributeData));
					retVal = true;
				}
				break;

				case AttributeName::MaxValue:
				{
					set_max_value(static_cast<std::uint16_t>(rawAttributeData));
					retVal = true;
				}
				break;

				case AttributeName::VariableReference:
				{
					auto variableObject = get_object_by_id(static_cast<std::uint16_t>(rawAttributeData), objectPool);

					if ((NULL_OBJECT_ID == rawAttributeData) ||
					    ((nullptr != variableObject) &&
					     (VirtualTerminalObjectType::NumberVariable == variableObject->get_object_type())))
					{
						set_variable_reference(static_cast<std::uint16_t>(rawAttributeData));
						retVal = true;
					}
					else
					{
						returnedError = AttributeError::InvalidValue;
					}
				}
				break;

				case AttributeName::TargetValueVariableReference:
				{
					set_target_value_reference(static_cast<std::uint16_t>(rawAttributeData));
					retVal = true;
				}
				break;

				case AttributeName::TargetValue:
				{
					set_target_value(static_cast<std::uint16_t>(rawAttributeData));
					retVal = true;
				}
				break;

				default:
				{
					returnedError = AttributeError::InvalidAttributeID;
				}
				break;
			}
		}
		else
		{
			returnedError = AttributeError::InvalidAttributeID;
		}
		return retVal;
	}

	bool OutputArchedBarGraph::get_attribute(std::uint8_t attributeID, std::uint32_t &returnedAttributeData) const
	{
		bool retVal = false;

		if (attributeID < static_cast<std::uint8_t>(AttributeName::NumberOfAttributes))
		{
			switch (attributeID)
			{
				case static_cast<std::uint8_t>(AttributeName::Type):
				{
					returnedAttributeData = static_cast<std::uint8_t>(get_object_type());
					retVal = true;
				}
				break;

				case static_cast<std::uint8_t>(AttributeName::Width):
				{
					returnedAttributeData = get_width();
					retVal = true;
				}
				break;

				case static_cast<std::uint8_t>(AttributeName::Height):
				{
					returnedAttributeData = get_height();
					retVal = true;
				}
				break;

				case static_cast<std::uint8_t>(AttributeName::Colour):
				{
					returnedAttributeData = get_colour();
					retVal = true;
				}
				break;

				case static_cast<std::uint8_t>(AttributeName::TargetLineColour):
				{
					returnedAttributeData = get_target_line_colour();
					retVal = true;
				}
				break;

				case static_cast<std::uint8_t>(AttributeName::Options):
				{
					returnedAttributeData = optionsBitfield;
					retVal = true;
				}
				break;

				case static_cast<std::uint8_t>(AttributeName::StartAngle):
				{
					returnedAttributeData = get_start_angle();
					retVal = true;
				}
				break;

				case static_cast<std::uint8_t>(AttributeName::EndAngle):
				{
					returnedAttributeData = get_end_angle();
					retVal = true;
				}
				break;

				case static_cast<std::uint8_t>(AttributeName::BarGraphWidth):
				{
					returnedAttributeData = get_bar_graph_width();
					retVal = true;
				}
				break;

				case static_cast<std::uint8_t>(AttributeName::MinValue):
				{
					returnedAttributeData = get_min_value();
					retVal = true;
				}
				break;

				case static_cast<std::uint8_t>(AttributeName::MaxValue):
				{
					returnedAttributeData = get_max_value();
					retVal = true;
				}
				break;

				case static_cast<std::uint8_t>(AttributeName::VariableReference):
				{
					returnedAttributeData = get_variable_reference();
					retVal = true;
				}
				break;

				case static_cast<std::uint8_t>(AttributeName::TargetValueVariableReference):
				{
					returnedAttributeData = get_target_value_reference();
					retVal = true;
				}
				break;

				case static_cast<std::uint8_t>(AttributeName::TargetValue):
				{
					returnedAttributeData = get_target_value();
					retVal = true;
				}
				break;

				default:
				{
					// Do nothing, return false
				}
				break;
			}
		}
		return retVal;
	}

	std::uint16_t OutputArchedBarGraph::get_bar_graph_width() const
	{
		return barGraphWidth;
	}

	void OutputArchedBarGraph::set_bar_graph_width(std::uint16_t width)
	{
		barGraphWidth = width;
	}

	std::uint16_t OutputArchedBarGraph::get_min_value() const
	{
		return minValue;
	}

	void OutputArchedBarGraph::set_min_value(std::uint16_t minimumValue)
	{
		minValue = minimumValue;
	}

	std::uint16_t OutputArchedBarGraph::get_max_value() const
	{
		return maxValue;
	}

	void OutputArchedBarGraph::set_max_value(std::uint16_t maximumValue)
	{
		maxValue = maximumValue;
	}

	std::uint16_t OutputArchedBarGraph::get_value() const
	{
		return value;
	}

	void OutputArchedBarGraph::set_value(std::uint16_t aValue)
	{
		value = aValue;
	}

	std::uint8_t OutputArchedBarGraph::get_target_line_colour() const
	{
		return targetLineColour;
	}

	void OutputArchedBarGraph::set_target_line_colour(std::uint8_t value)
	{
		targetLineColour = value;
	}

	std::uint8_t OutputArchedBarGraph::get_colour() const
	{
		return colour;
	}

	void OutputArchedBarGraph::set_colour(std::uint8_t value)
	{
		colour = value;
	}

	bool OutputArchedBarGraph::get_option(Options option) const
	{
		return (0 != ((1 << static_cast<std::uint8_t>(option)) & optionsBitfield));
	}

	void OutputArchedBarGraph::set_options(std::uint8_t options)
	{
		optionsBitfield = options;
	}

	void OutputArchedBarGraph::set_option(Options option, bool optionValue)
	{
		if (optionValue)
		{
			optionsBitfield |= (1 << static_cast<std::uint8_t>(option));
		}
		else
		{
			optionsBitfield &= ~(1 << static_cast<std::uint8_t>(option));
		}
	}

	std::uint8_t OutputArchedBarGraph::get_start_angle() const
	{
		return startAngle;
	}

	void OutputArchedBarGraph::set_start_angle(std::uint8_t value)
	{
		startAngle = value;
	}

	std::uint8_t OutputArchedBarGraph::get_end_angle() const
	{
		return endAngle;
	}

	void OutputArchedBarGraph::set_end_angle(std::uint8_t value)
	{
		endAngle = value;
	}

	std::uint16_t OutputArchedBarGraph::get_target_value() const
	{
		return targetValue;
	}

	void OutputArchedBarGraph::set_target_value(std::uint16_t value)
	{
		targetValue = value;
	}

	std::uint16_t OutputArchedBarGraph::get_target_value_reference() const
	{
		return targetValueReference;
	}

	void OutputArchedBarGraph::set_target_value_reference(std::uint16_t value)
	{
		targetValueReference = value;
	}

	VirtualTerminalObjectType PictureGraphic::get_object_type() const
	{
		return VirtualTerminalObjectType::PictureGraphic;
	}

	std::uint32_t PictureGraphic::get_minumum_object_length() const
	{
		return MIN_OBJECT_LENGTH;
	}

	bool PictureGraphic::get_is_valid(const std::map<std::uint16_t, std::shared_ptr<VTObject>> &) const
	{
		return true;
	}

	bool PictureGraphic::set_attribute(std::uint8_t attributeID, std::uint32_t rawAttributeData, const std::map<std::uint16_t, std::shared_ptr<VTObject>> &, AttributeError &returnedError)
	{
		bool retVal = false;

		if (attributeID < static_cast<std::uint8_t>(AttributeName::NumberOfAttributes))
		{
			switch (static_cast<AttributeName>(attributeID))
			{
				case AttributeName::Width:
				{
					set_width(static_cast<std::uint16_t>(rawAttributeData));
					retVal = true;
				}
				break;

				case AttributeName::Options:
				{
					set_options(static_cast<std::uint8_t>(rawAttributeData));
					retVal = true;
				}
				break;

				case AttributeName::TransparencyColour:
				{
					set_transparency_colour(static_cast<std::uint8_t>(rawAttributeData));
					retVal = true;
				}
				break;

				default:
				{
					returnedError = AttributeError::InvalidAttributeID;
				}
				break;
			}
		}
		else
		{
			returnedError = AttributeError::InvalidAttributeID;
		}
		return retVal;
	}

	bool PictureGraphic::get_attribute(std::uint8_t attributeID, std::uint32_t &returnedAttributeData) const
	{
		bool retVal = false;

		if (attributeID < static_cast<std::uint8_t>(AttributeName::NumberOfAttributes))
		{
			switch (attributeID)
			{
				case static_cast<std::uint8_t>(AttributeName::Type):
				{
					returnedAttributeData = static_cast<std::uint8_t>(get_object_type());
					retVal = true;
				}
				break;

				case static_cast<std::uint8_t>(AttributeName::Width):
				{
					returnedAttributeData = get_width();
					retVal = true;
				}
				break;

				case static_cast<std::uint8_t>(AttributeName::ActualWidth):
				{
					returnedAttributeData = get_actual_width();
					retVal = true;
				}
				break;

				case static_cast<std::uint8_t>(AttributeName::ActualHeight):
				{
					returnedAttributeData = get_actual_height();
					retVal = true;
				}
				break;

				case static_cast<std::uint8_t>(AttributeName::Options):
				{
					returnedAttributeData = optionsBitfield;
					retVal = true;
				}
				break;

				case static_cast<std::uint8_t>(AttributeName::TransparencyColour):
				{
					returnedAttributeData = get_transparency_colour();
					retVal = true;
				}
				break;

				case static_cast<std::uint8_t>(AttributeName::Format):
				{
					returnedAttributeData = static_cast<std::uint8_t>(get_format());
					retVal = true;
				}
				break;

				default:
				{
					// Do nothing, return false
				}
				break;
			}
		}
		return retVal;
	}

	std::vector<std::uint8_t> &PictureGraphic::get_raw_data()
	{
		return rawData;
	}

	void PictureGraphic::set_raw_data(const std::uint8_t *data, std::uint32_t size)
	{
		rawData.clear();
		rawData.resize(size);

		for (std::uint32_t i = 0; i < size; i++)
		{
			rawData[i] = data[i];
		}
	}

	void PictureGraphic::add_raw_data(std::uint8_t dataByte)
	{
		if (rawData.size() < (get_actual_width() * get_actual_height()))
		{
			rawData.push_back(dataByte);
		}
	}

	std::uint32_t PictureGraphic::get_number_of_bytes_in_raw_data() const
	{
		return numberOfBytesInRawData;
	}

	void PictureGraphic::set_number_of_bytes_in_raw_data(std::uint32_t value)
	{
		numberOfBytesInRawData = value;
		rawData.reserve(value);
	}

	std::uint16_t PictureGraphic::get_actual_width() const
	{
		return actualWidth;
	}

	void PictureGraphic::set_actual_width(std::uint16_t value)
	{
		actualWidth = value;
	}

	std::uint16_t PictureGraphic::get_actual_height() const
	{
		return actualHeight;
	}

	void PictureGraphic::set_actual_height(std::uint16_t value)
	{
		actualHeight = value;
	}

	PictureGraphic::Format PictureGraphic::get_format() const
	{
		return static_cast<Format>(formatByte);
	}

	void PictureGraphic::set_format(Format value)
	{
		formatByte = static_cast<std::uint8_t>(value);
	}

	bool PictureGraphic::get_option(Options option) const
	{
		return (0 != ((1 << static_cast<std::uint8_t>(option)) & optionsBitfield));
	}

	void PictureGraphic::set_options(std::uint8_t value)
	{
		optionsBitfield = value;
	}

	void PictureGraphic::set_option(Options option, bool value)
	{
		if (value)
		{
			optionsBitfield |= (1 << static_cast<std::uint8_t>(option));
		}
		else
		{
			optionsBitfield &= ~(1 << static_cast<std::uint8_t>(option));
		}
	}

	std::uint8_t PictureGraphic::get_transparency_colour() const
	{
		return transparencyColour;
	}

	void PictureGraphic::set_transparency_colour(std::uint8_t value)
	{
		transparencyColour = value;
	}

	VirtualTerminalObjectType NumberVariable::get_object_type() const
	{
		return VirtualTerminalObjectType::NumberVariable;
	}

	std::uint32_t NumberVariable::get_minumum_object_length() const
	{
		return MIN_OBJECT_LENGTH;
	}

	bool NumberVariable::get_is_valid(const std::map<std::uint16_t, std::shared_ptr<VTObject>> &) const
	{
		return true;
	}

	bool NumberVariable::set_attribute(std::uint8_t, std::uint32_t, const std::map<std::uint16_t, std::shared_ptr<VTObject>> &, AttributeError &returnedError)
	{
		returnedError = AttributeError::InvalidAttributeID;
		return false;
	}

	bool NumberVariable::get_attribute(std::uint8_t attributeID, std::uint32_t &returnedAttributeData) const
	{
		bool retVal = false;

		if (attributeID < static_cast<std::uint8_t>(AttributeName::NumberOfAttributes))
		{
			switch (attributeID)
			{
				case static_cast<std::uint8_t>(AttributeName::Type):
				{
					returnedAttributeData = static_cast<std::uint8_t>(get_object_type());
					retVal = true;
				}
				break;

				case static_cast<std::uint8_t>(AttributeName::Value):
				{
					returnedAttributeData = get_value();
					retVal = true;
				}
				break;

				default:
				{
					// Do nothing, return false
				}
				break;
			}
		}
		return retVal;
	}

	std::uint32_t NumberVariable::get_value() const
	{
		return value;
	}

	void NumberVariable::set_value(std::uint32_t aValue)
	{
		value = aValue;
	}

	VirtualTerminalObjectType StringVariable::get_object_type() const
	{
		return VirtualTerminalObjectType::StringVariable;
	}

	std::uint32_t StringVariable::get_minumum_object_length() const
	{
		return MIN_OBJECT_LENGTH;
	}

	bool StringVariable::get_is_valid(const std::map<std::uint16_t, std::shared_ptr<VTObject>> &) const
	{
		return true;
	}

	bool StringVariable::set_attribute(std::uint8_t, std::uint32_t, const std::map<std::uint16_t, std::shared_ptr<VTObject>> &, AttributeError &returnedError)
	{
		returnedError = AttributeError::InvalidAttributeID;
		return false;
	}

	bool StringVariable::get_attribute(std::uint8_t attributeID, std::uint32_t &returnedAttributeData) const
	{
		bool retVal = false;

		if (attributeID < static_cast<std::uint8_t>(AttributeName::NumberOfAttributes))
		{
			switch (attributeID)
			{
				case static_cast<std::uint8_t>(AttributeName::Type):
				{
					returnedAttributeData = static_cast<std::uint8_t>(get_object_type());
					retVal = true;
				}
				break;

				default:
				{
					// Do nothing, return false
				}
				break;
			}
		}
		return retVal;
	}

	std::string StringVariable::get_value() const
	{
		return value;
	}

	void StringVariable::set_value(const std::string &aValue)
	{
		value = aValue;
	}

	VirtualTerminalObjectType FontAttributes::get_object_type() const
	{
		return VirtualTerminalObjectType::FontAttributes;
	}

	std::uint32_t FontAttributes::get_minumum_object_length() const
	{
		return MIN_OBJECT_LENGTH;
	}

	bool FontAttributes::get_is_valid(const std::map<std::uint16_t, std::shared_ptr<VTObject>> &) const
	{
		return true;
	}

	bool FontAttributes::set_attribute(std::uint8_t attributeID, std::uint32_t rawAttributeData, const std::map<std::uint16_t, std::shared_ptr<VTObject>> &, AttributeError &returnedError)
	{
		bool retVal = false;

		if (attributeID < static_cast<std::uint8_t>(AttributeName::NumberOfAttributes))
		{
			switch (static_cast<AttributeName>(attributeID))
			{
				case AttributeName::FontColour:
				{
					set_colour(static_cast<std::uint8_t>(rawAttributeData));
					retVal = true;
				}
				break;

				case AttributeName::FontSize:
				{
					if (rawAttributeData <= static_cast<std::uint8_t>(FontAttributes::FontSize::Size128x192))
					{
						set_size(static_cast<FontAttributes::FontSize>(rawAttributeData));
						retVal = true;
					}
					else
					{
						returnedError = AttributeError::AnyOtherError;
					}
				}
				break;

				case AttributeName::FontType:
				{
					set_type(static_cast<FontAttributes::FontType>(rawAttributeData));
					retVal = true;
				}
				break;

				case AttributeName::FontStyle:
				{
					set_style(static_cast<std::uint8_t>(rawAttributeData));
					retVal = true;
				}
				break;

				default:
				{
					returnedError = AttributeError::InvalidAttributeID;
				}
				break;
			}
		}
		else
		{
			returnedError = AttributeError::InvalidAttributeID;
		}
		return retVal;
	}

	bool FontAttributes::get_attribute(std::uint8_t attributeID, std::uint32_t &returnedAttributeData) const
	{
		bool retVal = false;

		if (attributeID < static_cast<std::uint8_t>(AttributeName::NumberOfAttributes))
		{
			switch (attributeID)
			{
				case static_cast<std::uint8_t>(AttributeName::Type):
				{
					returnedAttributeData = static_cast<std::uint8_t>(get_object_type());
					retVal = true;
				}
				break;

				case static_cast<std::uint8_t>(AttributeName::FontColour):
				{
					returnedAttributeData = get_colour();
					retVal = true;
				}
				break;

				case static_cast<std::uint8_t>(AttributeName::FontSize):
				{
					returnedAttributeData = static_cast<std::uint8_t>(get_size());
					retVal = true;
				}
				break;

				case static_cast<std::uint8_t>(AttributeName::FontType):
				{
					returnedAttributeData = static_cast<std::uint8_t>(get_type());
					retVal = true;
				}
				break;

				case static_cast<std::uint8_t>(AttributeName::FontStyle):
				{
					returnedAttributeData = get_style();
					retVal = true;
				}
				break;

				default:
				{
					// Do nothing, return false
				}
				break;
			}
		}
		return retVal;
	}

	FontAttributes::FontType FontAttributes::get_type() const
	{
		return static_cast<FontType>(type);
	}

	void FontAttributes::set_type(FontType value)
	{
		type = static_cast<std::uint8_t>(value);
	}

	std::uint8_t FontAttributes::get_style() const
	{
		return style;
	}

	bool FontAttributes::get_style(FontStyleBits styleSetting) const
	{
		return (style >> static_cast<std::uint8_t>(styleSetting)) & 0x01;
	}

	void FontAttributes::set_style(FontStyleBits bit, bool value)
	{
		style = (static_cast<std::uint8_t>(value) << static_cast<std::uint8_t>(bit));
	}

	void FontAttributes::set_style(std::uint8_t value)
	{
		style = value;
	}

	FontAttributes::FontSize FontAttributes::get_size() const
	{
		return static_cast<FontSize>(size);
	}

	void FontAttributes::set_size(FontSize value)
	{
		size = static_cast<std::uint8_t>(value);
	}

	std::uint8_t FontAttributes::get_colour() const
	{
		return colour;
	}

	void FontAttributes::set_colour(std::uint8_t value)
	{
		colour = value;
	}

	std::uint8_t FontAttributes::get_font_width_pixels() const
	{
		std::uint8_t retVal = 0;

		switch (static_cast<FontSize>(size))
		{
			case FontSize::Size6x8:
			{
				retVal = 6;
			}
			break;

			case FontSize::Size8x8:
			case FontSize::Size8x12:
			{
				retVal = 8;
			}
			break;

			case FontSize::Size12x16:
			{
				retVal = 12;
			}
			break;

			case FontSize::Size16x16:
			case FontSize::Size16x24:
			{
				retVal = 16;
			}
			break;

			case FontSize::Size24x32:
			{
				retVal = 24;
			}
			break;

			case FontSize::Size32x32:
			case FontSize::Size32x48:
			{
				retVal = 32;
			}
			break;

			case FontSize::Size48x64:
			{
				retVal = 48;
			}
			break;

			case FontSize::Size64x64:
			case FontSize::Size64x96:
			{
				retVal = 64;
			}
			break;

			case FontSize::Size96x128:
			{
				retVal = 96;
			}
			break;

			case FontSize::Size128x128:
			case FontSize::Size128x192:
			{
				retVal = 128;
			}
			break;

			default:
				break;
		}
		return retVal;
	}

	std::uint8_t FontAttributes::get_font_height_pixels() const
	{
		std::uint8_t retVal = 0;

		switch (static_cast<FontSize>(size))
		{
			case FontSize::Size6x8:
			case FontSize::Size8x8:
			{
				retVal = 8;
			}
			break;

			case FontSize::Size8x12:
			{
				retVal = 12;
			}
			break;

			case FontSize::Size12x16:
			case FontSize::Size16x16:
			{
				retVal = 16;
			}
			break;

			case FontSize::Size16x24:
			{
				retVal = 24;
			}
			break;

			case FontSize::Size24x32:
			case FontSize::Size32x32:
			{
				retVal = 32;
			}
			break;

			case FontSize::Size32x48:
			{
				retVal = 48;
			}
			break;

			case FontSize::Size48x64:
			case FontSize::Size64x64:
			{
				retVal = 64;
			}
			break;

			case FontSize::Size64x96:
			{
				retVal = 96;
			}
			break;

			case FontSize::Size96x128:
			case FontSize::Size128x128:
			{
				retVal = 128;
			}
			break;

			case FontSize::Size128x192:
			{
				retVal = 192;
			}
			break;

			default:
				break;
		}
		return retVal;
	}

	VirtualTerminalObjectType LineAttributes::get_object_type() const
	{
		return VirtualTerminalObjectType::LineAttributes;
	}

	std::uint32_t LineAttributes::get_minumum_object_length() const
	{
		return MIN_OBJECT_LENGTH;
	}

	bool LineAttributes::get_is_valid(const std::map<std::uint16_t, std::shared_ptr<VTObject>> &) const
	{
		return true;
	}

	bool LineAttributes::set_attribute(std::uint8_t attributeID, std::uint32_t rawAttributeData, const std::map<std::uint16_t, std::shared_ptr<VTObject>> &, AttributeError &returnedError)
	{
		bool retVal = false;

		if (attributeID < static_cast<std::uint8_t>(AttributeName::NumberOfAttributes))
		{
			switch (static_cast<AttributeName>(attributeID))
			{
				case AttributeName::LineColour:
				{
					set_background_color(static_cast<std::uint8_t>(rawAttributeData));
					retVal = true;
				}
				break;

				case AttributeName::LineWidth:
				{
					set_width(static_cast<std::uint16_t>(rawAttributeData & 0xFF));
					retVal = true;
				}
				break;

				case AttributeName::LineArt:
				{
					set_line_art_bit_pattern(static_cast<std::uint16_t>(rawAttributeData));
					retVal = true;
				}
				break;

				default:
				{
					returnedError = AttributeError::InvalidAttributeID;
				}
				break;
			}
		}
		else
		{
			returnedError = AttributeError::InvalidAttributeID;
		}
		return retVal;
	}

	bool LineAttributes::get_attribute(std::uint8_t attributeID, std::uint32_t &returnedAttributeData) const
	{
		bool retVal = false;

		if (attributeID < static_cast<std::uint8_t>(AttributeName::NumberOfAttributes))
		{
			switch (attributeID)
			{
				case static_cast<std::uint8_t>(AttributeName::Type):
				{
					returnedAttributeData = static_cast<std::uint8_t>(get_object_type());
					retVal = true;
				}
				break;

				case static_cast<std::uint8_t>(AttributeName::LineColour):
				{
					returnedAttributeData = static_cast<std::uint32_t>(get_background_color());
					retVal = true;
				}
				break;

				case static_cast<std::uint8_t>(AttributeName::LineWidth):
				{
					returnedAttributeData = static_cast<std::uint32_t>(get_width());
					retVal = true;
				}
				break;

				case static_cast<std::uint8_t>(AttributeName::LineArt):
				{
					returnedAttributeData = static_cast<std::uint32_t>(get_line_art_bit_pattern());
					retVal = true;
				}
				break;

				default:
				{
					// Do nothing, return false
				}
				break;
			}
		}
		return retVal;
	}

	std::uint16_t LineAttributes::get_line_art_bit_pattern() const
	{
		return lineArtBitpattern;
	}

	void LineAttributes::set_line_art_bit_pattern(std::uint16_t value)
	{
		lineArtBitpattern = value;
	}

	VirtualTerminalObjectType FillAttributes::get_object_type() const
	{
		return VirtualTerminalObjectType::FillAttributes;
	}

	std::uint32_t FillAttributes::get_minumum_object_length() const
	{
		return MIN_OBJECT_LENGTH;
	}

	bool FillAttributes::get_is_valid(const std::map<std::uint16_t, std::shared_ptr<VTObject>> &objectPool) const
	{
		return ((NULL_OBJECT_ID == get_fill_pattern()) ||
		        ((nullptr != get_object_by_id(get_fill_pattern(), objectPool)) &&
		         (VirtualTerminalObjectType::PictureGraphic == get_object_by_id(get_fill_pattern(), objectPool)->get_object_type())));
	}

	bool FillAttributes::set_attribute(std::uint8_t attributeID, std::uint32_t rawAttributeData, const std::map<std::uint16_t, std::shared_ptr<VTObject>> &objectPool, AttributeError &returnedError)
	{
		bool retVal = false;

		if (attributeID < static_cast<std::uint8_t>(AttributeName::NumberOfAttributes))
		{
			switch (static_cast<AttributeName>(attributeID))
			{
				case AttributeName::FillType:
				{
					if (rawAttributeData <= static_cast<std::uint8_t>(FillType::FillWithPatternGivenByFillPatternAttribute))
					{
						set_type(static_cast<FillType>(rawAttributeData));
						retVal = true;
					}
					else
					{
						returnedError = AttributeError::AnyOtherError;
					}
				}
				break;

				case AttributeName::FillColour:
				{
					set_background_color(static_cast<std::uint8_t>(rawAttributeData));
					retVal = true;
				}
				break;

				case AttributeName::FillPattern:
				{
					auto fillPictureGraphic = get_object_by_id(static_cast<std::uint16_t>(rawAttributeData), objectPool);

					if (NULL_OBJECT_ID == rawAttributeData)
					{
						set_fill_pattern(static_cast<std::uint16_t>(rawAttributeData));
						retVal = true;
					}
					else if ((nullptr != fillPictureGraphic) &&
					         (VirtualTerminalObjectType::PictureGraphic == fillPictureGraphic->get_object_type()))
					{
						set_fill_pattern(static_cast<std::uint16_t>(rawAttributeData));
						retVal = true;
					}
					else
					{
						returnedError = AttributeError::InvalidValue;
					}
				}
				break;

				default:
				{
					returnedError = AttributeError::InvalidAttributeID;
				}
				break;
			}
		}
		else
		{
			returnedError = AttributeError::InvalidAttributeID;
		}
		return retVal;
	}

	bool FillAttributes::get_attribute(std::uint8_t attributeID, std::uint32_t &returnedAttributeData) const
	{
		bool retVal = false;

		if (attributeID < static_cast<std::uint8_t>(AttributeName::NumberOfAttributes))
		{
			switch (static_cast<AttributeName>(attributeID))
			{
				case AttributeName::Type:
				{
					returnedAttributeData = static_cast<std::uint32_t>(get_object_type());
					retVal = true;
				}
				break;

				case AttributeName::FillType:
				{
					returnedAttributeData = static_cast<std::uint32_t>(get_type());
					retVal = true;
				}
				break;

				case AttributeName::FillColour:
				{
					returnedAttributeData = static_cast<std::uint32_t>(get_background_color());
					retVal = true;
				}
				break;

				case AttributeName::FillPattern:
				{
					returnedAttributeData = static_cast<std::uint32_t>(get_fill_pattern());
					retVal = true;
				}
				break;

				default:
				{
					// Do nothing return false
				}
				break;
			}
		}
		return retVal;
	}

	std::uint16_t FillAttributes::get_fill_pattern() const
	{
		return fillPattern;
	}

	void FillAttributes::set_fill_pattern(std::uint16_t value)
	{
		fillPattern = value;
	}

	FillAttributes::FillType FillAttributes::get_type() const
	{
		return type;
	}

	void FillAttributes::set_type(FillType value)
	{
		type = value;
	}

	VirtualTerminalObjectType InputAttributes::get_object_type() const
	{
		return VirtualTerminalObjectType::InputAttributes;
	}

	std::uint32_t InputAttributes::get_minumum_object_length() const
	{
		return MIN_OBJECT_LENGTH;
	}

	bool InputAttributes::get_is_valid(const std::map<std::uint16_t, std::shared_ptr<VTObject>> &) const
	{
		return true;
	}

	bool InputAttributes::set_attribute(std::uint8_t, std::uint32_t, const std::map<std::uint16_t, std::shared_ptr<VTObject>> &, AttributeError &returnedError)
	{
		returnedError = AttributeError::InvalidAttributeID;
		return false;
	}

	bool InputAttributes::get_attribute(std::uint8_t attributeID, std::uint32_t &returnedAttributeData) const
	{
		bool retVal = false;

		if (attributeID < static_cast<std::uint8_t>(AttributeName::NumberOfAttributes))
		{
			switch (static_cast<AttributeName>(attributeID))
			{
				case AttributeName::Type:
				{
					returnedAttributeData = static_cast<std::uint32_t>(get_object_type());
					retVal = true;
				}
				break;

				case AttributeName::ValidationType:
				{
					returnedAttributeData = static_cast<std::uint32_t>(get_validation_type());
					retVal = true;
				}
				break;

				default:
				{
					// Do nothing return false
				}
				break;
			}
		}
		return retVal;
	}

	std::string InputAttributes::get_validation_string() const
	{
		return validationString;
	}

	void InputAttributes::set_validation_string(const std::string &value)
	{
		validationString = value;
	}

	InputAttributes::ValidationType InputAttributes::get_validation_type() const
	{
		return validationType;
	}

	void InputAttributes::set_validation_type(ValidationType newValidationType)
	{
		validationType = newValidationType;
	}

	VirtualTerminalObjectType ExtendedInputAttributes::get_object_type() const
	{
		return VirtualTerminalObjectType::ExtendedInputAttributes;
	}

	std::uint32_t ExtendedInputAttributes::get_minumum_object_length() const
	{
		return MIN_OBJECT_LENGTH;
	}

	bool ExtendedInputAttributes::get_is_valid(const std::map<std::uint16_t, std::shared_ptr<VTObject>> &) const
	{
		return true;
	}

	bool ExtendedInputAttributes::set_attribute(std::uint8_t, std::uint32_t, const std::map<std::uint16_t, std::shared_ptr<VTObject>> &, AttributeError &returnedError)
	{
		returnedError = AttributeError::InvalidAttributeID;
		return false;
	}

	bool ExtendedInputAttributes::get_attribute(std::uint8_t attributeID, std::uint32_t &returnedAttributeData) const
	{
		bool retVal = false;

		if (attributeID < static_cast<std::uint8_t>(AttributeName::NumberOfAttributes))
		{
			switch (static_cast<AttributeName>(attributeID))
			{
				case AttributeName::Type:
				{
					returnedAttributeData = static_cast<std::uint32_t>(get_object_type());
					retVal = true;
				}
				break;

				case AttributeName::ValidationType:
				{
					returnedAttributeData = static_cast<std::uint32_t>(get_validation_type());
					retVal = true;
				}
				break;

				default:
				{
					// Do nothing return false
				}
				break;
			}
		}
		return retVal;
	}

	std::uint8_t ExtendedInputAttributes::get_number_of_code_planes() const
	{
		return static_cast<std::uint8_t>(codePlanes.size());
	}

	void ExtendedInputAttributes::set_number_of_code_planes(std::uint8_t value)
	{
		codePlanes.resize(value);
	}

	ExtendedInputAttributes::ValidationType ExtendedInputAttributes::get_validation_type() const
	{
		return validationType;
	}

	void ExtendedInputAttributes::set_validation_type(ValidationType value)
	{
		validationType = value;
	}

	VirtualTerminalObjectType ObjectPointer::get_object_type() const
	{
		return VirtualTerminalObjectType::ObjectPointer;
	}

	std::uint32_t ObjectPointer::get_minumum_object_length() const
	{
		return MIN_OBJECT_LENGTH;
	}

	bool ObjectPointer::get_is_valid(const std::map<std::uint16_t, std::shared_ptr<VTObject>> &objectPool) const
	{
		return ((NULL_OBJECT_ID == value) || (nullptr != get_object_by_id(value, objectPool)));
	}

	bool ObjectPointer::set_attribute(std::uint8_t, std::uint32_t, const std::map<std::uint16_t, std::shared_ptr<VTObject>> &, AttributeError &returnedError)
	{
		returnedError = AttributeError::InvalidAttributeID;
		return false;
	}

	std::uint16_t ObjectPointer::get_value() const
	{
		return value;
	}

	void ObjectPointer::set_value(std::uint16_t objectIDToPointTo)
	{
		value = objectIDToPointTo;
	}

	bool ObjectPointer::get_attribute(std::uint8_t attributeID, std::uint32_t &returnedAttributeData) const
	{
		bool retVal = false;
		if (attributeID < static_cast<std::uint8_t>(AttributeName::NumberOfAttributes))
		{
			switch (static_cast<AttributeName>(attributeID))
			{
				case AttributeName::Type:
				{
					returnedAttributeData = static_cast<std::uint32_t>(get_object_type());
					retVal = true;
				}
				break;

				case AttributeName::Value:
				{
					if (get_number_children() > 0)
					{
						returnedAttributeData = get_child_id(0);
					}
					else
					{
						returnedAttributeData = NULL_OBJECT_ID;
					}
					retVal = true;
				}
				break;

				default:
				{
					// Do nothing return false
				}
				break;
			}
		}
		return retVal;
	}

	VirtualTerminalObjectType ExternalObjectPointer::get_object_type() const
	{
		return VirtualTerminalObjectType::ExternalObjectPointer;
	}

	std::uint32_t ExternalObjectPointer::get_minumum_object_length() const
	{
		return 9;
	}

	bool ExternalObjectPointer::get_is_valid(const std::map<std::uint16_t, std::shared_ptr<VTObject>> &objectPool) const
	{
		bool isDefaultObjectValid = (NULL_OBJECT_ID == get_default_object_id()) ||
		  (nullptr != objectPool.at(get_default_object_id()));
		bool isExternalNAMEIDValid = (NULL_OBJECT_ID == get_external_reference_name_id()) ||
		  (nullptr != objectPool.at(get_external_reference_name_id()));
		return (isDefaultObjectValid && isExternalNAMEIDValid);
	}

	bool ExternalObjectPointer::set_attribute(std::uint8_t attributeID, std::uint32_t rawAttributeData, const std::map<std::uint16_t, std::shared_ptr<VTObject>> &objectPool, AttributeError &returnedError)
	{
		bool retVal = false;

		if (attributeID < static_cast<std::uint8_t>(AttributeName::NumberOfAttributes))
		{
			switch (static_cast<AttributeName>(attributeID))
			{
				case AttributeName::DefaultObjectID:
				{
					if ((NULL_OBJECT_ID == rawAttributeData) ||
					    (nullptr != get_object_by_id(static_cast<std::uint16_t>(rawAttributeData), objectPool)))
					{
						set_default_object_id(static_cast<std::uint16_t>(rawAttributeData));
						retVal = true;
					}
					else
					{
						returnedError = AttributeError::InvalidValue;
					}
				}
				break;

				case AttributeName::ExternalReferenceNAMEID:
				{
					if ((NULL_OBJECT_ID == rawAttributeData) ||
					    (nullptr != get_object_by_id(static_cast<std::uint16_t>(rawAttributeData), objectPool)))
					{
						set_external_reference_name_id(static_cast<std::uint16_t>(rawAttributeData));
						retVal = true;
					}
					else
					{
						returnedError = AttributeError::InvalidValue;
					}
				}
				break;

				case AttributeName::ExternalObjectID:
				{
					set_external_object_id(static_cast<std::uint16_t>(rawAttributeData));
					retVal = true;
				}
				break;

				default:
				{
					returnedError = AttributeError::InvalidAttributeID;
				}
				break;
			}
		}
		else
		{
			returnedError = AttributeError::InvalidAttributeID;
		}
		return retVal;
	}

	bool ExternalObjectPointer::get_attribute(std::uint8_t attributeID, std::uint32_t &returnedAttributeData) const
	{
		bool retVal = false;
		if (attributeID < static_cast<std::uint8_t>(AttributeName::NumberOfAttributes))
		{
			switch (static_cast<AttributeName>(attributeID))
			{
				case AttributeName::Type:
				{
					returnedAttributeData = static_cast<std::uint32_t>(get_object_type());
					retVal = true;
				}
				break;

				case AttributeName::DefaultObjectID:
				{
					returnedAttributeData = static_cast<std::uint32_t>(get_default_object_id());
					retVal = true;
				}
				break;

				case AttributeName::ExternalReferenceNAMEID:
				{
					returnedAttributeData = static_cast<std::uint32_t>(get_external_reference_name_id());
					retVal = true;
				}
				break;

				case AttributeName::ExternalObjectID:
				{
					returnedAttributeData = static_cast<std::uint32_t>(get_external_object_id());
					retVal = true;
				}
				break;

				default:
				{
					// Do nothing return false
				}
				break;
			}
		}
		return retVal;
	}

	std::uint16_t ExternalObjectPointer::get_default_object_id() const
	{
		return defaultObjectID;
	}

	void ExternalObjectPointer::set_default_object_id(std::uint16_t id)
	{
		defaultObjectID = id;
	}

	std::uint16_t ExternalObjectPointer::get_external_reference_name_id() const
	{
		return externalReferenceNAMEID;
	}

	void ExternalObjectPointer::set_external_reference_name_id(std::uint16_t id)
	{
		externalReferenceNAMEID = id;
	}

	std::uint16_t ExternalObjectPointer::get_external_object_id() const
	{
		return externalObjectID;
	}

	void ExternalObjectPointer::set_external_object_id(std::uint16_t id)
	{
		externalObjectID = id;
	}

	const std::array<std::uint8_t, 28> Macro::ALLOWED_COMMANDS_LOOKUP_TABLE = {
		static_cast<std::uint8_t>(Command::HideShowObject),
		static_cast<std::uint8_t>(Command::EnableDisableObject),
		static_cast<std::uint8_t>(Command::SelectInputObject),
		static_cast<std::uint8_t>(Command::ControlAudioSignal),
		static_cast<std::uint8_t>(Command::SetAudioVolume),
		static_cast<std::uint8_t>(Command::ChangeChildLocation),
		static_cast<std::uint8_t>(Command::ChangeSize),
		static_cast<std::uint8_t>(Command::ChangeBackgroundColour),
		static_cast<std::uint8_t>(Command::ChangeNumericValue),
		static_cast<std::uint8_t>(Command::ChangeEndPoint),
		static_cast<std::uint8_t>(Command::ChangeFontAttributes),
		static_cast<std::uint8_t>(Command::ChangeLineAttributes),
		static_cast<std::uint8_t>(Command::ChangeFillAttributes),
		static_cast<std::uint8_t>(Command::ChangeActiveMask),
		static_cast<std::uint8_t>(Command::ChangeSoftKeyMask),
		static_cast<std::uint8_t>(Command::ChangeAttribute),
		static_cast<std::uint8_t>(Command::ChangePriority),
		static_cast<std::uint8_t>(Command::ChangeListItem),
		static_cast<std::uint8_t>(Command::ChangeStringValue),
		static_cast<std::uint8_t>(Command::ChangeChildPosition),
		static_cast<std::uint8_t>(Command::ChangeObjectLabel),
		static_cast<std::uint8_t>(Command::ChangePolygonPoint),
		static_cast<std::uint8_t>(Command::LockUnlockMask),
		static_cast<std::uint8_t>(Command::ExecuteMacro),
		static_cast<std::uint8_t>(Command::ChangePolygonScale),
		static_cast<std::uint8_t>(Command::GraphicsContextCommand),
		static_cast<std::uint8_t>(Command::SelectColourMap),
		static_cast<std::uint8_t>(Command::ExecuteExtendedMacro)
	};

	VirtualTerminalObjectType Macro::get_object_type() const
	{
		return VirtualTerminalObjectType::Macro;
	}

	std::uint32_t Macro::get_minumum_object_length() const
	{
		return MIN_OBJECT_LENGTH;
	}

	bool Macro::get_is_valid(const std::map<std::uint16_t, std::shared_ptr<VTObject>> &) const
	{
		return get_are_command_packets_valid();
	}

	bool Macro::set_attribute(std::uint8_t, std::uint32_t, const std::map<std::uint16_t, std::shared_ptr<VTObject>> &, AttributeError &returnedError)
	{
		returnedError = AttributeError::InvalidAttributeID;
		return false;
	}

	bool Macro::get_attribute(std::uint8_t attributeID, std::uint32_t &returnedAttributeData) const
	{
		bool retVal = false;
		if (attributeID < static_cast<std::uint8_t>(AttributeName::NumberOfAttributes))
		{
			switch (static_cast<AttributeName>(attributeID))
			{
				case AttributeName::Type:
				{
					returnedAttributeData = static_cast<std::uint32_t>(get_object_type());
					retVal = true;
				}
				break;

				default:
				{
					// Do nothing return false
				}
				break;
			}
		}
		return retVal;
	}

	bool Macro::add_command_packet(const std::vector<std::uint8_t> &command)
	{
		bool retVal = false;

		if (commandPackets.size() < 255)
		{
			commandPackets.push_back(command);
			retVal = true;
		}
		return retVal;
	}

	std::uint8_t Macro::get_number_of_commands() const
	{
		return static_cast<std::uint8_t>(commandPackets.size());
	}

	bool Macro::get_command_packet(std::uint8_t index, std::vector<std::uint8_t> &command)
	{
		bool retVal = false;

		if (index < commandPackets.size())
		{
			command = commandPackets.at(index);
			retVal = true;
		}
		return retVal;
	}

	bool Macro::remove_command_packet(std::uint8_t index)
	{
		bool retVal = false;

		if (index < commandPackets.size())
		{
			auto eraseLocation = commandPackets.begin() + index;
			commandPackets.erase(eraseLocation);
			retVal = true;
		}
		return retVal;
	}

	bool Macro::get_are_command_packets_valid() const
	{
		bool retVal = true;

		if (commandPackets.empty())
		{
			retVal = true;
		}
		else
		{
			for (const auto &command : commandPackets)
			{
				if (command.empty() ||
				    std::find(ALLOWED_COMMANDS_LOOKUP_TABLE.begin(), ALLOWED_COMMANDS_LOOKUP_TABLE.end(), command[0]) ==
				      ALLOWED_COMMANDS_LOOKUP_TABLE.end())
				{
					retVal = false;
					break;
				}
			}
		}
		return retVal;
	}

	VirtualTerminalObjectType ColourMap::get_object_type() const
	{
		return VirtualTerminalObjectType::ColourMap;
	}

	std::uint32_t ColourMap::get_minumum_object_length() const
	{
		return MIN_OBJECT_LENGTH;
	}

	bool ColourMap::get_is_valid(const std::map<std::uint16_t, std::shared_ptr<VTObject>> &) const
	{
		return true;
	}

	bool ColourMap::set_attribute(std::uint8_t, std::uint32_t, const std::map<std::uint16_t, std::shared_ptr<VTObject>> &, AttributeError &returnedError)
	{
		returnedError = AttributeError::InvalidAttributeID;
		return false;
	}

	bool ColourMap::get_attribute(std::uint8_t attributeID, std::uint32_t &returnedAttributeData) const
	{
		bool retVal = false;

		if (attributeID < static_cast<std::uint8_t>(AttributeName::NumberOfAttributes))
		{
			switch (static_cast<AttributeName>(attributeID))
			{
				case AttributeName::Type:
				{
					returnedAttributeData = static_cast<std::uint32_t>(get_object_type());
					retVal = true;
				}
				break;

				default:
				{
					// Do nothing return false
				}
				break;
			}
		}
		return retVal;
	}

	bool ColourMap::set_number_of_colour_indexes(std::uint16_t value)
	{
		bool retVal = false;

		if ((value != colourMapData.size()) &&
		    ((2 == value) ||
		     (16 == value) ||
		     (256 == value)))
		{
			colourMapData.clear();
			colourMapData.resize(value);

			for (std::size_t i = 0; i < colourMapData.size(); i++)
			{
				colourMapData[i] = static_cast<std::uint8_t>(i);
			}
			retVal = true;
		}
		return retVal;
	}

	std::uint16_t ColourMap::get_number_of_colour_indexes() const
	{
		return static_cast<std::uint16_t>(colourMapData.size());
	}

	bool ColourMap::set_colour_map_index(std::uint8_t index, std::uint8_t value)
	{
		bool retVal = false;

		if (index < colourMapData.size())
		{
			colourMapData[index] = value;
			retVal = true;
		}
		return retVal;
	}

	std::uint8_t ColourMap::get_colour_map_index(std::uint8_t index) const
	{
		std::uint8_t retVal = 0;

		if (index < get_number_of_colour_indexes())
		{
			retVal = colourMapData[index];
		}
		return retVal;
	}

	VirtualTerminalObjectType WindowMask::get_object_type() const
	{
		return VirtualTerminalObjectType::WindowMask;
	}

	std::uint32_t WindowMask::get_minumum_object_length() const
	{
		return MIN_OBJECT_LENGTH;
	}

	bool WindowMask::get_is_valid(const std::map<std::uint16_t, std::shared_ptr<VTObject>> &objectPool) const
	{
		bool anyWrongChildType = false;

		if (WindowType::Freeform != get_window_type())
		{
			if (NULL_OBJECT_ID != title)
			{
				auto titleObject = get_object_by_id(title, objectPool);

				if (nullptr != titleObject)
				{
					if ((VirtualTerminalObjectType::ObjectPointer != titleObject->get_object_type()) &&
					    (VirtualTerminalObjectType::OutputString != titleObject->get_object_type()))
					{
						anyWrongChildType = true;
					}
					else if (VirtualTerminalObjectType::ObjectPointer == titleObject->get_object_type())
					{
						if (0 == titleObject->get_number_children())
						{
							anyWrongChildType = true;
						}
						else
						{
							std::uint16_t titleObjectPointedTo = std::static_pointer_cast<ObjectPointer>(titleObject)->get_child_id(0);
							auto child = get_object_by_id(titleObjectPointedTo, objectPool);

							if ((nullptr != child) && (VirtualTerminalObjectType::OutputString == child->get_object_type()))
							{
								// Valid
							}
							else
							{
								anyWrongChildType = true;
							}
						}
					}
					else
					{
						// Valid
					}
				}
				else
				{
					anyWrongChildType = true;
				}
			}
			else
			{
				anyWrongChildType = true;
			}

			if (NULL_OBJECT_ID != name)
			{
				auto nameObject = get_object_by_id(name, objectPool);

				if (nullptr != nameObject)
				{
					if ((VirtualTerminalObjectType::ObjectPointer != nameObject->get_object_type()) &&
					    (VirtualTerminalObjectType::OutputString != nameObject->get_object_type()))
					{
						anyWrongChildType = true;
					}
					else if (VirtualTerminalObjectType::ObjectPointer == nameObject->get_object_type())
					{
						if (0 == nameObject->get_number_children())
						{
							anyWrongChildType = true;
						}
						else
						{
							std::uint16_t titleObjectPointedTo = std::static_pointer_cast<ObjectPointer>(nameObject)->get_child_id(0);
							auto child = get_object_by_id(titleObjectPointedTo, objectPool);

							if ((nullptr != child) && (VirtualTerminalObjectType::OutputString == child->get_object_type()))
							{
								// Valid
							}
							else
							{
								anyWrongChildType = true;
							}
						}
					}
					else
					{
						// Valid
					}
				}
				else
				{
					anyWrongChildType = true;
				}
			}
			else
			{
				anyWrongChildType = true;
			}

			if (NULL_OBJECT_ID != icon)
			{
				auto nameObject = get_object_by_id(icon, objectPool);

				if (nullptr != nameObject)
				{
					switch (nameObject->get_object_type())
					{
						case VirtualTerminalObjectType::OutputString:
						case VirtualTerminalObjectType::Container:
						case VirtualTerminalObjectType::OutputNumber:
						case VirtualTerminalObjectType::OutputList:
						case VirtualTerminalObjectType::OutputLine:
						case VirtualTerminalObjectType::OutputRectangle:
						case VirtualTerminalObjectType::OutputEllipse:
						case VirtualTerminalObjectType::OutputPolygon:
						case VirtualTerminalObjectType::OutputMeter:
						case VirtualTerminalObjectType::OutputLinearBarGraph:
						case VirtualTerminalObjectType::OutputArchedBarGraph:
						case VirtualTerminalObjectType::GraphicsContext:
						case VirtualTerminalObjectType::PictureGraphic:
						case VirtualTerminalObjectType::ObjectPointer:
						case VirtualTerminalObjectType::ScaledGraphic:
						{
							// Valid
						}
						break;

						default:
						{
							anyWrongChildType = true;
						}
						break;
					}
				}
				else
				{
					anyWrongChildType = true;
				}
			}
			else
			{
				anyWrongChildType = true;
			}
		}
		else if (NULL_OBJECT_ID != title)
		{
			anyWrongChildType = true;
		}

		// Validate the actual child object references for each window type
		switch (static_cast<WindowType>(windowType))
		{
			case WindowType::Freeform:
			{
				// Basically anything goes
			}
			break;

			case WindowType::NumericOutputValueWithUnits1x1:
			case WindowType::NumericOutputValueWithUnits2x1:
			{
				if (2 == get_number_children())
				{
					auto outputNum = get_object_by_id(get_child_id(0), objectPool);
					auto outputString = get_object_by_id(get_child_id(1), objectPool);

					if ((nullptr == outputNum) ||
					    (nullptr == outputString) ||
					    (VirtualTerminalObjectType::OutputNumber != outputNum->get_object_type()) ||
					    (VirtualTerminalObjectType::OutputString != outputString->get_object_type()))
					{
						anyWrongChildType = true;
					}
				}
				else
				{
					anyWrongChildType = true;
				}
			}
			break;

			case WindowType::NumericOutputValueNoUnits1x1:
			case WindowType::NumericOutputValueNoUnits2x1:
			{
				if (1 == get_number_children())
				{
					auto outputNum = get_object_by_id(get_child_id(0), objectPool);

					if ((nullptr == outputNum) ||
					    (VirtualTerminalObjectType::OutputNumber != outputNum->get_object_type()))
					{
						anyWrongChildType = true;
					}
				}
				else
				{
					anyWrongChildType = true;
				}
			}
			break;

			case WindowType::StringOutputValue1x1:
			case WindowType::StringOutputValue2x1:
			{
				if (1 == get_number_children())
				{
					auto outputString = get_object_by_id(get_child_id(0), objectPool);

					if ((nullptr == outputString) ||
					    (VirtualTerminalObjectType::OutputString != outputString->get_object_type()))
					{
						anyWrongChildType = true;
					}
				}
				else
				{
					anyWrongChildType = true;
				}
			}
			break;

			case WindowType::NumericInputValueWithUnits1x1:
			case WindowType::NumericInputValueWithUnits2x1:
			{
				if (2 == get_number_children())
				{
					auto inputNum = get_object_by_id(get_child_id(0), objectPool);
					auto outputString = get_object_by_id(get_child_id(1), objectPool);

					if ((nullptr == inputNum) ||
					    (nullptr == outputString) ||
					    (VirtualTerminalObjectType::InputNumber != inputNum->get_object_type()) ||
					    (VirtualTerminalObjectType::OutputString != outputString->get_object_type()))
					{
						anyWrongChildType = true;
					}
				}
				else
				{
					anyWrongChildType = true;
				}
			}
			break;

			case WindowType::NumericInputValueNoUnits1x1:
			case WindowType::NumericInputValueNoUnits2x1:
			{
				if (1 == get_number_children())
				{
					auto inputNum = get_object_by_id(get_child_id(0), objectPool);

					if ((nullptr == inputNum) ||
					    (VirtualTerminalObjectType::InputNumber != inputNum->get_object_type()))
					{
						anyWrongChildType = true;
					}
				}
				else
				{
					anyWrongChildType = true;
				}
			}
			break;

			case WindowType::StringInputValue1x1:
			case WindowType::StringInputValue2x1:
			{
				if (1 == get_number_children())
				{
					auto inputStr = get_object_by_id(get_child_id(0), objectPool);

					if ((nullptr == inputStr) ||
					    (VirtualTerminalObjectType::InputString != inputStr->get_object_type()))
					{
						anyWrongChildType = true;
					}
				}
				else
				{
					anyWrongChildType = true;
				}
			}
			break;

			case WindowType::HorizontalLinearBarGraphNoUnits1x1:
			case WindowType::HorizontalLinearBarGraphNoUnits2x1:
			{
				if (1 == get_number_children())
				{
					auto outputBargraph = get_object_by_id(get_child_id(0), objectPool);

					if ((nullptr == outputBargraph) ||
					    (VirtualTerminalObjectType::OutputLinearBarGraph != outputBargraph->get_object_type()))
					{
						anyWrongChildType = true;
					}
				}
				else
				{
					anyWrongChildType = true;
				}
			}
			break;

			case WindowType::SingleButton1x1:
			case WindowType::SingleButton2x1:
			{
				if (1 == get_number_children())
				{
					auto button = get_object_by_id(get_child_id(0), objectPool);

					if ((nullptr == button) ||
					    (VirtualTerminalObjectType::Button != button->get_object_type()))
					{
						anyWrongChildType = true;
					}
				}
				else
				{
					anyWrongChildType = true;
				}
			}
			break;

			case WindowType::DoubleButton1x1:
			case WindowType::DoubleButton2x1:
			{
				if (2 == get_number_children())
				{
					auto button1 = get_object_by_id(get_child_id(0), objectPool);
					auto button2 = get_object_by_id(get_child_id(1), objectPool);

					if ((nullptr == button1) ||
					    (nullptr == button2) ||
					    (VirtualTerminalObjectType::Button != button1->get_object_type()) ||
					    (VirtualTerminalObjectType::Button != button2->get_object_type()))
					{
						anyWrongChildType = true;
					}
				}
				else
				{
					anyWrongChildType = true;
				}
			}
			break;

			default:
			{
				anyWrongChildType = true;
			}
			break;
		}
		return !anyWrongChildType;
	}

	bool WindowMask::set_attribute(std::uint8_t attributeID, std::uint32_t rawAttributeData, const std::map<std::uint16_t, std::shared_ptr<VTObject>> &, AttributeError &returnedError)
	{
		bool retVal = false;

		if (attributeID < static_cast<std::uint8_t>(AttributeName::NumberOfAttributes))
		{
			switch (static_cast<AttributeName>(attributeID))
			{
				case AttributeName::BackgroundColour:
				{
					set_background_color(static_cast<std::uint8_t>(rawAttributeData));
					retVal = true;
				}
				break;

				case AttributeName::Options:
				{
					set_options(static_cast<std::uint8_t>(rawAttributeData));
					retVal = true;
				}
				break;

				case AttributeName::Name:
				{
					set_name_object_id(static_cast<std::uint16_t>(rawAttributeData));
					retVal = true;
				}
				break;

				default:
				{
					returnedError = AttributeError::InvalidAttributeID;
				}
				break;
			}
		}
		else
		{
			returnedError = AttributeError::InvalidAttributeID;
		}
		return retVal;
	}

	bool WindowMask::get_attribute(std::uint8_t attributeID, std::uint32_t &returnedAttributeData) const
	{
		bool retVal = false;

		if (attributeID < static_cast<std::uint8_t>(AttributeName::NumberOfAttributes))
		{
			switch (static_cast<AttributeName>(attributeID))
			{
				case AttributeName::Type:
				{
					returnedAttributeData = static_cast<std::uint32_t>(get_object_type());
					retVal = true;
				}
				break;

				case AttributeName::BackgroundColour:
				{
					returnedAttributeData = static_cast<std::uint32_t>(get_background_color());
					retVal = true;
				}
				break;

				case AttributeName::Options:
				{
					returnedAttributeData = optionsBitfield;
					retVal = true;
				}
				break;

				case AttributeName::Name:
				{
					returnedAttributeData = static_cast<std::uint32_t>(get_name_object_id());
					retVal = true;
				}
				break;

				default:
				{
					// Do nothing return false
				}
				break;
			}
		}
		return retVal;
	}

	std::uint16_t WindowMask::get_name_object_id() const
	{
		return name;
	}

	void WindowMask::set_name_object_id(std::uint16_t object)
	{
		name = object;
	}

	std::uint16_t WindowMask::get_title_object_id() const
	{
		return title;
	}

	void WindowMask::set_title_object_id(std::uint16_t object)
	{
		title = object;
	}

	std::uint16_t WindowMask::get_icon_object_id() const
	{
		return icon;
	}

	void WindowMask::set_icon_object_id(std::uint16_t object)
	{
		icon = object;
	}

	WindowMask::WindowType WindowMask::get_window_type() const
	{
		return static_cast<WindowType>(windowType);
	}

	void WindowMask::set_window_type(WindowType type)
	{
		if (static_cast<std::uint8_t>(type) <= static_cast<std::uint8_t>(WindowType::DoubleButton2x1))
		{
			windowType = static_cast<std::uint8_t>(type);
		}
	}

	bool WindowMask::get_option(Options option) const
	{
		return (0 != ((1 << static_cast<std::uint8_t>(option)) & optionsBitfield));
	}

	void WindowMask::set_options(std::uint8_t value)
	{
		optionsBitfield = value;
	}

	void WindowMask::set_option(Options option, bool value)
	{
		if (value)
		{
			optionsBitfield |= (1 << static_cast<std::uint8_t>(option));
		}
		else
		{
			optionsBitfield &= ~(1 << static_cast<std::uint8_t>(option));
		}
	}

	VirtualTerminalObjectType AuxiliaryFunctionType1::get_object_type() const
	{
		return VirtualTerminalObjectType::AuxiliaryFunctionType1;
	}

	std::uint32_t AuxiliaryFunctionType1::get_minumum_object_length() const
	{
		return 6;
	}

	bool AuxiliaryFunctionType1::get_is_valid(const std::map<std::uint16_t, std::shared_ptr<VTObject>> &objectPool) const
	{
		// Despite modern VTs not using this object, we still have to validate it.
		bool anyWrongChildType = false;

		for (const auto &child : children)
		{
			auto childObject = get_object_by_id(child.id, objectPool);

			if (nullptr != childObject)
			{
				switch (childObject->get_object_type())
				{
					case VirtualTerminalObjectType::OutputString:
					case VirtualTerminalObjectType::OutputNumber:
					case VirtualTerminalObjectType::OutputLine:
					case VirtualTerminalObjectType::OutputRectangle:
					case VirtualTerminalObjectType::OutputEllipse:
					case VirtualTerminalObjectType::OutputPolygon:
					case VirtualTerminalObjectType::PictureGraphic:
					{
						// Valid
					}
					break;

					default:
					{
						anyWrongChildType = true;
					}
					break;
				}
			}
			else
			{
				anyWrongChildType = true; // Some invalid child ID
				break;
			}
		}
		return !anyWrongChildType;
	}

	bool AuxiliaryFunctionType1::set_attribute(std::uint8_t, std::uint32_t, const std::map<std::uint16_t, std::shared_ptr<VTObject>> &, AttributeError &returnedError)
	{
		returnedError = AttributeError::InvalidAttributeID;
		return false; // All attributes are read only
	}

	bool AuxiliaryFunctionType1::get_attribute(std::uint8_t attributeID, std::uint32_t &returnedAttributeData) const
	{
		bool retVal = false;

		if (attributeID == static_cast<std::uint8_t>(AttributeName::Type))
		{
			returnedAttributeData = static_cast<std::uint32_t>(get_object_type());
			retVal = true;
		}
		return retVal;
	}

	AuxiliaryFunctionType1::FunctionType AuxiliaryFunctionType1::get_function_type() const
	{
		return functionType;
	}

	void AuxiliaryFunctionType1::set_function_type(FunctionType type)
	{
		functionType = type;
	}

	VirtualTerminalObjectType AuxiliaryFunctionType2::get_object_type() const
	{
		return VirtualTerminalObjectType::AuxiliaryFunctionType2;
	}

	std::uint32_t AuxiliaryFunctionType2::get_minumum_object_length() const
	{
		return 6;
	}

	bool AuxiliaryFunctionType2::get_is_valid(const std::map<std::uint16_t, std::shared_ptr<VTObject>> &objectPool) const
	{
		bool anyWrongChildType = false;

		for (const auto &child : children)
		{
			auto childObject = get_object_by_id(child.id, objectPool);

			if (nullptr != childObject)
			{
				switch (childObject->get_object_type())
				{
					case VirtualTerminalObjectType::OutputString:
					case VirtualTerminalObjectType::OutputNumber:
					case VirtualTerminalObjectType::OutputLine:
					case VirtualTerminalObjectType::OutputList:
					case VirtualTerminalObjectType::OutputRectangle:
					case VirtualTerminalObjectType::OutputEllipse:
					case VirtualTerminalObjectType::OutputPolygon:
					case VirtualTerminalObjectType::OutputMeter:
					case VirtualTerminalObjectType::OutputLinearBarGraph:
					case VirtualTerminalObjectType::OutputArchedBarGraph:
					case VirtualTerminalObjectType::PictureGraphic:
					case VirtualTerminalObjectType::ObjectPointer:
					case VirtualTerminalObjectType::ScaledGraphic:
					{
						// Valid
					}
					break;

					default:
					{
						anyWrongChildType = true;
					}
					break;
				}
			}
			else
			{
				anyWrongChildType = true; // Some invalid child ID
				break;
			}
		}
		return !anyWrongChildType;
	}

	bool AuxiliaryFunctionType2::set_attribute(std::uint8_t attributeID, std::uint32_t rawAttributeData, const std::map<std::uint16_t, std::shared_ptr<VTObject>> &, AttributeError &returnedError)
	{
		bool retVal = false;

		if (attributeID == static_cast<std::uint8_t>(AttributeName::BackgroundColour))
		{
			set_background_color(static_cast<std::uint8_t>(rawAttributeData));
			retVal = true;
		}
		else
		{
			returnedError = AttributeError::InvalidAttributeID;
		}
		return retVal;
	}

	bool AuxiliaryFunctionType2::get_attribute(std::uint8_t attributeID, std::uint32_t &returnedAttributeData) const
	{
		bool retVal = false;

		switch (attributeID)
		{
			case static_cast<std::uint8_t>(AttributeName::Type):
			{
				returnedAttributeData = static_cast<std::uint32_t>(get_object_type());
				retVal = true;
			}
			break;

			case static_cast<std::uint8_t>(AttributeName::BackgroundColour):
			{
				returnedAttributeData = static_cast<std::uint32_t>(get_background_color());
				retVal = true;
			}
			break;

			case static_cast<std::uint8_t>(AttributeName::FunctionAttributes):
			{
				returnedAttributeData = functionAttributesBitfield;
				retVal = true;
			}
			break;

			default:
			{
				// Do nothing return false
			}
			break;
		}
		return retVal;
	}

	AuxiliaryFunctionType2::FunctionType AuxiliaryFunctionType2::get_function_type() const
	{
		return static_cast<FunctionType>(functionAttributesBitfield & 0x1F);
	}

	void AuxiliaryFunctionType2::set_function_type(FunctionType type)
	{
		functionAttributesBitfield &= 0xE0;
		functionAttributesBitfield |= static_cast<std::uint8_t>(type) & 0x1F;
	}

	bool AuxiliaryFunctionType2::get_function_attribute(FunctionAttribute attributeToCheck) const
	{
		return (0 != ((1 << static_cast<std::uint8_t>(attributeToCheck)) & functionAttributesBitfield));
	}

	void AuxiliaryFunctionType2::set_function_attribute(FunctionAttribute attributeToSet, bool value)
	{
		if (value)
		{
			functionAttributesBitfield |= (1 << static_cast<std::uint8_t>(attributeToSet));
		}
		else
		{
			functionAttributesBitfield &= ~(1 << static_cast<std::uint8_t>(attributeToSet));
		}
	}

	VirtualTerminalObjectType AuxiliaryInputType1::get_object_type() const
	{
		return VirtualTerminalObjectType::AuxiliaryInputType1;
	}

	std::uint32_t AuxiliaryInputType1::get_minumum_object_length() const
	{
		return 7;
	}

	bool AuxiliaryInputType1::get_is_valid(const std::map<std::uint16_t, std::shared_ptr<VTObject>> &objectPool) const
	{
		bool anyWrongChildType = false;

		for (const auto &child : children)
		{
			auto childObject = get_object_by_id(child.id, objectPool);

			if (nullptr != childObject)
			{
				switch (childObject->get_object_type())
				{
					case VirtualTerminalObjectType::OutputString:
					case VirtualTerminalObjectType::OutputNumber:
					case VirtualTerminalObjectType::OutputLine:
					case VirtualTerminalObjectType::OutputRectangle:
					case VirtualTerminalObjectType::OutputEllipse:
					case VirtualTerminalObjectType::OutputPolygon:
					case VirtualTerminalObjectType::PictureGraphic:
					{
						// Valid
					}
					break;

					default:
					{
						anyWrongChildType = true;
					}
					break;
				}
			}
			else
			{
				anyWrongChildType = true; // Some invalid child ID
				break;
			}
		}
		return !anyWrongChildType;
	}

	bool AuxiliaryInputType1::set_attribute(std::uint8_t, std::uint32_t, const std::map<std::uint16_t, std::shared_ptr<VTObject>> &, AttributeError &returnedError)
	{
		returnedError = AttributeError::InvalidAttributeID;
		return false; // All attributes are read only
	}

	bool AuxiliaryInputType1::get_attribute(std::uint8_t attributeID, std::uint32_t &returnedAttributeData) const
	{
		bool retVal = false;

		if (attributeID == static_cast<std::uint8_t>(AttributeName::Type))
		{
			returnedAttributeData = static_cast<std::uint32_t>(get_object_type());
			retVal = true;
		}
		return retVal;
	}

	AuxiliaryInputType1::FunctionType AuxiliaryInputType1::get_function_type() const
	{
		return functionType;
	}

	void AuxiliaryInputType1::set_function_type(FunctionType type)
	{
		functionType = type;
	}

	std::uint8_t AuxiliaryInputType1::get_input_id() const
	{
		return inputID;
	}

	bool AuxiliaryInputType1::set_input_id(std::uint8_t id)
	{
		bool retVal = false;

		if (id <= 250) // The range is defined in ISO 11783-6 table J.3
		{
			inputID = id;
			retVal = true;
		}
		return retVal;
	}

	VirtualTerminalObjectType AuxiliaryInputType2::get_object_type() const
	{
		return VirtualTerminalObjectType::AuxiliaryInputType2;
	}

	std::uint32_t AuxiliaryInputType2::get_minumum_object_length() const
	{
		return 6;
	}

	bool AuxiliaryInputType2::get_is_valid(const std::map<std::uint16_t, std::shared_ptr<VTObject>> &objectPool) const
	{
		bool anyWrongChildType = false;

		for (const auto &child : children)
		{
			auto childObject = get_object_by_id(child.id, objectPool);

			if (nullptr != childObject)
			{
				switch (childObject->get_object_type())
				{
					case VirtualTerminalObjectType::OutputString:
					case VirtualTerminalObjectType::OutputNumber:
					case VirtualTerminalObjectType::OutputLine:
					case VirtualTerminalObjectType::OutputList:
					case VirtualTerminalObjectType::OutputRectangle:
					case VirtualTerminalObjectType::OutputEllipse:
					case VirtualTerminalObjectType::OutputPolygon:
					case VirtualTerminalObjectType::OutputMeter:
					case VirtualTerminalObjectType::OutputLinearBarGraph:
					case VirtualTerminalObjectType::OutputArchedBarGraph:
					case VirtualTerminalObjectType::PictureGraphic:
					case VirtualTerminalObjectType::ObjectPointer:
					case VirtualTerminalObjectType::ScaledGraphic:
					{
						// Valid
					}
					break;

					default:
					{
						anyWrongChildType = true;
					}
					break;
				}
			}
			else
			{
				anyWrongChildType = true; // Some invalid child ID
				break;
			}
		}
		return !anyWrongChildType;
	}

	bool AuxiliaryInputType2::set_attribute(std::uint8_t attributeID, std::uint32_t rawAttributeData, const std::map<std::uint16_t, std::shared_ptr<VTObject>> &, AttributeError &returnedError)
	{
		bool retVal = false;

		switch (attributeID)
		{
			case static_cast<std::uint8_t>(AttributeName::BackgroundColour):
			{
				set_background_color(static_cast<std::uint8_t>(rawAttributeData));
				retVal = true;
			}
			break;

			case static_cast<std::uint8_t>(AttributeName::FunctionAttributes):
			{
				functionAttributesBitfield = (static_cast<std::uint8_t>(rawAttributeData));
				retVal = true;
			}
			break;

			default:
			{
				returnedError = AttributeError::InvalidAttributeID;
			}
			break;
		}
		return retVal;
	}

	bool AuxiliaryInputType2::get_attribute(std::uint8_t attributeID, std::uint32_t &returnedAttributeData) const
	{
		bool retVal = false;

		switch (attributeID)
		{
			case static_cast<std::uint8_t>(AttributeName::Type):
			{
				returnedAttributeData = static_cast<std::uint32_t>(get_object_type());
				retVal = true;
			}
			break;

			case static_cast<std::uint8_t>(AttributeName::FunctionAttributes):
			{
				returnedAttributeData = functionAttributesBitfield;
				retVal = true;
			}
			break;

			case static_cast<std::uint8_t>(AttributeName::BackgroundColour):
			{
				returnedAttributeData = get_background_color();
				retVal = true;
			}
			break;

			default:
			{
				// Do nothing return false
			}
			break;
		}
		return retVal;
	}

	AuxiliaryFunctionType2::FunctionType AuxiliaryInputType2::get_function_type() const
	{
		return static_cast<AuxiliaryFunctionType2::FunctionType>(functionAttributesBitfield & 0x1F);
	}

	void AuxiliaryInputType2::set_function_type(AuxiliaryFunctionType2::FunctionType type)
	{
		functionAttributesBitfield &= 0xE0;
		functionAttributesBitfield |= static_cast<std::uint8_t>(type) & 0x1F;
	}

	bool AuxiliaryInputType2::get_function_attribute(FunctionAttribute attributeToCheck) const
	{
		return (0 != ((1 << static_cast<std::uint8_t>(attributeToCheck)) & functionAttributesBitfield));
	}

	void AuxiliaryInputType2::set_function_attribute(FunctionAttribute attributeToSet, bool value)
	{
		if (value)
		{
			functionAttributesBitfield |= (1 << static_cast<std::uint8_t>(attributeToSet));
		}
		else
		{
			functionAttributesBitfield &= ~(1 << static_cast<std::uint8_t>(attributeToSet));
		}
	}

	VirtualTerminalObjectType AuxiliaryControlDesignatorType2::get_object_type() const
	{
		return VirtualTerminalObjectType::AuxiliaryControlDesignatorType2;
	}

	std::uint32_t AuxiliaryControlDesignatorType2::get_minumum_object_length() const
	{
		return 6;
	}

	bool AuxiliaryControlDesignatorType2::get_is_valid(const std::map<std::uint16_t, std::shared_ptr<VTObject>> &objectPool) const
	{
		bool retVal = (((NULL_OBJECT_ID == auxiliaryObjectID) || ((nullptr != get_object_by_id(auxiliaryObjectID, objectPool)))) && (pointerType <= 3));

		if (retVal)
		{
			// Check the referenced object is valid
			auto object = get_object_by_id(auxiliaryObjectID, objectPool);

			if ((VirtualTerminalObjectType::AuxiliaryFunctionType2 == object->get_object_type()) ||
			    (VirtualTerminalObjectType::AuxiliaryInputType2 == object->get_object_type()))
			{
				// Valid
			}
			else
			{
				retVal = false;
			}
		}
		return retVal;
	}

	bool AuxiliaryControlDesignatorType2::set_attribute(std::uint8_t attributeID, std::uint32_t rawAttributeData, const std::map<std::uint16_t, std::shared_ptr<VTObject>> &objectPool, AttributeError &returnedError)
	{
		bool retVal = false;
		returnedError = AttributeError::InvalidAttributeID;

		if (static_cast<std::uint8_t>(AttributeName::AuxiliaryObjectID) == attributeID)
		{
			if ((NULL_OBJECT_ID == rawAttributeData) ||
			    ((nullptr != get_object_by_id(static_cast<std::uint16_t>(rawAttributeData), objectPool)) &&
			     ((VirtualTerminalObjectType::AuxiliaryFunctionType2 == objectPool.at(static_cast<std::uint16_t>(rawAttributeData))->get_object_type()) ||
			      (VirtualTerminalObjectType::AuxiliaryInputType2 == objectPool.at(static_cast<std::uint16_t>(rawAttributeData))->get_object_type()))))
			{
				set_auxiliary_object_id(static_cast<std::uint16_t>(rawAttributeData));
				retVal = true;
			}
			else
			{
				returnedError = AttributeError::InvalidValue;
			}
		}
		return retVal;
	}

	bool AuxiliaryControlDesignatorType2::get_attribute(std::uint8_t attributeID, std::uint32_t &returnedAttributeData) const
	{
		bool retVal = false;

		switch (attributeID)
		{
			case static_cast<std::uint8_t>(AttributeName::Type):
			{
				returnedAttributeData = static_cast<std::uint32_t>(get_object_type());
				retVal = true;
			}
			break;

			case static_cast<std::uint8_t>(AttributeName::PointerType):
			{
				returnedAttributeData = get_pointer_type();
				retVal = true;
			}
			break;

			case static_cast<std::uint8_t>(AttributeName::AuxiliaryObjectID):
			{
				returnedAttributeData = get_auxiliary_object_id();
				retVal = true;
			}
			break;

			default:
			{
				// Invalid attribute ID
			}
			break;
		}
		return retVal;
	}

	std::uint16_t AuxiliaryControlDesignatorType2::get_auxiliary_object_id() const
	{
		return auxiliaryObjectID;
	}

	void AuxiliaryControlDesignatorType2::set_auxiliary_object_id(std::uint16_t id)
	{
		auxiliaryObjectID = id;
	}

	std::uint8_t AuxiliaryControlDesignatorType2::get_pointer_type() const
	{
		return pointerType;
	}

	void AuxiliaryControlDesignatorType2::set_pointer_type(std::uint8_t type)
	{
		pointerType = type;
	}

} // namespace isobus
