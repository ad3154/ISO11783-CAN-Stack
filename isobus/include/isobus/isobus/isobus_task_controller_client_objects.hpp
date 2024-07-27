//================================================================================================
/// @file isobus_task_controller_client_objects.hpp
///
/// @brief Defines a set of C++ objects that represent a DDOP
/// @author Adrian Del Grosso
///
/// @copyright 2023 The Open-Agriculture Developers
//================================================================================================
#ifndef ISOBUS_TASK_CONTROLLER_CLIENT_OBJECTS_HPP
#define ISOBUS_TASK_CONTROLLER_CLIENT_OBJECTS_HPP

#include <array>
#include <cstdint>
#include <string>
#include <vector>

namespace isobus
{
	/// @brief A namespace that contains the generic task controller objects
	namespace task_controller_object
	{
		/// @brief Enumerates the different kinds of DDOP objects
		enum class ObjectTypes
		{
			Device, ///< The root object. Each device shall have one single Device
			DeviceElement, ///< Subcomponent of a device. Has multiple sub-types
			DeviceProcessData, ///< Contains a single process data variable definition
			DeviceProperty, ///< A device property element
			DeviceValuePresentation ///< Contains the presentation information to display the value of a DeviceProcessData or DeviceProperty object
		};

		/// @brief A base class for a Task Controller Object
		class Object
		{
		public:
			/// @brief Constructor for the TC object base class
			/// @param[in] objectDesignator Descriptive text for this object, UTF-8 encoded, 32 characters max
			/// @param[in] uniqueID The object ID of the object. Must be unique in the DDOP.
			Object(std::string objectDesignator, std::uint16_t uniqueID);

			/// @brief Destructor for a TC Object
			virtual ~Object() = default;

			/// @brief Returns the Descriptive text for this object, UTF-8 encoded, 32 characters max
			/// @returns Descriptive text for this object, UTF-8 encoded, 32 characters max
			std::string get_designator() const;

			/// @brief Updates the designator to a new value
			/// @param[in] newDesignator The designator to set, UTF-8 encoded, 32 characters max
			void set_designator(const std::string &newDesignator);

			/// @brief Returns the object ID of the object
			/// @returns The object ID of the object
			std::uint16_t get_object_id() const;

			/// @brief Updates the object ID of the object to a new value.
			/// @param[in] id The object ID to set. IDs must be unique in the DDOP and less than or equal to MAX_OBJECT_ID
			void set_object_id(std::uint16_t id);

			/// @brief Returns the XML namespace for the object
			/// @returns the XML namespace for the object
			virtual std::string get_table_id() const = 0;

			/// @brief Returns the derived TC object type fot the object
			/// @returns The derived TC object type for this object
			virtual ObjectTypes get_object_type() const = 0;

			/// @brief Returns the binary representation of the TC object, or an empty vector if object is invalid
			/// @returns The binary representation of the TC object, or an empty vector if object is invalid
			virtual std::vector<std::uint8_t> get_binary_object() const = 0;

			/// @brief The max allowable "valid" object ID
			static constexpr std::uint16_t MAX_OBJECT_ID = 65534;

			/// @brief Defines the max length of a designator (in bytes)
			static constexpr std::size_t MAX_DESIGNATOR_LENGTH = 128;

			/// @brief Defines the max length of a designator (in bytes) for TCs older than version 4
			static constexpr std::size_t MAX_DESIGNATOR_LEGACY_LENGTH = 32;

		protected:
			std::string designator; ///< UTF-8 Descriptive text to identify this object. Max length of 32.
			std::uint16_t objectID; ///< Unique object ID in the DDOP
		};

