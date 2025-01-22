//================================================================================================
/// @file isobus_data_dictionary.hpp
///
/// @brief This file contains the definition of an auto-generated lookup of all ISOBUS DDIs
/// as defined in ISO11783-11, exported from isobus.net.
/// This file was generated January 16, 2025.
///
/// @author Adrian Del Grosso
/// @author Daan Steenbergen
/// @copyright 2024 The Open-Agriculture Developers
//================================================================================================
#ifndef ISOBUS_DATA_DICTIONARY_HPP
#define ISOBUS_DATA_DICTIONARY_HPP

#include <cstdint>
#include <string>

namespace isobus
{
	/// @brief This class contains the definition of an auto-generated lookup of all ISOBUS DDIs
	class DataDictionary
	{
	public:
		/// @brief A class containing the information for a single DDI
		class Entry
		{
		public:
			const std::uint16_t ddi; ///< The DDI number

			const std::string name; ///< The name of the DDI
			const std::string unitSymbol; ///< The symbol of the unit of the DDI, if any
			const std::string unitDescription; ///< The description of the unit of the DDI, or "n.a." if no unit
			const float resolution; ///< The resolution of the DDI
			const std::pair<float, float> displayRange; ///< The display range of the DDI

			/**
			 * @brief Get the string representation of this Entry
			 * @return The string representation
			 */
			std::string to_string() const;

			/**
			 * @brief Format a value to a string based on this Entry
			 * @param value The value to format
			 * @return The formatted value
			 */
			std::string format_value(std::int32_t value) const;
		};

		/**
		 * @brief Get the string representation of a device data identifier
		 * @param ddi The device data identifier
		 * @return The string representation
		 */
		static std::string ddi_to_string(std::uint16_t ddi);

		/**
		 * @brief Format a value to a string based on a device data identifier
		 * @param ddi The device data identifier
		 * @param value The value to format
		 * @return The formatted value
		 */
		static std::string format_value_with_ddi(std::uint16_t ddi, std::int32_t value);

		/// @brief Checks the ISO 11783-11 database for the given DDI number
		/// and returns the corresponding entry if found.
		/// @param dataDictionaryIdentifier The DDI number to look up
		/// @return The entry for the given DDI number, or a default entry if not found
		static const Entry &get_entry(std::uint16_t dataDictionaryIdentifier);

	private:
#ifndef DISABLE_ISOBUS_DATA_DICTIONARY
		static const Entry DDI_ENTRIES[724]; ///< A lookup table of all DDI entries in ISO11783-11
#endif
		static const Entry DEFAULT_ENTRY; ///< A default "unknown" DDI to return if a DDI is not in the database
	};
} // namespace isobus

#endif // ISOBUS_DATA_DICTIONARY_HPP
