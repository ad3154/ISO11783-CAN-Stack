//================================================================================================
/// @file isobus_virtual_terminal_working_set_base.hpp
///
/// @brief A base class for a VT working set that isolates common working set functionality
/// so that things useful to VT designer application and a VT server application can be shared.
/// @author Adrian Del Grosso
///
/// @copyright 2024 The Open-Agriculture Developers
//================================================================================================
#ifndef ISOBUS_VIRTUAL_TERMINAL_WORKING_SET_BASE_HPP
#define ISOBUS_VIRTUAL_TERMINAL_WORKING_SET_BASE_HPP

#include "isobus/isobus/isobus_virtual_terminal_objects.hpp"

#include <mutex>

namespace isobus
{
	/// @brief A base class for a VT working set that isolates common working set functionality
	/// so that things useful to VT designer application and a VT server application can be shared.
	class VirtualTerminalWorkingSetBase
	{
	public:
		/// @brief Takes a raw block of IOP data and parses it into VT objects
		/// @param[in] iopData A pointer to the raw IOP data
		/// @param[in] iopLength The length of the raw IOP data
		/// @returns true if the IOP data was parsed successfully, otherwise false
		bool parse_iop_into_objects(std::uint8_t *iopData, std::uint32_t iopLength);

		/// @brief Returns a colour from this working set's current colour table, by index
		/// @param[in] colourIndex The index into the VT's colour table to retrieve
		/// @returns A colour from this working set's current colour table, by index
		VTColourVector get_colour(std::uint8_t colourIndex) const;

		/// @brief Returns the working set's object tree
		/// @returns The working set's object tree
		const std::map<std::uint16_t, std::shared_ptr<VTObject>> &get_object_tree() const;

		/// @brief Returns a VT object from the object tree by object ID
		/// @param[in] objectID The object ID to retrieve from the object tree
		/// @returns A VT object from the object tree by object ID, or an empty shared pointer if not found
		std::shared_ptr<VTObject> get_object_by_id(std::uint16_t objectID);

		/// @brief Returns the working set object in the object pool, if one exists
		/// @returns The working set object in the object pool, if one exists, otherwise an empty shared pointer
		std::shared_ptr<VTObject> get_working_set_object();

		/// @brief Appends raw IOP data to the working set's IOP file data
		/// @param[in] dataToAdd The raw IOP data to add to the working set
		void add_iop_raw_data(const std::vector<std::uint8_t> &dataToAdd);

		/// @brief Returns the number of discrete IOP file chunks that have been added to the object pool
		/// @returns The number of discrete IOP file chunks that have been added to the object pool
		std::size_t get_number_iop_files() const;

		/// @brief Returns IOP file data by index of IOP file
		/// @param[in] index The index of the IOP file to retrieve
		/// @returns The IOP file data by index of IOP file
		std::vector<std::uint8_t> &get_iop_raw_data(std::size_t index);

		/// @brief Returns the object ID of the the faulting object if parsing the object pool failed
		/// @returns The object ID of the faulting object if parsing the object pool failed
		std::uint16_t get_object_pool_faulting_object_id();

	protected:
		/// @brief Adds an object to the object tree, and replaces an object
		/// if there's already one in the tree with the same ID.
		/// @param[in] objectToAdd The object to add to the object tree
		/// @returns true if the object was added or replaced, otherwise false
		bool add_or_replace_object(std::shared_ptr<VTObject> objectToAdd);

		/// @brief Parses one object in the remaining object pool data
		/// @param[in,out] iopData A pointer to some object pool data
		/// @param[in,out] iopLength The number of bytes remaining in the object pool
		/// @returns true if an object was parsed
		bool parse_next_object(std::uint8_t *&iopData, std::uint32_t &iopLength);

		/// @brief Checks if the object pool contains an object with the supplied object ID
		/// @param[in] objectID The object ID to check for in the object pool
		/// @returns true if an object with the specified ID exists in the object pool
		bool get_object_id_exists(std::uint16_t objectID);

		/// @brief Returns the event ID from a byte. Does validation to ensure that the byte is valid.
		/// If the proprietary range or reserved range is used, it will be considered invalid and event 0 will be returned.
		/// @param[in] eventByte The byte to convert to an event ID
		/// @returns The event ID from a byte, or event 0 if the byte is invalid
		static EventID get_event_from_byte(std::uint8_t eventByte);

		/// @brief Sets the object ID associated with a faulting object during pool parsing
		/// @param[in] value The object ID to set as the faulting object
		void set_object_pool_faulting_object_id(std::uint16_t value);

		std::mutex managedWorkingSetMutex; ///< A mutex to protect the interface of the managed working set
		VTColourTable workingSetColourTable; ///< This working set's colour table
		std::uint32_t iopSize = 0; ///< Total size of the IOP in bytes
		std::uint32_t transferredIopSize = 0; ///< Total number of IOP bytes transferred
		std::map<std::uint16_t, std::shared_ptr<VTObject>> vtObjectTree; ///< The C++ object representation (deserialized) of the object pool being managed
		std::vector<std::vector<std::uint8_t>> iopFilesRawData; ///< Raw IOP File data from the client
		std::uint16_t workingSetID = NULL_OBJECT_ID; ///< Stores the object ID of the working set object itself
		std::uint16_t faultingObjectID = NULL_OBJECT_ID; ///< Stores the faulting object ID to send to a client when parsing the pool fails
	};
} // namespace isobus
#endif // ISOBUS_VIRTUAL_TERMINAL_WORKING_SET_BASE_HPP