		/// @brief Each device shall have one single DeviceObject in its device descriptor object pool.
		/// see A.2 in ISO11783-10
		class DeviceObject : public Object
		{
		public:
			/// @brief Constructor for a DeviceObject
			/// @param[in] deviceDesignator Descriptive text for the object, UTF-8, 32-128 chars max depending on TC version
			/// @param[in] deviceSoftwareVersion Software version indicating text (UTF-8)
			/// @param[in] deviceSerialNumber Device and manufacturer-specific serial number of the Device (UTF-8)
			/// @param[in] deviceStructureLabel This label allows the device to identify the current version of the device descriptor object pool (byte array /ascii)
			/// @param[in] deviceLocalizationLabel Defined by the language command PGN (ascii / byte array)
			/// @param[in] deviceExtendedStructureLabel Continuation of the Label given by Device to identify the Device descriptor Structure (byte array)
			/// @param[in] clientIsoNAME NAME of client device as defined in ISO 11783-5
			/// @param[in] shouldUseExtendedStructureLabel If the device should include the extended structure label during binary serialization
			DeviceObject(std::string deviceDesignator,
			             std::string deviceSoftwareVersion,
			             std::string deviceSerialNumber,
			             std::string deviceStructureLabel,
			             std::array<std::uint8_t, 7> deviceLocalizationLabel,
			             std::vector<std::uint8_t> deviceExtendedStructureLabel,
			             std::uint64_t clientIsoNAME,
			             bool shouldUseExtendedStructureLabel);

			/// @brief Destructor for a DeviceObject
			~DeviceObject() override = default;

			/// @brief Returns the XML namespace for the object
			/// @returns the XML namespace for the object
			std::string get_table_id() const override;

			/// @brief Returns the object type
			/// @returns The object type for this object (Object::Device)
			ObjectTypes get_object_type() const override;

			/// @brief Returns the binary representation of the TC object, or an empty vector if object is invalid
			/// @returns The binary representation of the TC object, or an empty vector if object is invalid
			std::vector<std::uint8_t> get_binary_object() const override;

			/// @brief Returns the software version of the device
			/// @returns The software version of the device
			std::string get_software_version() const;

			/// @brief Sets the software version for the device, as reported in the DDOP.
			/// @param[in] version The software version to set as a UTF-8 string (or ascii).
			void set_software_version(const std::string &version);

			/// @brief Returns the serial number for the device
			/// @returns The serial number for the device
			std::string get_serial_number() const;

			/// @brief Sets the serial number for the device as reported in the DDOP
			/// @param[in] serial The serial number to set as a UTF-8 string (or ascii)
			void set_serial_number(const std::string &serial);

			/// @brief Returns the structure label for this DDOP
			/// @returns The structure label for this DDOP
			std::string get_structure_label() const;

			/// @brief Sets the device structure label to a new value
			/// @param[in] label The new structure label to set
			void set_structure_label(const std::string &label);

			/// @brief Returns the localization label for this DDOP
			/// @returns The  localization label for this DDOP
			std::array<std::uint8_t, 7> get_localization_label() const;

			/// @brief Changes the localization label to a new value
			/// @param[in] label The new label to set
			void set_localization_label(std::array<std::uint8_t, 7> label);

			/// @brief Returns the extended structure label (if applicable)
			/// @returns The extended structure label (if applicable)
			std::vector<std::uint8_t> get_extended_structure_label() const;

			/// @brief Sets the extended structure label to a new value. Only used for TCs with version 4+.
			/// @param[in] label The extended structure label to report or an empty vector if none is being used.
			void set_extended_structure_label(const std::vector<std::uint8_t> &label);

			/// @brief Returns the ISO NAME associated with this DDOP
			/// @returns The raw ISO NAME associated with this DDOP
			std::uint64_t get_iso_name() const;

			/// @brief Changes the stored ISO NAME to a new value
			/// @param[in] name The new ISO NAME to set
			void set_iso_name(std::uint64_t name);

			/// @brief Returns if the class will append the extended structure label to its serialized form
			/// @details This is TC version 4 behavior. For version 3, this should return false.
			/// @returns `true` if the class will append the extended structure label to its serialized form, otherwise `false`
			bool get_use_extended_structure_label() const;

			/// @brief Sets the class' behavior for dealing with the extended structure label.
			/// @details When this is set to true, the class will use TC version 4 behavior for the extended structure label.
			/// When it is false, it will use < version 4 behavior (the label will not be included in the binary object).
			/// @param[in] shouldUseExtendedStructureLabel `true` to use version 4 behavior, `false` to use earlier version behavior
			void set_use_extended_structure_label(bool shouldUseExtendedStructureLabel);

			/// @brief Defines the max length of the device structure label and device localization label (in bytes)
			static constexpr std::size_t MAX_STRUCTURE_AND_LOCALIZATION_LABEL_LENGTH = 7;

			/// @brief Defines the max length of the device extended structure label (in bytes)
			static constexpr std::size_t MAX_EXTENDED_STRUCTURE_LABEL_LENGTH = 32;

		private:
			static const std::string tableID; ///< XML element namespace for device.
			std::string serialNumber; ///< Device and manufacturer-specific serial number of the Device
			std::string softwareVersion; ///< Software version of the device
			std::string structureLabel; ///< Label given by device to identify the device descriptor structure
			std::array<std::uint8_t, task_controller_object::DeviceObject::MAX_STRUCTURE_AND_LOCALIZATION_LABEL_LENGTH> localizationLabel; ///< Label given by device to identify the device descriptor localization
			std::vector<std::uint8_t> extendedStructureLabel; ///< Continuation of the Label given by Device to identify the Device descriptor Structure
			std::uint64_t NAME; ///< The NAME of client device as defined in ISO 11783-5. MUST match your address claim
			bool useExtendedStructureLabel; ///< Tells the device if it should generate binary info using the extended structure label or ignore it
		};

		/// @brief DeviceElementObject is the object definition of the XML element DeviceElement.
		/// The attribute Type specifies the type of this particular element definition
		/// @details Referable Child Objects: DeviceProcessDataObject, DevicePropertyObject
		class DeviceElementObject : public Object
		{
		public:
			/// @brief Enumerates the types of device element object
			enum class Type : std::uint8_t
			{
				Device = 1, ///< The device descriptor object pool shall have one device element of type device
				Function = 2, ///< This device element type can be used as a generic device element to define individually accessible components of a device like valves or sensors
				Bin = 3, ///< This is, for instance, the tank of a sprayer or the bin of a seeder.
				Section = 4, ///< This is, for instance, the section of a spray boom, seed toolbar, or planter toolbar.
				Unit = 5, ///< This device element type is, for example, used for spray boom nozzles, seeder openers, or planter row units.
				Connector = 6, ///< This device element type specifies the mounting/connection position of the device
				NavigationReference = 7 ///< This device element type defines the navigation reference position for navigation devices such as GPS receivers
			};

			/// @brief Constructor for a DeviceElementObject
			/// @param[in] deviceElementDesignator Descriptive text for the object, UTF-8, 32-128 chars max depending on TC version
			/// @param[in] deviceElementNumber The Element number for process data variable	addressing
			/// @param[in] parentObjectID Object ID of parent DeviceElementObject or DeviceObject in order to establish a hierarchical order of DeviceElements
			/// @param[in] deviceEelementType The type of element, such as "device" or "bin"
			/// @param[in] uniqueID The object ID of the object. Must be unique in the DDOP.
			DeviceElementObject(std::string deviceElementDesignator,
			                    std::uint16_t deviceElementNumber,
			                    std::uint16_t parentObjectID,
			                    Type deviceEelementType,
			                    std::uint16_t uniqueID);

			/// @brief Destructor for a DeviceElementObject
			~DeviceElementObject() override = default;

			/// @brief Returns the XML namespace for the object
			/// @returns The string "DET", the XML namespace for the DeviceElementObject
			std::string get_table_id() const override;

			/// @brief Returns the object type
			/// @returns The object type for this object (Object::DeviceElement)
			ObjectTypes get_object_type() const override;

			/// @brief Returns the binary representation of the TC object, or an empty vector if object is invalid
			/// @returns The binary representation of the TC object, or an empty vector if object is invalid
			std::vector<std::uint8_t> get_binary_object() const override;

			/// @brief Returns the element number
			/// @returns The element number
			std::uint16_t get_element_number() const;

			/// @brief Update the object's element number to a new value.
			/// @param[in] newElementNumber The element number to set
			void set_element_number(std::uint16_t newElementNumber);

			/// @brief Returns the parent object ID
			/// @returns The parent object ID
			std::uint16_t get_parent_object() const;

			/// @brief Updates the object ID associated to this object's parent object
			/// @param[in] parentObjectID The object ID to set as the parent to this object
			void set_parent_object(std::uint16_t parentObjectID);

			/// @brief Returns the type of the element object
			/// @returns The type of the element object
			Type get_type() const;

			/// @brief This function can be called to add an object as a child of this object.
			/// @note You should only add Device or Device Element objects as children of this object
			/// @param[in] childID The object ID of the child to reference from this object
			void add_reference_to_child_object(std::uint16_t childID);

			/// @brief Removes a child object reference from this object.
			/// @param[in] childID An object ID associated to a child object to remove.
			/// @returns true if the child object ID was found and removed, otherwise false
			bool remove_reference_to_child_object(std::uint16_t childID);

			/// @brief Returns the number of child objects added with `add_reference_to_child_object`.
			/// @note The maximum number of child objects is technically 65535 because the serialized
			/// form of the value uses a 16-bit integer to store the count.
			/// @returns The number of child objects added with `add_reference_to_child_object`
			std::uint16_t get_number_child_objects() const;

			/// @brief Returns a child object ID by index
			/// @param[in] index The index of the child object ID to return
			/// @returns Child object ID by index, or NULL_OBJECT_ID if the index is out of range
			std::uint16_t get_child_object_id(std::size_t index);

		private:
			static const std::string tableID; ///< XML element namespace for DeviceElement.
			std::vector<std::uint16_t> referenceList; ///< List of references to DeviceProcessDataObjects or DevicePropertyObjects
			std::uint16_t elementNumber; ///< Element number for process data variable addressing
			std::uint16_t parentObject; ///< Object ID of parent DeviceElementObject or DeviceObject in order to establish a hierarchical order of DeviceElements
			Type elementType; ///< See the comments on `Type` or ISO11783-10 table A.2
		};

		/// @brief The DeviceProcessDataObject is the object definition of the XML element DeviceProcessData. Each
		/// object contains a single process data variable definition
		/// @details Referable child object: DeviceValuePresentationObject
		class DeviceProcessDataObject : public Object
		{
		public:
			/// @brief Enumerates the properties in the properties bitset of this object
			enum class PropertiesBit : std::uint8_t
			{
				MemberOfDefaultSet = 0x01, ///< member of default set
				Settable = 0x02, ///< if this object is settable
				ControlSource = 0x04 ///< Version 4, mutually exclusive with bit 2
			};

			/// @brief Enumerates the trigger methods that can be set in the available trigger bitset of this object
			enum class AvailableTriggerMethods : std::uint8_t
			{
				TimeInterval = 0x01, ///< The device can provide these device process data based on a time interval
				DistanceInterval = 0x02, ///< The device can provide these device process data based on a distance interval.
				ThresholdLimits = 0x04, ///< The device can provide these device process data based on a surpassing of the value threshold
				OnChange = 0x08, ///< The device can provide these device process data when its value changes
				Total = 0x10 ///< These device process data are a total
			};

			/// @brief Constructor for a DeviceProcessDataObject
			/// @param[in] processDataDesignator Descriptive text for the object, UTF-8, 32 chars max
			/// @param[in] processDataDDI Identifier of process data variable (DDI) according to definitions in Annex B and ISO 11783 - 11
			/// @param[in] deviceValuePresentationObjectID Object identifier of a DeviceValuePresentationObject, or the null ID
			/// @param[in] processDataProperties A bitset of properties associated to this object. Some combination of `PropertiesBit`
			/// @param[in] processDataTriggerMethods A bitset of available trigger methods, built from some combination of `AvailableTriggerMethods`
			/// @param[in] uniqueID The object ID of the object. Must be unique in the DDOP.
			DeviceProcessDataObject(std::string processDataDesignator,
			                        std::uint16_t processDataDDI,
			                        std::uint16_t deviceValuePresentationObjectID,
			                        std::uint8_t processDataProperties,
			                        std::uint8_t processDataTriggerMethods,
			                        std::uint16_t uniqueID);

			/// @brief Destructor for a DeviceProcessDataObject
			~DeviceProcessDataObject() override = default;

			/// @brief Returns the XML element namespace for DeviceProcess-Data.
			/// @returns The string "DPD", the XML element namespace for DeviceProcess-Data.
			std::string get_table_id() const override;

			/// @brief Returns the object type
			/// @returns The object type for this object (Object::DeviceProcessData)
			ObjectTypes get_object_type() const override;

			/// @brief Returns the binary representation of the TC object, or an empty vector if object is invalid
			/// @returns The binary representation of the TC object, or an empty vector if object is invalid
			std::vector<std::uint8_t> get_binary_object() const override;

			/// @brief Returns the DDI
			/// @returns the DDI for this property
			std::uint16_t get_ddi() const;

			/// @brief Updates the DDI associated to this DPD object
			/// @param[in] newDDI The DDI to associate with this object
			void set_ddi(std::uint16_t newDDI);

			/// @brief Returns Object identifier of the DeviceValuePresentation-Object for this object, or the null ID
			/// @returns Object identifier of DeviceValuePresentation-Object for this object
			std::uint16_t get_device_value_presentation_object_id() const;

			/// @brief Updates the object ID to use as an associated presentation for this object
			/// @param[in] id Object identifier of the DeviceValuePresentation to use for this object, or the null ID for none
			void set_device_value_presentation_object_id(std::uint16_t id);

			/// @brief Returns the object's properties bitfield
			/// @returns The properties bitfield for this object
			std::uint8_t get_properties_bitfield() const;

			/// @brief Updates the properties bitfield to a new value
			/// @param[in] properties The new properties bitfield to set
			void set_properties_bitfield(std::uint8_t properties);

			/// @brief Returns the object's available trigger methods
			/// @returns The available trigger methods bitfield for this object
			std::uint8_t get_trigger_methods_bitfield() const;

			/// @brief Updates the object's available trigger methods bitfield to a new value
			/// @param[in] methods The new trigger methods bitfield to set
			void set_trigger_methods_bitfield(std::uint8_t methods);

		private:
			static const std::string tableID; ///< XML element namespace for DeviceProcessData.
			std::uint16_t ddi; ///< Identifier of process data variable
			std::uint16_t deviceValuePresentationObject; ///< Object identifier of DeviceValuePresentation-Object
			std::uint8_t propertiesBitfield; ///<  A bitset of properties for this object
			std::uint8_t triggerMethodsBitfield; ///< A bitset defined in A.4.1 to A.4.5
		};

		/// @brief DevicePropertyObject is the object definition of the XML element DeviceProperty. Each object contains
		/// a single DeviceElementProperty definition
		/// @details Referable child object: DeviceValuePresentationObject
		class DevicePropertyObject : public Object
		{
		public:
			/// @brief Constructor for a DevicePropertyObject
			/// @param[in] propertyDesignator Descriptive text for the object, UTF-8, 32 chars max
			/// @param[in] propertyValue The value of the property
			/// @param[in] propertyDDI Identifier of property (DDI) according to definitions in Annex B and ISO 11783 - 11.
			/// @param[in] valuePresentationObject Object identifier of DeviceValuePresentationObject, or NULL object ID
			/// @param[in] uniqueID The object ID of the object. Must be unique in the DDOP.
			DevicePropertyObject(std::string propertyDesignator,
			                     std::int32_t propertyValue,
			                     std::uint16_t propertyDDI,
			                     std::uint16_t valuePresentationObject,
			                     std::uint16_t uniqueID);

			/// @brief Destructor for a DevicePropertyObject
			~DevicePropertyObject() override = default;

			/// @brief Returns the XML element namespace for DeviceProperty.
			/// @returns The string "DPT", the XML element namespace for DeviceProperty.
			std::string get_table_id() const override;

			/// @brief Returns the object type
			/// @returns The object type for this object (Object::DeviceProperty)
			ObjectTypes get_object_type() const override;

			/// @brief Returns the binary representation of the TC object, or an empty vector if object is invalid
			/// @returns The binary representation of the TC object, or an empty vector if object is invalid
			std::vector<std::uint8_t> get_binary_object() const override;

			/// @brief Returns the property's value
			/// @returns The property's value
			std::int32_t get_value() const;

			/// @brief Sets the property value
			/// @param[in] newValue The value to set the property to
			void set_value(std::int32_t newValue);

			/// @brief Returns the DDI for this object
			/// @returns The DDI for this object
			std::uint16_t get_ddi() const;

			/// @brief Updates the DDI associated with this DPT object to a new value
			/// @param[in] newDDI The DDI to associate with this object's value
			void set_ddi(std::uint16_t newDDI);

			/// @brief Returns the object identifier of an associated DeviceValuePresentationObject
			/// @returns The object identifier of an associated DeviceValuePresentationObject
			std::uint16_t get_device_value_presentation_object_id() const;

			/// @brief Updates the object ID to use as an associated presentation for this object
			/// @param[in] id Object identifier of the DeviceValuePresentation to use for this object, or the null ID for none
			void set_device_value_presentation_object_id(std::uint16_t id);

		private:
			static const std::string tableID; ///< XML element namespace for DeviceProperty.
			std::int32_t value; ///< The value of the property.
			std::uint16_t ddi; ///< Identifier of property (DDI) according to definitions in Annex B and ISO 11783 - 11.
			std::uint16_t deviceValuePresentationObject; ///< Object identifier of DeviceValuePresentationObject
		};

		/// @brief This object contains the presentation information to display the value of a DeviceProcessData or
		/// DeviceProperty object.The device can update these objects when the language and/or units of measure
		/// are changed by the operator
		/// @details Referable child objects: none.
		class DeviceValuePresentationObject : public Object
		{
		public:
			/// @brief The constructor for a DeviceValuePresentationObject
			/// @param[in] unitDesignator Unit designator for this value presentation
			/// @param[in] offsetValue Offset to be applied to the value for presentation.
			/// @param[in] scaleFactor Scale to be applied to the value for presentation.
			/// @param[in] numberDecimals Specifies the number of decimals to display after the decimal point.
			/// @param[in] uniqueID The object ID of the object. Must be unique in the DDOP.
			DeviceValuePresentationObject(std::string unitDesignator,
			                              std::int32_t offsetValue,
			                              float scaleFactor,
			                              std::uint8_t numberDecimals,
			                              std::uint16_t uniqueID);

			/// @brief Destructor for a DeviceValuePresentationObject
			~DeviceValuePresentationObject() override = default;

			/// @brief Returns the XML element namespace for DeviceValuePresentation.
			/// @returns The string "DPD", the XML element namespace for DeviceProcessData.
			std::string get_table_id() const override;

			/// @brief Returns the object type
			/// @returns The object type for this object (Object::DeviceValuePresentation)
			ObjectTypes get_object_type() const override;

			/// @brief Returns the binary representation of the TC object, or an empty vector if object is invalid
			/// @returns The binary representation of the TC object, or an empty vector if object is invalid
			std::vector<std::uint8_t> get_binary_object() const override;

			/// @brief Returns the offset that is applied to the value for presentation
			/// @returns The offset that is applied to the value for presentation
			std::int32_t get_offset() const;

			/// @brief Sets the offset that is applied to the value for presentation
			/// @param[in] newOffset The offset to set for this object's value
			void set_offset(std::int32_t newOffset);

			/// @brief Returns the scale that is applied to the value for presentation
			/// @returns The scale that is applied to the value for presentation
			float get_scale() const;

			/// @brief Sets the scale which will be applied to the value for presentation
			/// @param[in] newScale The scale to set for this object's value
			void set_scale(float newScale);

			/// @brief Returns the number of decimals shown after the decimal point
			/// @returns The number of decimals shown after the decimal point
			std::uint8_t get_number_of_decimals() const;

			/// @brief Sets the number of decimals to show when presenting objects associated with this presentation
			/// @param[in] decimals The number of decimals to show after the decimal point
			void set_number_of_decimals(std::uint8_t decimals);

		private:
			static const std::string tableID; ///< XML element namespace for DeviceValuePresentation.
			std::int32_t offset; ///< Offset to be applied to the value for presentation
			float scale; ///< Scale to be applied to the value for presentation
			std::uint8_t numberOfDecimals; ///< Specify number of decimals to display after the decimal point
		};
	} // namespace task_controller_object
} // namespace isobus

#endif // ISOBUS_TASK_CONTROLLER_CLIENT_OBJECTS_HPP
