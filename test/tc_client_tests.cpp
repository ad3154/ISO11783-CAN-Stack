#include <gtest/gtest.h>

#include "isobus/hardware_integration/can_hardware_interface.hpp"
#include "isobus/hardware_integration/virtual_can_plugin.hpp"
#include "isobus/isobus/can_network_manager.hpp"
#include "isobus/isobus/isobus_standard_data_description_indices.hpp"
#include "isobus/isobus/isobus_task_controller_client.hpp"
#include "isobus/isobus/isobus_virtual_terminal_client.hpp"
#include "isobus/utility/system_timing.hpp"

#include "helpers/control_function_helpers.hpp"

using namespace isobus;

class DerivedTestTCClient : public TaskControllerClient
{
public:
	DerivedTestTCClient(std::shared_ptr<PartneredControlFunction> partner, std::shared_ptr<InternalControlFunction> clientSource, std::shared_ptr<PartneredControlFunction> primaryVT = nullptr) :
	  TaskControllerClient(partner, clientSource, primaryVT){};

	bool test_wrapper_send_working_set_master() const
	{
		return TaskControllerClient::send_working_set_master();
	}

	void test_wrapper_set_state(TaskControllerClient::StateMachineState newState)
	{
		TaskControllerClient::set_state(newState);
	}

	void test_wrapper_set_state(TaskControllerClient::StateMachineState newState, std::uint32_t timestamp_ms)
	{
		TaskControllerClient::set_state(newState, timestamp_ms);
	}

	TaskControllerClient::StateMachineState test_wrapper_get_state() const
	{
		return TaskControllerClient::get_state();
	}

	bool test_wrapper_send_version_request() const
	{
		return TaskControllerClient::send_version_request();
	}

	bool test_wrapper_send_request_version_response() const
	{
		return TaskControllerClient::send_request_version_response();
	}

	bool test_wrapper_send_request_structure_label() const
	{
		return TaskControllerClient::send_request_structure_label();
	}

	bool test_wrapper_send_request_localization_label() const
	{
		return TaskControllerClient::send_request_localization_label();
	}

	bool test_wrapper_send_delete_object_pool() const
	{
		return TaskControllerClient::send_delete_object_pool();
	}

	bool test_wrapper_send_pdack(std::uint16_t elementNumber, std::uint16_t ddi) const
	{
		return TaskControllerClient::send_pdack(elementNumber, ddi);
	}

	bool test_wrapper_send_value_command(std::uint16_t elementNumber, std::uint16_t ddi, std::uint32_t value) const
	{
		return TaskControllerClient::send_value_command(elementNumber, ddi, value);
	}

	bool test_wrapper_process_internal_object_pool_upload_callback(std::uint32_t callbackIndex,
	                                                               std::uint32_t bytesOffset,
	                                                               std::uint32_t numberOfBytesNeeded,
	                                                               std::uint8_t *chunkBuffer,
	                                                               void *parentPointer)
	{
		return TaskControllerClient::process_internal_object_pool_upload_callback(callbackIndex, bytesOffset, numberOfBytesNeeded, chunkBuffer, parentPointer);
	}

	void test_wrapper_process_tx_callback(std::uint32_t parameterGroupNumber,
	                                      std::uint32_t dataLength,
	                                      std::shared_ptr<InternalControlFunction> sourceControlFunction,
	                                      std::shared_ptr<ControlFunction> destinationControlFunction,
	                                      bool successful,
	                                      void *parentPointer)
	{
		TaskControllerClient::process_tx_callback(parameterGroupNumber, dataLength, sourceControlFunction, destinationControlFunction, successful, parentPointer);
	}

	bool test_wrapper_request_task_controller_identification() const
	{
		return TaskControllerClient::request_task_controller_identification();
	}

	void test_wrapper_process_labels_from_ddop()
	{
		TaskControllerClient::process_labels_from_ddop();
	}

	static const std::uint8_t testBinaryDDOP[];
};

// clang-format off
// array size is 2356
// This is the binary version of the example program's DDOP when it gets serialized
const uint8_t DerivedTestTCClient::testBinaryDDOP[]  = {
  0x44, 0x56, 0x43, 0x00, 0x00, 0x11, 0x49, 0x73, 0x6f, 0x62, 0x75, 0x73, 0x2b, 0x2b, 0x20, 0x55, 
  0x6e, 0x69, 0x74, 0x54, 0x65, 0x73, 0x74, 0x05, 0x31, 0x2e, 0x30, 0x2e, 0x30, 0x02, 0x00, 0x00, 
  0x08, 0x00, 0x80, 0x0c, 0xa0, 0x03, 0x31, 0x32, 0x33, 0x49, 0x2b, 0x2b, 0x31, 0x2e, 0x30, 0x20, 
  0x65, 0x6e, 0x50, 0x00, 0x55, 0x55, 0xff, 0x44, 0x45, 0x54, 0x01, 0x00, 0x01, 0x07, 0x53, 0x70, 
  0x72, 0x61, 0x79, 0x65, 0x72, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x02, 0x00, 0x04, 0x00, 0x44, 
  0x50, 0x44, 0x02, 0x00, 0x8d, 0x00, 0x01, 0x08, 0x11, 0x41, 0x63, 0x74, 0x75, 0x61, 0x6c, 0x20, 
  0x57, 0x6f, 0x72, 0x6b, 0x20, 0x53, 0x74, 0x61, 0x74, 0x65, 0xff, 0xff, 0x44, 0x50, 0x44, 0x03, 
  0x00, 0x03, 0x00, 0x00, 0x10, 0x12, 0x52, 0x65, 0x71, 0x75, 0x65, 0x73, 0x74, 0x20, 0x44, 0x65, 
  0x66, 0x61, 0x75, 0x6c, 0x74, 0x20, 0x50, 0x44, 0xff, 0xff, 0x44, 0x50, 0x44, 0x04, 0x00, 0x77, 
  0x00, 0x03, 0x10, 0x0a, 0x54, 0x6f, 0x74, 0x61, 0x6c, 0x20, 0x54, 0x69, 0x6d, 0x65, 0x3b, 0x04, 
  0x44, 0x45, 0x54, 0x05, 0x00, 0x06, 0x09, 0x43, 0x6f, 0x6e, 0x6e, 0x65, 0x63, 0x74, 0x6f, 0x72, 
  0x01, 0x00, 0x01, 0x00, 0x03, 0x00, 0x06, 0x00, 0x07, 0x00, 0x08, 0x00, 0x44, 0x50, 0x44, 0x06, 
  0x00, 0x86, 0x00, 0x02, 0x00, 0x0b, 0x43, 0x6f, 0x6e, 0x6e, 0x65, 0x63, 0x74, 0x6f, 0x72, 0x20, 
  0x58, 0x3c, 0x04, 0x44, 0x50, 0x44, 0x07, 0x00, 0x87, 0x00, 0x02, 0x00, 0x0b, 0x43, 0x6f, 0x6e, 
  0x6e, 0x65, 0x63, 0x74, 0x6f, 0x72, 0x20, 0x59, 0x3c, 0x04, 0x44, 0x50, 0x54, 0x08, 0x00, 0x9d, 
  0x00, 0x09, 0x00, 0x00, 0x00, 0x04, 0x54, 0x79, 0x70, 0x65, 0xff, 0xff, 0x44, 0x45, 0x54, 0x09, 
  0x00, 0x02, 0x04, 0x42, 0x6f, 0x6f, 0x6d, 0x02, 0x00, 0x01, 0x00, 0x17, 0x00, 0x0f, 0x00, 0x10, 
  0x00, 0x11, 0x00, 0x0b, 0x00, 0x0e, 0x00, 0x12, 0x04, 0x22, 0x04, 0x12, 0x00, 0x13, 0x00, 0x14, 
  0x00, 0x15, 0x00, 0x16, 0x00, 0x17, 0x00, 0x18, 0x00, 0x19, 0x00, 0x1a, 0x00, 0x1b, 0x00, 0x1c, 
  0x00, 0x1d, 0x00, 0x1e, 0x00, 0x1f, 0x00, 0x20, 0x00, 0x21, 0x00, 0x44, 0x50, 0x54, 0x0f, 0x00, 
  0x86, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x4f, 0x66, 0x66, 0x73, 0x65, 0x74, 0x20, 0x58, 0x3c, 
  0x04, 0x44, 0x50, 0x54, 0x10, 0x00, 0x87, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x4f, 0x66, 0x66, 
  0x73, 0x65, 0x74, 0x20, 0x59, 0x3c, 0x04, 0x44, 0x50, 0x54, 0x11, 0x00, 0x88, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x08, 0x4f, 0x66, 0x66, 0x73, 0x65, 0x74, 0x20, 0x5a, 0x3c, 0x04, 0x44, 0x50, 0x44, 
  0x0b, 0x00, 0x43, 0x00, 0x01, 0x08, 0x14, 0x41, 0x63, 0x74, 0x75, 0x61, 0x6c, 0x20, 0x57, 0x6f, 
  0x72, 0x6b, 0x69, 0x6e, 0x67, 0x20, 0x57, 0x69, 0x64, 0x74, 0x68, 0x3d, 0x04, 0x44, 0x50, 0x44, 
  0x0d, 0x00, 0x21, 0x01, 0x03, 0x08, 0x13, 0x53, 0x65, 0x74, 0x70, 0x6f, 0x69, 0x6e, 0x74, 0x20, 
  0x57, 0x6f, 0x72, 0x6b, 0x20, 0x53, 0x74, 0x61, 0x74, 0x65, 0xff, 0xff, 0x44, 0x50, 0x44, 0x0c, 
  0x00, 0x74, 0x00, 0x01, 0x10, 0x0a, 0x41, 0x72, 0x65, 0x61, 0x20, 0x54, 0x6f, 0x74, 0x61, 0x6c, 
  0x3a, 0x04, 0x44, 0x50, 0x44, 0x0e, 0x00, 0xa0, 0x00, 0x03, 0x09, 0x15, 0x53, 0x65, 0x63, 0x74, 
  0x69, 0x6f, 0x6e, 0x20, 0x43, 0x6f, 0x6e, 0x74, 0x72, 0x6f, 0x6c, 0x20, 0x53, 0x74, 0x61, 0x74, 
  0x65, 0xff, 0xff, 0x44, 0x45, 0x54, 0x32, 0x04, 0x03, 0x07, 0x50, 0x72, 0x6f, 0x64, 0x75, 0x63, 
  0x74, 0x03, 0x00, 0x09, 0x00, 0x07, 0x00, 0x33, 0x04, 0x34, 0x04, 0x35, 0x04, 0x36, 0x04, 0x37, 
  0x04, 0x38, 0x04, 0x39, 0x04, 0x44, 0x50, 0x44, 0x33, 0x04, 0x49, 0x00, 0x01, 0x09, 0x0d, 0x54, 
  0x61, 0x6e, 0x6b, 0x20, 0x43, 0x61, 0x70, 0x61, 0x63, 0x69, 0x74, 0x79, 0x3e, 0x04, 0x44, 0x50, 
  0x44, 0x34, 0x04, 0x48, 0x00, 0x03, 0x09, 0x0b, 0x54, 0x61, 0x6e, 0x6b, 0x20, 0x56, 0x6f, 0x6c, 
  0x75, 0x6d, 0x65, 0x3e, 0x04, 0x44, 0x50, 0x44, 0x35, 0x04, 0x45, 0x01, 0x01, 0x10, 0x15, 0x4c, 
  0x69, 0x66, 0x65, 0x74, 0x69, 0x6d, 0x65, 0x20, 0x54, 0x6f, 0x74, 0x61, 0x6c, 0x20, 0x56, 0x6f, 
  0x6c, 0x75, 0x6d, 0x65, 0x3e, 0x04, 0x44, 0x50, 0x44, 0x36, 0x04, 0x9e, 0x00, 0x03, 0x09, 0x10, 
  0x52, 0x78, 0x20, 0x43, 0x6f, 0x6e, 0x74, 0x72, 0x6f, 0x6c, 0x20, 0x53, 0x74, 0x61, 0x74, 0x65, 
  0xff, 0xff, 0x44, 0x50, 0x44, 0x38, 0x04, 0x01, 0x00, 0x03, 0x08, 0x0b, 0x54, 0x61, 0x72, 0x67, 
  0x65, 0x74, 0x20, 0x52, 0x61, 0x74, 0x65, 0x3f, 0x04, 0x44, 0x50, 0x44, 0x39, 0x04, 0x02, 0x00, 
  0x01, 0x09, 0x0b, 0x41, 0x63, 0x74, 0x75, 0x61, 0x6c, 0x20, 0x52, 0x61, 0x74, 0x65, 0x3f, 0x04, 
  0x44, 0x50, 0x54, 0x37, 0x04, 0xb3, 0x00, 0x03, 0x00, 0x00, 0x00, 0x0e, 0x4f, 0x70, 0x65, 0x72, 
  0x61, 0x74, 0x69, 0x6f, 0x6e, 0x20, 0x54, 0x79, 0x70, 0x65, 0xff, 0xff, 0x44, 0x45, 0x54, 0x12, 
  0x00, 0x04, 0x09, 0x53, 0x65, 0x63, 0x74, 0x69, 0x6f, 0x6e, 0x20, 0x30, 0x04, 0x00, 0x09, 0x00, 
  0x03, 0x00, 0x12, 0x02, 0x12, 0x01, 0x12, 0x03, 0x44, 0x50, 0x54, 0x12, 0x01, 0x86, 0x00, 0xec, 
  0xff, 0xff, 0xff, 0x08, 0x4f, 0x66, 0x66, 0x73, 0x65, 0x74, 0x20, 0x58, 0x3d, 0x04, 0x44, 0x50, 
  0x54, 0x12, 0x02, 0x87, 0x00, 0x07, 0xbd, 0xff, 0xff, 0x08, 0x4f, 0x66, 0x66, 0x73, 0x65, 0x74, 
  0x20, 0x59, 0x3d, 0x04, 0x44, 0x50, 0x54, 0x12, 0x03, 0x43, 0x00, 0xee, 0x08, 0x00, 0x00, 0x05, 
  0x57, 0x69, 0x64, 0x74, 0x68, 0x3d, 0x04, 0x44, 0x45, 0x54, 0x13, 0x00, 0x04, 0x09, 0x53, 0x65, 
  0x63, 0x74, 0x69, 0x6f, 0x6e, 0x20, 0x31, 0x05, 0x00, 0x09, 0x00, 0x03, 0x00, 0x13, 0x02, 0x13, 
  0x01, 0x13, 0x03, 0x44, 0x50, 0x54, 0x13, 0x01, 0x86, 0x00, 0xec, 0xff, 0xff, 0xff, 0x08, 0x4f, 
  0x66, 0x66, 0x73, 0x65, 0x74, 0x20, 0x58, 0x3d, 0x04, 0x44, 0x50, 0x54, 0x13, 0x02, 0x87, 0x00, 
  0xf5, 0xc5, 0xff, 0xff, 0x08, 0x4f, 0x66, 0x66, 0x73, 0x65, 0x74, 0x20, 0x59, 0x3d, 0x04, 0x44, 
  0x50, 0x54, 0x13, 0x03, 0x43, 0x00, 0xee, 0x08, 0x00, 0x00, 0x05, 0x57, 0x69, 0x64, 0x74, 0x68, 
  0x3d, 0x04, 0x44, 0x45, 0x54, 0x14, 0x00, 0x04, 0x09, 0x53, 0x65, 0x63, 0x74, 0x69, 0x6f, 0x6e, 
  0x20, 0x32, 0x06, 0x00, 0x09, 0x00, 0x03, 0x00, 0x14, 0x02, 0x14, 0x01, 0x14, 0x03, 0x44, 0x50, 
  0x54, 0x14, 0x01, 0x86, 0x00, 0xec, 0xff, 0xff, 0xff, 0x08, 0x4f, 0x66, 0x66, 0x73, 0x65, 0x74, 
  0x20, 0x58, 0x3d, 0x04, 0x44, 0x50, 0x54, 0x14, 0x02, 0x87, 0x00, 0xe3, 0xce, 0xff, 0xff, 0x08, 
  0x4f, 0x66, 0x66, 0x73, 0x65, 0x74, 0x20, 0x59, 0x3d, 0x04, 0x44, 0x50, 0x54, 0x14, 0x03, 0x43, 
  0x00, 0xee, 0x08, 0x00, 0x00, 0x05, 0x57, 0x69, 0x64, 0x74, 0x68, 0x3d, 0x04, 0x44, 0x45, 0x54, 
  0x15, 0x00, 0x04, 0x09, 0x53, 0x65, 0x63, 0x74, 0x69, 0x6f, 0x6e, 0x20, 0x33, 0x07, 0x00, 0x09, 
  0x00, 0x03, 0x00, 0x15, 0x02, 0x15, 0x01, 0x15, 0x03, 0x44, 0x50, 0x54, 0x15, 0x01, 0x86, 0x00, 
  0xec, 0xff, 0xff, 0xff, 0x08, 0x4f, 0x66, 0x66, 0x73, 0x65, 0x74, 0x20, 0x58, 0x3d, 0x04, 0x44, 
  0x50, 0x54, 0x15, 0x02, 0x87, 0x00, 0xd1, 0xd7, 0xff, 0xff, 0x08, 0x4f, 0x66, 0x66, 0x73, 0x65, 
  0x74, 0x20, 0x59, 0x3d, 0x04, 0x44, 0x50, 0x54, 0x15, 0x03, 0x43, 0x00, 0xee, 0x08, 0x00, 0x00, 
  0x05, 0x57, 0x69, 0x64, 0x74, 0x68, 0x3d, 0x04, 0x44, 0x45, 0x54, 0x16, 0x00, 0x04, 0x09, 0x53, 
  0x65, 0x63, 0x74, 0x69, 0x6f, 0x6e, 0x20, 0x34, 0x08, 0x00, 0x09, 0x00, 0x03, 0x00, 0x16, 0x02, 
  0x16, 0x01, 0x16, 0x03, 0x44, 0x50, 0x54, 0x16, 0x01, 0x86, 0x00, 0xec, 0xff, 0xff, 0xff, 0x08, 
  0x4f, 0x66, 0x66, 0x73, 0x65, 0x74, 0x20, 0x58, 0x3d, 0x04, 0x44, 0x50, 0x54, 0x16, 0x02, 0x87, 
  0x00, 0xbf, 0xe0, 0xff, 0xff, 0x08, 0x4f, 0x66, 0x66, 0x73, 0x65, 0x74, 0x20, 0x59, 0x3d, 0x04, 
  0x44, 0x50, 0x54, 0x16, 0x03, 0x43, 0x00, 0xee, 0x08, 0x00, 0x00, 0x05, 0x57, 0x69, 0x64, 0x74, 
  0x68, 0x3d, 0x04, 0x44, 0x45, 0x54, 0x17, 0x00, 0x04, 0x09, 0x53, 0x65, 0x63, 0x74, 0x69, 0x6f, 
  0x6e, 0x20, 0x35, 0x09, 0x00, 0x09, 0x00, 0x03, 0x00, 0x17, 0x02, 0x17, 0x01, 0x17, 0x03, 0x44, 
  0x50, 0x54, 0x17, 0x01, 0x86, 0x00, 0xec, 0xff, 0xff, 0xff, 0x08, 0x4f, 0x66, 0x66, 0x73, 0x65, 
  0x74, 0x20, 0x58, 0x3d, 0x04, 0x44, 0x50, 0x54, 0x17, 0x02, 0x87, 0x00, 0xad, 0xe9, 0xff, 0xff, 
  0x08, 0x4f, 0x66, 0x66, 0x73, 0x65, 0x74, 0x20, 0x59, 0x3d, 0x04, 0x44, 0x50, 0x54, 0x17, 0x03, 
  0x43, 0x00, 0xee, 0x08, 0x00, 0x00, 0x05, 0x57, 0x69, 0x64, 0x74, 0x68, 0x3d, 0x04, 0x44, 0x45, 
  0x54, 0x18, 0x00, 0x04, 0x09, 0x53, 0x65, 0x63, 0x74, 0x69, 0x6f, 0x6e, 0x20, 0x36, 0x0a, 0x00, 
  0x09, 0x00, 0x03, 0x00, 0x18, 0x02, 0x18, 0x01, 0x18, 0x03, 0x44, 0x50, 0x54, 0x18, 0x01, 0x86, 
  0x00, 0xec, 0xff, 0xff, 0xff, 0x08, 0x4f, 0x66, 0x66, 0x73, 0x65, 0x74, 0x20, 0x58, 0x3d, 0x04, 
  0x44, 0x50, 0x54, 0x18, 0x02, 0x87, 0x00, 0x9b, 0xf2, 0xff, 0xff, 0x08, 0x4f, 0x66, 0x66, 0x73, 
  0x65, 0x74, 0x20, 0x59, 0x3d, 0x04, 0x44, 0x50, 0x54, 0x18, 0x03, 0x43, 0x00, 0xee, 0x08, 0x00, 
  0x00, 0x05, 0x57, 0x69, 0x64, 0x74, 0x68, 0x3d, 0x04, 0x44, 0x45, 0x54, 0x19, 0x00, 0x04, 0x09, 
  0x53, 0x65, 0x63, 0x74, 0x69, 0x6f, 0x6e, 0x20, 0x37, 0x0b, 0x00, 0x09, 0x00, 0x03, 0x00, 0x19, 
  0x02, 0x19, 0x01, 0x19, 0x03, 0x44, 0x50, 0x54, 0x19, 0x01, 0x86, 0x00, 0xec, 0xff, 0xff, 0xff, 
  0x08, 0x4f, 0x66, 0x66, 0x73, 0x65, 0x74, 0x20, 0x58, 0x3d, 0x04, 0x44, 0x50, 0x54, 0x19, 0x02, 
  0x87, 0x00, 0x89, 0xfb, 0xff, 0xff, 0x08, 0x4f, 0x66, 0x66, 0x73, 0x65, 0x74, 0x20, 0x59, 0x3d, 
  0x04, 0x44, 0x50, 0x54, 0x19, 0x03, 0x43, 0x00, 0xee, 0x08, 0x00, 0x00, 0x05, 0x57, 0x69, 0x64, 
  0x74, 0x68, 0x3d, 0x04, 0x44, 0x45, 0x54, 0x1a, 0x00, 0x04, 0x09, 0x53, 0x65, 0x63, 0x74, 0x69, 
  0x6f, 0x6e, 0x20, 0x38, 0x0c, 0x00, 0x09, 0x00, 0x03, 0x00, 0x1a, 0x02, 0x1a, 0x01, 0x1a, 0x03, 
  0x44, 0x50, 0x54, 0x1a, 0x01, 0x86, 0x00, 0xec, 0xff, 0xff, 0xff, 0x08, 0x4f, 0x66, 0x66, 0x73, 
  0x65, 0x74, 0x20, 0x58, 0x3d, 0x04, 0x44, 0x50, 0x54, 0x1a, 0x02, 0x87, 0x00, 0x77, 0x04, 0x00, 
  0x00, 0x08, 0x4f, 0x66, 0x66, 0x73, 0x65, 0x74, 0x20, 0x59, 0x3d, 0x04, 0x44, 0x50, 0x54, 0x1a, 
  0x03, 0x43, 0x00, 0xee, 0x08, 0x00, 0x00, 0x05, 0x57, 0x69, 0x64, 0x74, 0x68, 0x3d, 0x04, 0x44, 
  0x45, 0x54, 0x1b, 0x00, 0x04, 0x09, 0x53, 0x65, 0x63, 0x74, 0x69, 0x6f, 0x6e, 0x20, 0x39, 0x0d, 
  0x00, 0x09, 0x00, 0x03, 0x00, 0x1b, 0x02, 0x1b, 0x01, 0x1b, 0x03, 0x44, 0x50, 0x54, 0x1b, 0x01, 
  0x86, 0x00, 0xec, 0xff, 0xff, 0xff, 0x08, 0x4f, 0x66, 0x66, 0x73, 0x65, 0x74, 0x20, 0x58, 0x3d, 
  0x04, 0x44, 0x50, 0x54, 0x1b, 0x02, 0x87, 0x00, 0x65, 0x0d, 0x00, 0x00, 0x08, 0x4f, 0x66, 0x66, 
  0x73, 0x65, 0x74, 0x20, 0x59, 0x3d, 0x04, 0x44, 0x50, 0x54, 0x1b, 0x03, 0x43, 0x00, 0xee, 0x08, 
  0x00, 0x00, 0x05, 0x57, 0x69, 0x64, 0x74, 0x68, 0x3d, 0x04, 0x44, 0x45, 0x54, 0x1c, 0x00, 0x04, 
  0x0a, 0x53, 0x65, 0x63, 0x74, 0x69, 0x6f, 0x6e, 0x20, 0x31, 0x30, 0x0e, 0x00, 0x09, 0x00, 0x03, 
  0x00, 0x1c, 0x02, 0x1c, 0x01, 0x1c, 0x03, 0x44, 0x50, 0x54, 0x1c, 0x01, 0x86, 0x00, 0xec, 0xff, 
  0xff, 0xff, 0x08, 0x4f, 0x66, 0x66, 0x73, 0x65, 0x74, 0x20, 0x58, 0x3d, 0x04, 0x44, 0x50, 0x54, 
  0x1c, 0x02, 0x87, 0x00, 0x53, 0x16, 0x00, 0x00, 0x08, 0x4f, 0x66, 0x66, 0x73, 0x65, 0x74, 0x20, 
  0x59, 0x3d, 0x04, 0x44, 0x50, 0x54, 0x1c, 0x03, 0x43, 0x00, 0xee, 0x08, 0x00, 0x00, 0x05, 0x57, 
  0x69, 0x64, 0x74, 0x68, 0x3d, 0x04, 0x44, 0x45, 0x54, 0x1d, 0x00, 0x04, 0x0a, 0x53, 0x65, 0x63, 
  0x74, 0x69, 0x6f, 0x6e, 0x20, 0x31, 0x31, 0x0f, 0x00, 0x09, 0x00, 0x03, 0x00, 0x1d, 0x02, 0x1d, 
  0x01, 0x1d, 0x03, 0x44, 0x50, 0x54, 0x1d, 0x01, 0x86, 0x00, 0xec, 0xff, 0xff, 0xff, 0x08, 0x4f, 
  0x66, 0x66, 0x73, 0x65, 0x74, 0x20, 0x58, 0x3d, 0x04, 0x44, 0x50, 0x54, 0x1d, 0x02, 0x87, 0x00, 
  0x41, 0x1f, 0x00, 0x00, 0x08, 0x4f, 0x66, 0x66, 0x73, 0x65, 0x74, 0x20, 0x59, 0x3d, 0x04, 0x44, 
  0x50, 0x54, 0x1d, 0x03, 0x43, 0x00, 0xee, 0x08, 0x00, 0x00, 0x05, 0x57, 0x69, 0x64, 0x74, 0x68, 
  0x3d, 0x04, 0x44, 0x45, 0x54, 0x1e, 0x00, 0x04, 0x0a, 0x53, 0x65, 0x63, 0x74, 0x69, 0x6f, 0x6e, 
  0x20, 0x31, 0x32, 0x10, 0x00, 0x09, 0x00, 0x03, 0x00, 0x1e, 0x02, 0x1e, 0x01, 0x1e, 0x03, 0x44, 
  0x50, 0x54, 0x1e, 0x01, 0x86, 0x00, 0xec, 0xff, 0xff, 0xff, 0x08, 0x4f, 0x66, 0x66, 0x73, 0x65, 
  0x74, 0x20, 0x58, 0x3d, 0x04, 0x44, 0x50, 0x54, 0x1e, 0x02, 0x87, 0x00, 0x2f, 0x28, 0x00, 0x00, 
  0x08, 0x4f, 0x66, 0x66, 0x73, 0x65, 0x74, 0x20, 0x59, 0x3d, 0x04, 0x44, 0x50, 0x54, 0x1e, 0x03, 
  0x43, 0x00, 0xee, 0x08, 0x00, 0x00, 0x05, 0x57, 0x69, 0x64, 0x74, 0x68, 0x3d, 0x04, 0x44, 0x45, 
  0x54, 0x1f, 0x00, 0x04, 0x0a, 0x53, 0x65, 0x63, 0x74, 0x69, 0x6f, 0x6e, 0x20, 0x31, 0x33, 0x11, 
  0x00, 0x09, 0x00, 0x03, 0x00, 0x1f, 0x02, 0x1f, 0x01, 0x1f, 0x03, 0x44, 0x50, 0x54, 0x1f, 0x01, 
  0x86, 0x00, 0xec, 0xff, 0xff, 0xff, 0x08, 0x4f, 0x66, 0x66, 0x73, 0x65, 0x74, 0x20, 0x58, 0x3d, 
  0x04, 0x44, 0x50, 0x54, 0x1f, 0x02, 0x87, 0x00, 0x1d, 0x31, 0x00, 0x00, 0x08, 0x4f, 0x66, 0x66, 
  0x73, 0x65, 0x74, 0x20, 0x59, 0x3d, 0x04, 0x44, 0x50, 0x54, 0x1f, 0x03, 0x43, 0x00, 0xee, 0x08, 
  0x00, 0x00, 0x05, 0x57, 0x69, 0x64, 0x74, 0x68, 0x3d, 0x04, 0x44, 0x45, 0x54, 0x20, 0x00, 0x04, 
  0x0a, 0x53, 0x65, 0x63, 0x74, 0x69, 0x6f, 0x6e, 0x20, 0x31, 0x34, 0x12, 0x00, 0x09, 0x00, 0x03, 
  0x00, 0x20, 0x02, 0x20, 0x01, 0x20, 0x03, 0x44, 0x50, 0x54, 0x20, 0x01, 0x86, 0x00, 0xec, 0xff, 
  0xff, 0xff, 0x08, 0x4f, 0x66, 0x66, 0x73, 0x65, 0x74, 0x20, 0x58, 0x3d, 0x04, 0x44, 0x50, 0x54, 
  0x20, 0x02, 0x87, 0x00, 0x0b, 0x3a, 0x00, 0x00, 0x08, 0x4f, 0x66, 0x66, 0x73, 0x65, 0x74, 0x20, 
  0x59, 0x3d, 0x04, 0x44, 0x50, 0x54, 0x20, 0x03, 0x43, 0x00, 0xee, 0x08, 0x00, 0x00, 0x05, 0x57, 
  0x69, 0x64, 0x74, 0x68, 0x3d, 0x04, 0x44, 0x45, 0x54, 0x21, 0x00, 0x04, 0x0a, 0x53, 0x65, 0x63, 
  0x74, 0x69, 0x6f, 0x6e, 0x20, 0x31, 0x35, 0x13, 0x00, 0x09, 0x00, 0x03, 0x00, 0x21, 0x02, 0x21, 
  0x01, 0x21, 0x03, 0x44, 0x50, 0x54, 0x21, 0x01, 0x86, 0x00, 0xec, 0xff, 0xff, 0xff, 0x08, 0x4f, 
  0x66, 0x66, 0x73, 0x65, 0x74, 0x20, 0x58, 0x3d, 0x04, 0x44, 0x50, 0x54, 0x21, 0x02, 0x87, 0x00, 
  0xf9, 0x42, 0x00, 0x00, 0x08, 0x4f, 0x66, 0x66, 0x73, 0x65, 0x74, 0x20, 0x59, 0x3d, 0x04, 0x44, 
  0x50, 0x54, 0x21, 0x03, 0x43, 0x00, 0xee, 0x08, 0x00, 0x00, 0x05, 0x57, 0x69, 0x64, 0x74, 0x68, 
  0x3d, 0x04, 0x44, 0x50, 0x44, 0x12, 0x04, 0xa1, 0x00, 0x01, 0x08, 0x16, 0x41, 0x63, 0x74, 0x75, 
  0x61, 0x6c, 0x20, 0x57, 0x6f, 0x72, 0x6b, 0x20, 0x53, 0x74, 0x61, 0x74, 0x65, 0x20, 0x31, 0x2d, 
  0x31, 0x36, 0xff, 0xff, 0x44, 0x50, 0x44, 0x22, 0x04, 0x22, 0x01, 0x03, 0x08, 0x18, 0x53, 0x65, 
  0x74, 0x70, 0x6f, 0x69, 0x6e, 0x74, 0x20, 0x57, 0x6f, 0x72, 0x6b, 0x20, 0x53, 0x74, 0x61, 0x74, 
  0x65, 0x20, 0x31, 0x2d, 0x31, 0x36, 0xff, 0xff, 0x44, 0x56, 0x50, 0x3c, 0x04, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x80, 0x3f, 0x00, 0x02, 0x6d, 0x6d, 0x44, 0x56, 0x50, 0x3d, 0x04, 0x00, 0x00, 
  0x00, 0x00, 0x6f, 0x12, 0x83, 0x3a, 0x00, 0x01, 0x6d, 0x44, 0x56, 0x50, 0x3a, 0x04, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x80, 0x3f, 0x00, 0x03, 0x6d, 0x5e, 0x32, 0x44, 0x56, 0x50, 0x3e, 0x04, 
  0x00, 0x00, 0x00, 0x00, 0x6f, 0x12, 0x83, 0x3a, 0x00, 0x01, 0x4c, 0x44, 0x56, 0x50, 0x3b, 0x04, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x3f, 0x01, 0x07, 0x6d, 0x69, 0x6e, 0x75, 0x74, 0x65, 
  0x73, 0x44, 0x56, 0x50, 0x3f, 0x04, 0x00, 0x00, 0x00, 0x00, 0x6f, 0x12, 0x83, 0x3a, 0x01, 0x04, 
  0x4c, 0x2f, 0x68, 0x61
};

// clang-format on

TEST(TASK_CONTROLLER_CLIENT_TESTS, MessageEncoding)
{
	VirtualCANPlugin serverTC;
	serverTC.open();
	auto blankDDOP = std::make_shared<DeviceDescriptorObjectPool>();

	CANHardwareInterface::set_number_of_can_channels(1);
	CANHardwareInterface::assign_can_channel_frame_handler(0, std::make_shared<VirtualCANPlugin>());
	CANHardwareInterface::start();

	NAME clientNAME(0);
	clientNAME.set_industry_group(2);
	clientNAME.set_function_code(static_cast<std::uint8_t>(NAME::Function::RateControl));
	auto internalECU = test_helpers::claim_internal_control_function(0x84, 0);

	CANMessageFrame testFrame;

	std::uint32_t waitingTimestamp_ms = SystemTiming::get_timestamp_ms();

	while ((!internalECU->get_address_valid()) &&
	       (!SystemTiming::time_expired_ms(waitingTimestamp_ms, 2000)))
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(50));
	}

	ASSERT_TRUE(internalECU->get_address_valid());

	std::vector<isobus::NAMEFilter> vtNameFilters;
	const isobus::NAMEFilter testFilter(isobus::NAME::NAMEParameters::FunctionCode, static_cast<std::uint8_t>(isobus::NAME::Function::TaskController));
	vtNameFilters.push_back(testFilter);

	CANNetworkManager::CANNetwork.update();

	auto tcPartner = test_helpers::force_claim_partnered_control_function(0xF7, 0);

	DerivedTestTCClient interfaceUnderTest(tcPartner, internalECU);

	EXPECT_EQ(tcPartner, interfaceUnderTest.get_partner_control_function());
	EXPECT_EQ(internalECU, interfaceUnderTest.get_internal_control_function());

	std::this_thread::sleep_for(std::chrono::milliseconds(50));

	// Get the virtual CAN plugin back to a known state
	while (!serverTC.get_queue_empty())
	{
		serverTC.read_frame(testFrame);
	}
	ASSERT_TRUE(serverTC.get_queue_empty());
	ASSERT_TRUE(tcPartner->get_address_valid());

	// Test Working Set Master Message
	ASSERT_TRUE(interfaceUnderTest.test_wrapper_send_working_set_master());

	ASSERT_TRUE(serverTC.read_frame(testFrame));

	ASSERT_TRUE(testFrame.isExtendedFrame);
	ASSERT_EQ(testFrame.dataLength, 8);
	EXPECT_EQ(CANIdentifier(testFrame.identifier).get_parameter_group_number(), 0xFE0D);
	EXPECT_EQ(testFrame.data[0], 1); // 1 Working set member by default

	for (std::uint_fast8_t i = 1; i < 8; i++)
	{
		// Check Reserved Bytes
		ASSERT_EQ(testFrame.data[i], 0xFF);
	}

	// Test Version Request Message
	ASSERT_TRUE(interfaceUnderTest.test_wrapper_send_version_request());

	ASSERT_TRUE(serverTC.read_frame(testFrame));

	ASSERT_TRUE(testFrame.isExtendedFrame);
	ASSERT_EQ(testFrame.dataLength, 8);
	EXPECT_EQ(CANIdentifier(testFrame.identifier).get_parameter_group_number(), 0xCB00);
	EXPECT_EQ(0x00, testFrame.data[0]);

	for (std::uint_fast8_t i = 1; i < 8; i++)
	{
		// Check Reserved Bytes
		ASSERT_EQ(testFrame.data[i], 0xFF);
	}

	// Test status message
	interfaceUnderTest.test_wrapper_set_state(TaskControllerClient::StateMachineState::SendStatusMessage);
	ASSERT_EQ(interfaceUnderTest.test_wrapper_get_state(), TaskControllerClient::StateMachineState::SendStatusMessage);
	interfaceUnderTest.update();

	serverTC.read_frame(testFrame);

	ASSERT_TRUE(testFrame.isExtendedFrame);
	ASSERT_EQ(testFrame.dataLength, 8);
	EXPECT_EQ(CANIdentifier(testFrame.identifier).get_parameter_group_number(), 0xCB00);
	EXPECT_EQ(0xFF, testFrame.data[0]); // Mux
	EXPECT_EQ(0xFF, testFrame.data[1]); // Element number
	EXPECT_EQ(0xFF, testFrame.data[2]); // DDI
	EXPECT_EQ(0xFF, testFrame.data[3]); // DDI
	EXPECT_EQ(0x00, testFrame.data[4]); // Status
	EXPECT_EQ(0x00, testFrame.data[5]); // 0 Reserved
	EXPECT_EQ(0x00, testFrame.data[6]); // 0 Reserved
	EXPECT_EQ(0x00, testFrame.data[7]); // 0 Reserved

	// Test version response
	ASSERT_TRUE(interfaceUnderTest.test_wrapper_send_request_version_response());
	serverTC.read_frame(testFrame);
	ASSERT_TRUE(testFrame.isExtendedFrame);
	ASSERT_EQ(testFrame.dataLength, 8);
	EXPECT_EQ(CANIdentifier(testFrame.identifier).get_parameter_group_number(), 0xCB00);
	EXPECT_EQ(0x10, testFrame.data[0]); // Mux
	EXPECT_EQ(0x04, testFrame.data[1]); // Version (4 - Second Edition)
	EXPECT_EQ(0xFF, testFrame.data[2]); // Must be 0xFF
	EXPECT_EQ(0x00, testFrame.data[3]); // Options
	EXPECT_EQ(0x00, testFrame.data[4]); // Must be zero
	EXPECT_EQ(0x00, testFrame.data[5]); // Booms
	EXPECT_EQ(0x00, testFrame.data[6]); // Sections
	EXPECT_EQ(0x00, testFrame.data[7]); // Channels

	interfaceUnderTest.test_wrapper_set_state(TaskControllerClient::StateMachineState::Disconnected);
	interfaceUnderTest.configure(blankDDOP, 1, 2, 3, true, true, true, true, true);
	ASSERT_TRUE(interfaceUnderTest.test_wrapper_send_request_version_response());
	serverTC.read_frame(testFrame);

	ASSERT_TRUE(testFrame.isExtendedFrame);
	ASSERT_EQ(testFrame.dataLength, 8);
	EXPECT_EQ(CANIdentifier(testFrame.identifier).get_parameter_group_number(), 0xCB00);
	EXPECT_EQ(0x10, testFrame.data[0]); // Mux
	EXPECT_EQ(0x04, testFrame.data[1]); // Version (4 - Second Edition)
	EXPECT_EQ(0xFF, testFrame.data[2]); // Must be 0xFF
	EXPECT_EQ(0x1F, testFrame.data[3]); // Options
	EXPECT_EQ(0x00, testFrame.data[4]); // Must be zero
	EXPECT_EQ(0x01, testFrame.data[5]); // Booms
	EXPECT_EQ(0x02, testFrame.data[6]); // Sections
	EXPECT_EQ(0x03, testFrame.data[7]); // Channels

	// Test Request structure label
	ASSERT_TRUE(interfaceUnderTest.test_wrapper_send_request_structure_label());
	serverTC.read_frame(testFrame);
	ASSERT_TRUE(testFrame.isExtendedFrame);
	ASSERT_EQ(testFrame.dataLength, 8);
	EXPECT_EQ(CANIdentifier(testFrame.identifier).get_parameter_group_number(), 0xCB00);
	EXPECT_EQ(0x01, testFrame.data[0]);
	for (std::uint_fast8_t i = 1; i < 7; i++)
	{
		EXPECT_EQ(0xFF, testFrame.data[i]);
	}

	// Test Request localization label
	ASSERT_TRUE(interfaceUnderTest.test_wrapper_send_request_localization_label());
	serverTC.read_frame(testFrame);
	ASSERT_TRUE(testFrame.isExtendedFrame);
	ASSERT_EQ(testFrame.dataLength, 8);
	EXPECT_EQ(CANIdentifier(testFrame.identifier).get_parameter_group_number(), 0xCB00);
	EXPECT_EQ(0x21, testFrame.data[0]);
	for (std::uint_fast8_t i = 1; i < 7; i++)
	{
		EXPECT_EQ(0xFF, testFrame.data[i]);
	}

	// Test Delete Object Pool
	ASSERT_TRUE(interfaceUnderTest.test_wrapper_send_delete_object_pool());
	serverTC.read_frame(testFrame);
	ASSERT_TRUE(testFrame.isExtendedFrame);
	ASSERT_EQ(testFrame.dataLength, 8);
	EXPECT_EQ(CANIdentifier(testFrame.identifier).get_parameter_group_number(), 0xCB00);
	EXPECT_EQ(0xA1, testFrame.data[0]);
	for (std::uint_fast8_t i = 1; i < 7; i++)
	{
		EXPECT_EQ(0xFF, testFrame.data[i]);
	}

	// Test PDACK
	ASSERT_TRUE(interfaceUnderTest.test_wrapper_send_pdack(47, 29));
	serverTC.read_frame(testFrame);
	ASSERT_TRUE(testFrame.isExtendedFrame);
	ASSERT_EQ(testFrame.dataLength, 8);
	EXPECT_EQ(CANIdentifier(testFrame.identifier).get_parameter_group_number(), 0xCB00);
	EXPECT_EQ(0xFD, testFrame.data[0]);
	EXPECT_EQ(0x02, testFrame.data[1]);
	EXPECT_EQ(0x1D, testFrame.data[2]);
	EXPECT_EQ(0x00, testFrame.data[3]);

	// Test Value Command
	ASSERT_TRUE(interfaceUnderTest.test_wrapper_send_value_command(1234, 567, 8910));
	serverTC.read_frame(testFrame);
	ASSERT_TRUE(testFrame.isExtendedFrame);
	ASSERT_EQ(testFrame.dataLength, 8);
	EXPECT_EQ(CANIdentifier(testFrame.identifier).get_parameter_group_number(), 0xCB00);
	EXPECT_EQ(0x23, testFrame.data[0]);
	EXPECT_EQ(0x4D, testFrame.data[1]);
	EXPECT_EQ(0x37, testFrame.data[2]);
	EXPECT_EQ(0x02, testFrame.data[3]);
	EXPECT_EQ(0xCE, testFrame.data[4]);
	EXPECT_EQ(0x22, testFrame.data[5]);
	EXPECT_EQ(0x00, testFrame.data[6]);
	EXPECT_EQ(0x00, testFrame.data[7]);

	// Test identify TC
	ASSERT_TRUE(interfaceUnderTest.test_wrapper_request_task_controller_identification());
	serverTC.read_frame(testFrame);
	ASSERT_TRUE(testFrame.isExtendedFrame);
	ASSERT_EQ(testFrame.dataLength, 8);
	EXPECT_EQ(CANIdentifier(testFrame.identifier).get_parameter_group_number(), 0xCB00);
	EXPECT_EQ(0x20, testFrame.data[0]);
	EXPECT_EQ(0xFF, testFrame.data[1]);
	EXPECT_EQ(0xFF, testFrame.data[2]);
	EXPECT_EQ(0xFF, testFrame.data[3]);
	EXPECT_EQ(0xFF, testFrame.data[4]);
	EXPECT_EQ(0xFF, testFrame.data[5]);
	EXPECT_EQ(0xFF, testFrame.data[6]);
	EXPECT_EQ(0xFF, testFrame.data[7]);

	CANHardwareInterface::stop();
	CANHardwareInterface::set_number_of_can_channels(0);

	CANNetworkManager::CANNetwork.deactivate_control_function(tcPartner);
	CANNetworkManager::CANNetwork.deactivate_control_function(internalECU);
}

TEST(TASK_CONTROLLER_CLIENT_TESTS, BadPartnerDeathTest)
{
	NAME clientNAME(0);
	clientNAME.set_industry_group(2);
	clientNAME.set_function_code(static_cast<std::uint8_t>(NAME::Function::RateControl));
	auto internalECU = CANNetworkManager::CANNetwork.create_internal_control_function(clientNAME, 0, 0x81);
	DerivedTestTCClient interfaceUnderTest(nullptr, internalECU);
	ASSERT_FALSE(interfaceUnderTest.get_is_initialized());
	EXPECT_DEATH(interfaceUnderTest.initialize(false), "");

	CANNetworkManager::CANNetwork.deactivate_control_function(internalECU);
}

TEST(TASK_CONTROLLER_CLIENT_TESTS, BadICFDeathTest)
{
	std::vector<isobus::NAMEFilter> vtNameFilters;
	const isobus::NAMEFilter testFilter(isobus::NAME::NAMEParameters::FunctionCode, static_cast<std::uint8_t>(isobus::NAME::Function::TaskController));
	vtNameFilters.push_back(testFilter);

	auto tcPartner = CANNetworkManager::CANNetwork.create_partnered_control_function(0, vtNameFilters);
	DerivedTestTCClient interfaceUnderTest(tcPartner, nullptr);
	ASSERT_FALSE(interfaceUnderTest.get_is_initialized());
	EXPECT_DEATH(interfaceUnderTest.initialize(false), "");
	CANNetworkManager::CANNetwork.deactivate_control_function(tcPartner);
}

TEST(TASK_CONTROLLER_CLIENT_TESTS, BadBinaryPointerDDOPDeathTest)
{
	DerivedTestTCClient interfaceUnderTest(nullptr, nullptr);
	EXPECT_DEATH(interfaceUnderTest.configure(nullptr, 0, 6, 64, 32, false, false, false, false, false), "");
}

TEST(TASK_CONTROLLER_CLIENT_TESTS, BadBinaryPointerDDOPSizeDeathTest)
{
	DerivedTestTCClient interfaceUnderTest(nullptr, nullptr);
	EXPECT_DEATH(interfaceUnderTest.configure(DerivedTestTCClient::testBinaryDDOP, 0, 6, 64, 32, false, false, false, false, false), "");
}

TEST(TASK_CONTROLLER_CLIENT_TESTS, BadBinaryVectorDDOPSDeathTest)
{
	DerivedTestTCClient interfaceUnderTest(nullptr, nullptr);
	EXPECT_DEATH(interfaceUnderTest.configure(std::shared_ptr<std::vector<std::uint8_t>>(), 6, 64, 32, false, false, false, false, false), "");
}

TEST(TASK_CONTROLLER_CLIENT_TESTS, StateMachineTests)
{
	// Boilerplate...
	VirtualCANPlugin serverTC;
	serverTC.open();

	CANHardwareInterface::set_number_of_can_channels(1);
	CANHardwareInterface::assign_can_channel_frame_handler(0, std::make_shared<VirtualCANPlugin>());
	CANHardwareInterface::start();

	auto internalECU = test_helpers::claim_internal_control_function(0x83, 0);
	auto tcPartner = test_helpers::force_claim_partnered_control_function(0xF7, 0);

	DerivedTestTCClient interfaceUnderTest(tcPartner, internalECU);
	interfaceUnderTest.initialize(false);

	std::this_thread::sleep_for(std::chrono::milliseconds(50));

	// Get the virtual CAN plugin back to a known state
	CANMessageFrame testFrame = {};
	while (!serverTC.get_queue_empty())
	{
		serverTC.read_frame(testFrame);
	}
	ASSERT_TRUE(serverTC.get_queue_empty());
	ASSERT_TRUE(tcPartner->get_address_valid());

	// End boilerplate

	// Check initial state
	EXPECT_EQ(interfaceUnderTest.test_wrapper_get_state(), TaskControllerClient::StateMachineState::Disconnected);
	interfaceUnderTest.update();
	EXPECT_EQ(interfaceUnderTest.test_wrapper_get_state(), TaskControllerClient::StateMachineState::Disconnected);

	// Check Transition out of status message wait state
	interfaceUnderTest.test_wrapper_set_state(TaskControllerClient::StateMachineState::WaitForServerStatusMessage);
	EXPECT_EQ(interfaceUnderTest.test_wrapper_get_state(), TaskControllerClient::StateMachineState::WaitForServerStatusMessage);

	// Send a status message and confirm we move on to the next state.
	testFrame.identifier = 0x18CBFFF7;
	testFrame.data[0] = 0xFE; // Status mux
	testFrame.data[1] = 0xFF; // Element number, set to not available
	testFrame.data[2] = 0xFF; // DDI (N/A)
	testFrame.data[3] = 0xFF; // DDI (N/A)
	testFrame.data[4] = 0x01; // Status (task active)
	testFrame.data[5] = 0x00; // Command address
	testFrame.data[6] = 0x00; // Command
	testFrame.data[7] = 0xFF; // Reserved
	CANNetworkManager::CANNetwork.process_receive_can_message_frame(testFrame);
	CANNetworkManager::CANNetwork.update();

	EXPECT_EQ(interfaceUnderTest.test_wrapper_get_state(), TaskControllerClient::StateMachineState::SendWorkingSetMaster);

	// Test Send Working Set Master State
	interfaceUnderTest.update();
	EXPECT_EQ(interfaceUnderTest.test_wrapper_get_state(), TaskControllerClient::StateMachineState::SendStatusMessage);

	// Test Request Language state
	interfaceUnderTest.test_wrapper_set_state(TaskControllerClient::StateMachineState::RequestLanguage);
	interfaceUnderTest.update();

	EXPECT_EQ(interfaceUnderTest.test_wrapper_get_state(), TaskControllerClient::StateMachineState::WaitForLanguageResponse);
	interfaceUnderTest.test_wrapper_set_state(TaskControllerClient::StateMachineState::WaitForLanguageResponse, 0);

	// Test wait for language response state
	testFrame.identifier = 0x18FE0FF7;
	testFrame.data[0] = 'e';
	testFrame.data[1] = 'n',
	testFrame.data[2] = 0x0F;
	testFrame.data[3] = 0x04;
	testFrame.data[4] = 0x5A;
	testFrame.data[5] = 0x04;
	testFrame.data[6] = 0xFF;
	testFrame.data[7] = 0xFF;
	CANNetworkManager::CANNetwork.process_receive_can_message_frame(testFrame);
	CANNetworkManager::CANNetwork.update();
	interfaceUnderTest.update();
	EXPECT_EQ(interfaceUnderTest.test_wrapper_get_state(), TaskControllerClient::StateMachineState::ProcessDDOP);

	// Test Version Response State
	interfaceUnderTest.test_wrapper_set_state(TaskControllerClient::StateMachineState::WaitForRequestVersionResponse);
	interfaceUnderTest.update();

	EXPECT_EQ(interfaceUnderTest.test_wrapper_get_state(), TaskControllerClient::StateMachineState::WaitForRequestVersionResponse);

	// Send the version response to the client as the TC server
	// Send a status message and confirm we move on to the next state.
	testFrame.identifier = 0x18CB83F7;
	testFrame.data[0] = 0x10; // Mux
	testFrame.data[1] = 0x04; // Version number (Version 4)
	testFrame.data[2] = 0xFF; // Max boot time (Not available)
	testFrame.data[3] = 0x1F; // Supports all options
	testFrame.data[4] = 0x00; // Reserved options = 0
	testFrame.data[5] = 0x01; // Number of booms for section control (1)
	testFrame.data[6] = 0x20; // Number of sections for section control (32)
	testFrame.data[7] = 0x10; // Number channels for position based control (16)
	CANNetworkManager::CANNetwork.process_receive_can_message_frame(testFrame);

	CANNetworkManager::CANNetwork.update();

	// Test the values parsed in this state machine state
	EXPECT_EQ(TaskControllerClient::StateMachineState::WaitForRequestVersionFromServer, interfaceUnderTest.test_wrapper_get_state());
	EXPECT_EQ(TaskControllerClient::Version::SecondPublishedEdition, interfaceUnderTest.get_connected_tc_version());
	EXPECT_EQ(0xFF, interfaceUnderTest.get_connected_tc_max_boot_time());
	EXPECT_EQ(true, interfaceUnderTest.get_connected_tc_option_supported(TaskControllerClient::ServerOptions::SupportsDocumentation));
	EXPECT_EQ(true, interfaceUnderTest.get_connected_tc_option_supported(TaskControllerClient::ServerOptions::SupportsImplementSectionControlFunctionality));
	EXPECT_EQ(true, interfaceUnderTest.get_connected_tc_option_supported(TaskControllerClient::ServerOptions::SupportsPeerControlAssignment));
	EXPECT_EQ(true, interfaceUnderTest.get_connected_tc_option_supported(TaskControllerClient::ServerOptions::SupportsTCGEOWithPositionBasedControl));
	EXPECT_EQ(true, interfaceUnderTest.get_connected_tc_option_supported(TaskControllerClient::ServerOptions::SupportsTCGEOWithoutPositionBasedControl));
	EXPECT_EQ(false, interfaceUnderTest.get_connected_tc_option_supported(TaskControllerClient::ServerOptions::ReservedOption1));
	EXPECT_EQ(false, interfaceUnderTest.get_connected_tc_option_supported(TaskControllerClient::ServerOptions::ReservedOption2));
	EXPECT_EQ(false, interfaceUnderTest.get_connected_tc_option_supported(TaskControllerClient::ServerOptions::ReservedOption3));
	EXPECT_EQ(1, interfaceUnderTest.get_connected_tc_number_booms_supported());
	EXPECT_EQ(32, interfaceUnderTest.get_connected_tc_number_sections_supported());
	EXPECT_EQ(16, interfaceUnderTest.get_connected_tc_number_channels_supported());

	// Test Status Message State
	interfaceUnderTest.test_wrapper_set_state(TaskControllerClient::StateMachineState::SendStatusMessage);
	EXPECT_EQ(interfaceUnderTest.test_wrapper_get_state(), TaskControllerClient::StateMachineState::SendStatusMessage);
	interfaceUnderTest.update();
	EXPECT_EQ(interfaceUnderTest.test_wrapper_get_state(), TaskControllerClient::StateMachineState::RequestVersion);

	// Test transition to disconnect from NACK
	// Send a NACK
	testFrame.identifier = 0x18E883F7;
	testFrame.data[0] = 0x01; // N-ACK
	testFrame.data[1] = 0xFF;
	testFrame.data[2] = 0xFF;
	testFrame.data[3] = 0xFF;
	testFrame.data[4] = 0x83; // Address
	testFrame.data[5] = 0x00; // PGN
	testFrame.data[6] = 0xCB; // PGN
	testFrame.data[7] = 0x00; // PGN
	CANNetworkManager::CANNetwork.process_receive_can_message_frame(testFrame);
	CANNetworkManager::CANNetwork.update();
	EXPECT_EQ(interfaceUnderTest.test_wrapper_get_state(), TaskControllerClient::StateMachineState::Disconnected);

	// Test send structure request state
	interfaceUnderTest.test_wrapper_set_state(TaskControllerClient::StateMachineState::RequestStructureLabel);
	EXPECT_EQ(interfaceUnderTest.test_wrapper_get_state(), TaskControllerClient::StateMachineState::RequestStructureLabel);
	interfaceUnderTest.update();
	EXPECT_EQ(interfaceUnderTest.test_wrapper_get_state(), TaskControllerClient::StateMachineState::WaitForStructureLabelResponse);

	// Test send localization request state
	interfaceUnderTest.test_wrapper_set_state(TaskControllerClient::StateMachineState::RequestLocalizationLabel);
	EXPECT_EQ(interfaceUnderTest.test_wrapper_get_state(), TaskControllerClient::StateMachineState::RequestLocalizationLabel);
	interfaceUnderTest.update();
	EXPECT_EQ(interfaceUnderTest.test_wrapper_get_state(), TaskControllerClient::StateMachineState::WaitForLocalizationLabelResponse);

	// Test send delete object pool states
	interfaceUnderTest.test_wrapper_set_state(TaskControllerClient::StateMachineState::SendDeleteObjectPool);
	EXPECT_EQ(interfaceUnderTest.test_wrapper_get_state(), TaskControllerClient::StateMachineState::SendDeleteObjectPool);
	interfaceUnderTest.update();
	EXPECT_EQ(interfaceUnderTest.test_wrapper_get_state(), TaskControllerClient::StateMachineState::WaitForDeleteObjectPoolResponse);
	// Send a response
	testFrame.identifier = 0x18CB83F7;
	testFrame.data[0] = 0xB1; // Mux
	testFrame.data[1] = 0xFF; // Ambigious
	testFrame.data[2] = 0xFF; // Ambigious
	testFrame.data[3] = 0xFF; // error details are not available
	testFrame.data[4] = 0xFF; // Reserved
	testFrame.data[5] = 0xFF; // Reserved
	testFrame.data[6] = 0xFF; // Reserved
	testFrame.data[7] = 0xFF; // Reserved
	CANNetworkManager::CANNetwork.process_receive_can_message_frame(testFrame);
	CANNetworkManager::CANNetwork.update();
	EXPECT_EQ(interfaceUnderTest.test_wrapper_get_state(), TaskControllerClient::StateMachineState::SendRequestTransferObjectPool);

	// Test send activate object pool state
	interfaceUnderTest.test_wrapper_set_state(TaskControllerClient::StateMachineState::SendObjectPoolActivate);
	EXPECT_EQ(interfaceUnderTest.test_wrapper_get_state(), TaskControllerClient::StateMachineState::SendObjectPoolActivate);
	interfaceUnderTest.update();
	EXPECT_EQ(interfaceUnderTest.test_wrapper_get_state(), TaskControllerClient::StateMachineState::WaitForObjectPoolActivateResponse);

	// Test send deactivate object pool state
	interfaceUnderTest.test_wrapper_set_state(TaskControllerClient::StateMachineState::DeactivateObjectPool);
	EXPECT_EQ(interfaceUnderTest.test_wrapper_get_state(), TaskControllerClient::StateMachineState::DeactivateObjectPool);
	interfaceUnderTest.update();
	EXPECT_EQ(interfaceUnderTest.test_wrapper_get_state(), TaskControllerClient::StateMachineState::WaitForObjectPoolDeactivateResponse);

	// Test task state when not connected
	EXPECT_EQ(false, interfaceUnderTest.get_is_task_active());

	// Test Connected State
	interfaceUnderTest.test_wrapper_set_state(TaskControllerClient::StateMachineState::Connected);
	EXPECT_EQ(interfaceUnderTest.test_wrapper_get_state(), TaskControllerClient::StateMachineState::Connected);
	EXPECT_EQ(true, interfaceUnderTest.get_is_connected());
	EXPECT_EQ(true, interfaceUnderTest.get_is_task_active());

	// Test WaitForRequestVersionFromServer State
	interfaceUnderTest.test_wrapper_set_state(TaskControllerClient::StateMachineState::WaitForRequestVersionFromServer);
	EXPECT_EQ(interfaceUnderTest.test_wrapper_get_state(), TaskControllerClient::StateMachineState::WaitForRequestVersionFromServer);
	// Send a request for version
	testFrame.identifier = 0x18CB83F7;
	testFrame.data[0] = 0x00; // Mux
	testFrame.data[1] = 0xFF; // Reserved
	testFrame.data[2] = 0xFF; // Reserved
	testFrame.data[3] = 0xFF; // Reserved
	testFrame.data[4] = 0xFF; // Reserved
	testFrame.data[5] = 0xFF; // Reserved
	testFrame.data[6] = 0xFF; // Reserved
	testFrame.data[7] = 0xFF; // Reserved
	CANNetworkManager::CANNetwork.process_receive_can_message_frame(testFrame);
	CANNetworkManager::CANNetwork.update();
	EXPECT_EQ(interfaceUnderTest.test_wrapper_get_state(), TaskControllerClient::StateMachineState::SendRequestVersionResponse);
	// Test strange technical command doesn't change the state
	interfaceUnderTest.test_wrapper_set_state(TaskControllerClient::StateMachineState::WaitForRequestVersionFromServer);
	EXPECT_EQ(interfaceUnderTest.test_wrapper_get_state(), TaskControllerClient::StateMachineState::WaitForRequestVersionFromServer);
	// Send a request for version
	testFrame.identifier = 0x18CB83F7;
	testFrame.data[0] = 0x40; // Mux
	testFrame.data[1] = 0xFF; // Reserved
	testFrame.data[2] = 0xFF; // Reserved
	testFrame.data[3] = 0xFF; // Reserved
	testFrame.data[4] = 0xFF; // Reserved
	testFrame.data[5] = 0xFF; // Reserved
	testFrame.data[6] = 0xFF; // Reserved
	testFrame.data[7] = 0xFF; // Reserved
	CANNetworkManager::CANNetwork.process_receive_can_message_frame(testFrame);
	CANNetworkManager::CANNetwork.update();
	EXPECT_EQ(interfaceUnderTest.test_wrapper_get_state(), TaskControllerClient::StateMachineState::WaitForRequestVersionFromServer);

	// Test WaitForStructureLabelResponse State
	interfaceUnderTest.test_wrapper_set_state(TaskControllerClient::StateMachineState::WaitForStructureLabelResponse);
	EXPECT_EQ(interfaceUnderTest.test_wrapper_get_state(), TaskControllerClient::StateMachineState::WaitForStructureLabelResponse);
	// Send a request for version
	testFrame.identifier = 0x18CB83F7;
	testFrame.data[0] = 0x11; // Mux
	testFrame.data[1] = 0xFF; // No Label
	testFrame.data[2] = 0xFF; // No Label
	testFrame.data[3] = 0xFF; // No Label
	testFrame.data[4] = 0xFF; // No Label
	testFrame.data[5] = 0xFF; // No Label
	testFrame.data[6] = 0xFF; // No Label
	testFrame.data[7] = 0xFF; // No Label
	CANNetworkManager::CANNetwork.process_receive_can_message_frame(testFrame);
	CANNetworkManager::CANNetwork.update();
	EXPECT_EQ(interfaceUnderTest.test_wrapper_get_state(), TaskControllerClient::StateMachineState::SendRequestTransferObjectPool);

	// Test generating a null DDOP
	interfaceUnderTest.test_wrapper_set_state(TaskControllerClient::StateMachineState::ProcessDDOP);
	EXPECT_EQ(interfaceUnderTest.test_wrapper_get_state(), TaskControllerClient::StateMachineState::ProcessDDOP);
	EXPECT_DEATH(interfaceUnderTest.update(), "");

	// Need a DDOP to test some states...
	auto testDDOP = std::make_shared<DeviceDescriptorObjectPool>();
	ASSERT_NE(nullptr, testDDOP);

	// Make a test pool, don't care about our ISO NAME, Localization label, or extended structure label for this test
	// Set up device
	interfaceUnderTest.test_wrapper_set_state(TaskControllerClient::StateMachineState::Disconnected);
	ASSERT_EQ(true, testDDOP->add_device("AgIsoStack++ UnitTest", "1.0.0", "123", "I++1.0", { 0x01 }, std::vector<std::uint8_t>(), 0));
	interfaceUnderTest.configure(testDDOP, 6, 64, 32, false, false, false, false, false);

	// Now try it with a valid structure label
	interfaceUnderTest.test_wrapper_set_state(TaskControllerClient::StateMachineState::WaitForStructureLabelResponse);
	EXPECT_EQ(interfaceUnderTest.test_wrapper_get_state(), TaskControllerClient::StateMachineState::WaitForStructureLabelResponse);
	// Send a structure label
	testFrame.identifier = 0x18CB83F7;
	testFrame.data[0] = 0x11; // Mux
	testFrame.data[1] = 0x04; // A valid label technically
	testFrame.data[2] = 0xFF;
	testFrame.data[3] = 0xFF;
	testFrame.data[4] = 0xFF;
	testFrame.data[5] = 0xFF;
	testFrame.data[6] = 0xFF;
	testFrame.data[7] = 0xFF;
	CANNetworkManager::CANNetwork.process_receive_can_message_frame(testFrame);
	CANNetworkManager::CANNetwork.update();
	EXPECT_EQ(interfaceUnderTest.test_wrapper_get_state(), TaskControllerClient::StateMachineState::SendDeleteObjectPool);

	// Now try it with a matching structure label
	interfaceUnderTest.test_wrapper_set_state(TaskControllerClient::StateMachineState::WaitForStructureLabelResponse);
	EXPECT_EQ(interfaceUnderTest.test_wrapper_get_state(), TaskControllerClient::StateMachineState::WaitForStructureLabelResponse);
	interfaceUnderTest.test_wrapper_process_labels_from_ddop();
	// Send a structure label
	testFrame.identifier = 0x18CB83F7;
	testFrame.data[0] = 0x11; // Mux
	testFrame.data[1] = 'I';
	testFrame.data[2] = '+';
	testFrame.data[3] = '+';
	testFrame.data[4] = '1';
	testFrame.data[5] = '.';
	testFrame.data[6] = '0';
	testFrame.data[7] = ' ';
	CANNetworkManager::CANNetwork.process_receive_can_message_frame(testFrame);
	CANNetworkManager::CANNetwork.update();
	EXPECT_EQ(interfaceUnderTest.test_wrapper_get_state(), TaskControllerClient::StateMachineState::RequestLocalizationLabel);

	// Test structure label with binary DDOP
	interfaceUnderTest.test_wrapper_set_state(TaskControllerClient::StateMachineState::Disconnected);
	EXPECT_EQ(interfaceUnderTest.test_wrapper_get_state(), TaskControllerClient::StateMachineState::Disconnected);
	interfaceUnderTest.configure(DerivedTestTCClient::testBinaryDDOP, sizeof(DerivedTestTCClient::testBinaryDDOP), 32, 32, 32, true, true, true, true, true);
	interfaceUnderTest.update();
	EXPECT_EQ(interfaceUnderTest.test_wrapper_get_state(), TaskControllerClient::StateMachineState::WaitForStartUpDelay);
	interfaceUnderTest.test_wrapper_set_state(TaskControllerClient::StateMachineState::WaitForStructureLabelResponse);
	EXPECT_EQ(interfaceUnderTest.test_wrapper_get_state(), TaskControllerClient::StateMachineState::WaitForStructureLabelResponse);
	interfaceUnderTest.test_wrapper_process_labels_from_ddop();
	testFrame.identifier = 0x18CB83F7;
	testFrame.data[0] = 0x11; // Mux
	testFrame.data[1] = 'I';
	testFrame.data[2] = '+';
	testFrame.data[3] = '+';
	testFrame.data[4] = '1';
	testFrame.data[5] = '.';
	testFrame.data[6] = '0';
	testFrame.data[7] = ' ';
	CANNetworkManager::CANNetwork.process_receive_can_message_frame(testFrame);
	CANNetworkManager::CANNetwork.update();
	EXPECT_EQ(interfaceUnderTest.test_wrapper_get_state(), TaskControllerClient::StateMachineState::RequestLocalizationLabel);

	// Test Begin transfer DDOP state with the binary DDOP
	interfaceUnderTest.test_wrapper_set_state(TaskControllerClient::StateMachineState::BeginTransferDDOP);
	EXPECT_EQ(interfaceUnderTest.test_wrapper_get_state(), TaskControllerClient::StateMachineState::BeginTransferDDOP);
	EXPECT_NO_THROW(interfaceUnderTest.update());
	EXPECT_EQ(interfaceUnderTest.test_wrapper_get_state(), TaskControllerClient::StateMachineState::WaitForDDOPTransfer);

	// Pretend we got connected, and simulate replacing the DDOP. Should leave the connected state to
	// process the DDOP again.
	interfaceUnderTest.test_wrapper_set_state(TaskControllerClient::StateMachineState::Connected);
	EXPECT_EQ(interfaceUnderTest.test_wrapper_get_state(), TaskControllerClient::StateMachineState::Connected);
	interfaceUnderTest.reupload_device_descriptor_object_pool(DerivedTestTCClient::testBinaryDDOP, sizeof(DerivedTestTCClient::testBinaryDDOP));
	EXPECT_NE(interfaceUnderTest.test_wrapper_get_state(), TaskControllerClient::StateMachineState::Connected);

	// And one more time for a vector
	auto testVectorDDOP = std::make_shared<std::vector<std::uint8_t>>(DerivedTestTCClient::testBinaryDDOP, DerivedTestTCClient::testBinaryDDOP + sizeof(DerivedTestTCClient::testBinaryDDOP));
	interfaceUnderTest.test_wrapper_set_state(TaskControllerClient::StateMachineState::Connected);
	EXPECT_EQ(interfaceUnderTest.test_wrapper_get_state(), TaskControllerClient::StateMachineState::Connected);
	interfaceUnderTest.reupload_device_descriptor_object_pool(testVectorDDOP);
	EXPECT_NE(interfaceUnderTest.test_wrapper_get_state(), TaskControllerClient::StateMachineState::Connected);

	// Test the same conditions with a binary DDOP
	interfaceUnderTest.test_wrapper_set_state(TaskControllerClient::StateMachineState::Connected);
	EXPECT_EQ(interfaceUnderTest.test_wrapper_get_state(), TaskControllerClient::StateMachineState::Connected);
	interfaceUnderTest.reupload_device_descriptor_object_pool(testDDOP);
	EXPECT_NE(interfaceUnderTest.test_wrapper_get_state(), TaskControllerClient::StateMachineState::Connected);

	// Cleanup
	interfaceUnderTest.test_wrapper_set_state(TaskControllerClient::StateMachineState::Disconnected);
	EXPECT_EQ(interfaceUnderTest.test_wrapper_get_state(), TaskControllerClient::StateMachineState::Disconnected);

	// Test with a vector binary ddop, this time using the process DDOP state to run process_labels_from_ddop
	auto ddopVector = std::make_shared<std::vector<std::uint8_t>>();
	ddopVector->resize(sizeof(DerivedTestTCClient::testBinaryDDOP));
	memcpy(ddopVector->data(), DerivedTestTCClient::testBinaryDDOP, sizeof(DerivedTestTCClient::testBinaryDDOP));
	interfaceUnderTest.test_wrapper_set_state(TaskControllerClient::StateMachineState::Disconnected);
	EXPECT_EQ(interfaceUnderTest.test_wrapper_get_state(), TaskControllerClient::StateMachineState::Disconnected);
	interfaceUnderTest.configure(ddopVector, 32, 32, 32, true, true, true, true, true);
	interfaceUnderTest.update();
	EXPECT_EQ(interfaceUnderTest.test_wrapper_get_state(), TaskControllerClient::StateMachineState::WaitForStartUpDelay);
	interfaceUnderTest.test_wrapper_set_state(TaskControllerClient::StateMachineState::ProcessDDOP);
	EXPECT_EQ(interfaceUnderTest.test_wrapper_get_state(), TaskControllerClient::StateMachineState::ProcessDDOP);
	interfaceUnderTest.update();
	interfaceUnderTest.test_wrapper_set_state(TaskControllerClient::StateMachineState::WaitForStructureLabelResponse);
	EXPECT_EQ(interfaceUnderTest.test_wrapper_get_state(), TaskControllerClient::StateMachineState::WaitForStructureLabelResponse);
	testFrame.identifier = 0x18CB83F7;
	testFrame.data[0] = 0x11; // Mux
	testFrame.data[1] = 'I';
	testFrame.data[2] = '+';
	testFrame.data[3] = '+';
	testFrame.data[4] = '1';
	testFrame.data[5] = '.';
	testFrame.data[6] = '0';
	testFrame.data[7] = ' ';
	CANNetworkManager::CANNetwork.process_receive_can_message_frame(testFrame);
	CANNetworkManager::CANNetwork.update();
	EXPECT_EQ(interfaceUnderTest.test_wrapper_get_state(), TaskControllerClient::StateMachineState::RequestLocalizationLabel);
	// Cleanup
	interfaceUnderTest.test_wrapper_set_state(TaskControllerClient::StateMachineState::Disconnected);
	EXPECT_EQ(interfaceUnderTest.test_wrapper_get_state(), TaskControllerClient::StateMachineState::Disconnected);
	interfaceUnderTest.configure(testDDOP, 6, 64, 32, false, false, false, false, false);

	// Test wait for localization label response
	interfaceUnderTest.test_wrapper_set_state(TaskControllerClient::StateMachineState::WaitForLocalizationLabelResponse);
	EXPECT_EQ(interfaceUnderTest.test_wrapper_get_state(), TaskControllerClient::StateMachineState::WaitForLocalizationLabelResponse);
	interfaceUnderTest.update();
	EXPECT_EQ(interfaceUnderTest.test_wrapper_get_state(), TaskControllerClient::StateMachineState::WaitForLocalizationLabelResponse);
	// Send a localization label
	testFrame.identifier = 0x18CB83F7;
	testFrame.data[0] = 0x31; // Mux
	testFrame.data[1] = 0xFF; // A bad label, since all 0xFFs
	testFrame.data[2] = 0xFF;
	testFrame.data[3] = 0xFF;
	testFrame.data[4] = 0xFF;
	testFrame.data[5] = 0xFF;
	testFrame.data[6] = 0xFF;
	testFrame.data[7] = 0xFF;
	CANNetworkManager::CANNetwork.process_receive_can_message_frame(testFrame);
	CANNetworkManager::CANNetwork.update();
	EXPECT_EQ(interfaceUnderTest.test_wrapper_get_state(), TaskControllerClient::StateMachineState::SendRequestTransferObjectPool);
	interfaceUnderTest.test_wrapper_set_state(TaskControllerClient::StateMachineState::WaitForLocalizationLabelResponse);
	EXPECT_EQ(interfaceUnderTest.test_wrapper_get_state(), TaskControllerClient::StateMachineState::WaitForLocalizationLabelResponse);
	interfaceUnderTest.update();
	EXPECT_EQ(interfaceUnderTest.test_wrapper_get_state(), TaskControllerClient::StateMachineState::WaitForLocalizationLabelResponse);
	// Send a localization label that doesn't match the stored one
	testFrame.identifier = 0x18CB83F7;
	testFrame.data[0] = 0x31; // Mux
	testFrame.data[1] = 0x01; // A valid label
	testFrame.data[2] = 0xFF;
	testFrame.data[3] = 0xFF;
	testFrame.data[4] = 0xFF;
	testFrame.data[5] = 0xFF;
	testFrame.data[6] = 0xFF;
	testFrame.data[7] = 0xFF;
	CANNetworkManager::CANNetwork.process_receive_can_message_frame(testFrame);
	CANNetworkManager::CANNetwork.update();
	EXPECT_EQ(interfaceUnderTest.test_wrapper_get_state(), TaskControllerClient::StateMachineState::SendDeleteObjectPool);
	interfaceUnderTest.test_wrapper_set_state(TaskControllerClient::StateMachineState::WaitForLocalizationLabelResponse);
	EXPECT_EQ(interfaceUnderTest.test_wrapper_get_state(), TaskControllerClient::StateMachineState::WaitForLocalizationLabelResponse);
	interfaceUnderTest.update();
	EXPECT_EQ(interfaceUnderTest.test_wrapper_get_state(), TaskControllerClient::StateMachineState::WaitForLocalizationLabelResponse);
	interfaceUnderTest.test_wrapper_process_labels_from_ddop();
	// Send a localization label that does match the stored one
	testFrame.identifier = 0x18CB83F7;
	testFrame.data[0] = 0x31; // Mux
	testFrame.data[1] = 0x01; // A matching label
	testFrame.data[2] = 0x00;
	testFrame.data[3] = 0x00;
	testFrame.data[4] = 0x00;
	testFrame.data[5] = 0x00;
	testFrame.data[6] = 0x00;
	testFrame.data[7] = 0x00;
	CANNetworkManager::CANNetwork.process_receive_can_message_frame(testFrame);
	CANNetworkManager::CANNetwork.update();
	EXPECT_EQ(interfaceUnderTest.test_wrapper_get_state(), TaskControllerClient::StateMachineState::SendObjectPoolActivate);

	interfaceUnderTest.test_wrapper_set_state(TaskControllerClient::StateMachineState::WaitForDDOPTransfer);
	EXPECT_EQ(interfaceUnderTest.test_wrapper_get_state(), TaskControllerClient::StateMachineState::WaitForDDOPTransfer);
	CANNetworkManager::CANNetwork.update();
	EXPECT_EQ(interfaceUnderTest.test_wrapper_get_state(), TaskControllerClient::StateMachineState::WaitForDDOPTransfer);
	// Check ddop transfer callback
	interfaceUnderTest.test_wrapper_process_tx_callback(0xCB00, 8, nullptr, std::dynamic_pointer_cast<ControlFunction>(tcPartner), false, &interfaceUnderTest);
	// In this case it should be disconnected because we passed in false.
	EXPECT_EQ(interfaceUnderTest.test_wrapper_get_state(), TaskControllerClient::StateMachineState::Disconnected);

	interfaceUnderTest.test_wrapper_set_state(TaskControllerClient::StateMachineState::WaitForDDOPTransfer);
	EXPECT_EQ(interfaceUnderTest.test_wrapper_get_state(), TaskControllerClient::StateMachineState::WaitForDDOPTransfer);
	CANNetworkManager::CANNetwork.update();
	EXPECT_EQ(interfaceUnderTest.test_wrapper_get_state(), TaskControllerClient::StateMachineState::WaitForDDOPTransfer);
	// Check ddop transfer callback
	interfaceUnderTest.test_wrapper_process_tx_callback(0xCB00, 8, nullptr, std::dynamic_pointer_cast<ControlFunction>(tcPartner), true, &interfaceUnderTest);
	// In this case it should be disconnected because we passed in false.
	EXPECT_EQ(interfaceUnderTest.test_wrapper_get_state(), TaskControllerClient::StateMachineState::WaitForObjectPoolTransferResponse);

	// Test wait for object pool transfer response
	interfaceUnderTest.test_wrapper_set_state(TaskControllerClient::StateMachineState::WaitForObjectPoolTransferResponse);
	EXPECT_EQ(interfaceUnderTest.test_wrapper_get_state(), TaskControllerClient::StateMachineState::WaitForObjectPoolTransferResponse);
	// Send a response with good data
	testFrame.identifier = 0x18CB83F7;
	testFrame.data[0] = 0x71; // Mux
	testFrame.data[1] = 0x00;
	testFrame.data[2] = 0xFF;
	testFrame.data[3] = 0xFF;
	testFrame.data[4] = 0xFF;
	testFrame.data[5] = 0xFF;
	testFrame.data[6] = 0xFF;
	testFrame.data[7] = 0xFF;
	CANNetworkManager::CANNetwork.process_receive_can_message_frame(testFrame);
	CANNetworkManager::CANNetwork.update();
	EXPECT_EQ(interfaceUnderTest.test_wrapper_get_state(), TaskControllerClient::StateMachineState::SendObjectPoolActivate);

	interfaceUnderTest.test_wrapper_set_state(TaskControllerClient::StateMachineState::WaitForObjectPoolTransferResponse);
	EXPECT_EQ(interfaceUnderTest.test_wrapper_get_state(), TaskControllerClient::StateMachineState::WaitForObjectPoolTransferResponse);
	// Send a response with bad data
	testFrame.identifier = 0x18CB83F7;
	testFrame.data[0] = 0x71; // Mux
	testFrame.data[1] = 0x01; // Ran out of memory!
	CANNetworkManager::CANNetwork.process_receive_can_message_frame(testFrame);
	CANNetworkManager::CANNetwork.update();
	EXPECT_NE(interfaceUnderTest.test_wrapper_get_state(), TaskControllerClient::StateMachineState::SendObjectPoolActivate);
	interfaceUnderTest.initialize(false); // Fix the interface after terminate was called

	// Test wait for request object pool transfer response
	interfaceUnderTest.test_wrapper_set_state(TaskControllerClient::StateMachineState::WaitForRequestTransferObjectPoolResponse);
	EXPECT_EQ(interfaceUnderTest.test_wrapper_get_state(), TaskControllerClient::StateMachineState::WaitForRequestTransferObjectPoolResponse);
	// Send a response with good data
	testFrame.identifier = 0x18CB83F7;
	testFrame.data[0] = 0x51; // Mux
	testFrame.data[1] = 0x00;
	CANNetworkManager::CANNetwork.process_receive_can_message_frame(testFrame);
	CANNetworkManager::CANNetwork.update();
	EXPECT_EQ(interfaceUnderTest.test_wrapper_get_state(), TaskControllerClient::StateMachineState::BeginTransferDDOP);

	interfaceUnderTest.test_wrapper_set_state(TaskControllerClient::StateMachineState::WaitForRequestTransferObjectPoolResponse);
	EXPECT_EQ(interfaceUnderTest.test_wrapper_get_state(), TaskControllerClient::StateMachineState::WaitForRequestTransferObjectPoolResponse);
	// Send a response with bad data
	testFrame.identifier = 0x18CB83F7;
	testFrame.data[0] = 0x51; // Mux
	testFrame.data[1] = 0x01; // Not enough memory!
	CANNetworkManager::CANNetwork.process_receive_can_message_frame(testFrame);
	CANNetworkManager::CANNetwork.update();
	EXPECT_NE(interfaceUnderTest.test_wrapper_get_state(), TaskControllerClient::StateMachineState::BeginTransferDDOP);
	interfaceUnderTest.initialize(false); // Fix the interface after terminate was called

	interfaceUnderTest.test_wrapper_set_state(TaskControllerClient::StateMachineState::SendRequestVersionResponse);
	EXPECT_EQ(interfaceUnderTest.test_wrapper_get_state(), TaskControllerClient::StateMachineState::SendRequestVersionResponse);
	interfaceUnderTest.update(); // Update the state, should go to the request language state
	EXPECT_EQ(interfaceUnderTest.test_wrapper_get_state(), TaskControllerClient::StateMachineState::RequestLanguage);

	// Test generating a valid DDOP
	interfaceUnderTest.test_wrapper_set_state(TaskControllerClient::StateMachineState::ProcessDDOP);
	EXPECT_EQ(interfaceUnderTest.test_wrapper_get_state(), TaskControllerClient::StateMachineState::ProcessDDOP);
	interfaceUnderTest.update();
	EXPECT_EQ(interfaceUnderTest.test_wrapper_get_state(), TaskControllerClient::StateMachineState::RequestStructureLabel);

	// Do the DDOP generation again
	interfaceUnderTest.test_wrapper_set_state(TaskControllerClient::StateMachineState::ProcessDDOP);
	EXPECT_EQ(interfaceUnderTest.test_wrapper_get_state(), TaskControllerClient::StateMachineState::ProcessDDOP);
	interfaceUnderTest.update();
	EXPECT_EQ(interfaceUnderTest.test_wrapper_get_state(), TaskControllerClient::StateMachineState::RequestStructureLabel);

	// Try sending the DDOP
	interfaceUnderTest.test_wrapper_set_state(TaskControllerClient::StateMachineState::BeginTransferDDOP);
	EXPECT_EQ(interfaceUnderTest.test_wrapper_get_state(), TaskControllerClient::StateMachineState::BeginTransferDDOP);
	interfaceUnderTest.update();
	EXPECT_EQ(interfaceUnderTest.test_wrapper_get_state(), TaskControllerClient::StateMachineState::WaitForDDOPTransfer);

	// Switch to a trash DDOP
	auto testJunkDDOP = std::make_shared<DeviceDescriptorObjectPool>();
	ASSERT_NE(nullptr, testJunkDDOP);
	testJunkDDOP->add_device_property("aksldfjhalkf", 1, 6, 123, 456);
	interfaceUnderTest.test_wrapper_set_state(TaskControllerClient::StateMachineState::Disconnected);
	interfaceUnderTest.configure(testJunkDDOP, 32, 32, 32, true, true, true, true, true);

	interfaceUnderTest.test_wrapper_set_state(TaskControllerClient::StateMachineState::ProcessDDOP);
	EXPECT_EQ(interfaceUnderTest.test_wrapper_get_state(), TaskControllerClient::StateMachineState::ProcessDDOP);
	interfaceUnderTest.update();
	interfaceUnderTest.initialize(false); // Fix after terminate gets called.

	// Test sending request for object pool
	interfaceUnderTest.test_wrapper_set_state(TaskControllerClient::StateMachineState::SendRequestTransferObjectPool);
	EXPECT_EQ(interfaceUnderTest.test_wrapper_get_state(), TaskControllerClient::StateMachineState::SendRequestTransferObjectPool);
	interfaceUnderTest.update();
	EXPECT_EQ(interfaceUnderTest.test_wrapper_get_state(), TaskControllerClient::StateMachineState::WaitForRequestTransferObjectPoolResponse);

	interfaceUnderTest.test_wrapper_set_state(TaskControllerClient::StateMachineState::WaitForObjectPoolActivateResponse);
	EXPECT_EQ(interfaceUnderTest.test_wrapper_get_state(), TaskControllerClient::StateMachineState::WaitForObjectPoolActivateResponse);
	interfaceUnderTest.update();
	EXPECT_EQ(interfaceUnderTest.test_wrapper_get_state(), TaskControllerClient::StateMachineState::WaitForObjectPoolActivateResponse);
	testFrame.identifier = 0x18CB83F7;
	testFrame.data[0] = 0x91; // Mux
	testFrame.data[1] = 0x00; // It worked!
	testFrame.data[2] = 0xFF;
	testFrame.data[3] = 0xFF;
	testFrame.data[4] = 0xFF;
	testFrame.data[5] = 0xFF;
	testFrame.data[6] = 0xFF;
	testFrame.data[7] = 0xFF;
	CANNetworkManager::CANNetwork.process_receive_can_message_frame(testFrame);
	CANNetworkManager::CANNetwork.update();
	EXPECT_EQ(interfaceUnderTest.test_wrapper_get_state(), TaskControllerClient::StateMachineState::Connected);
	interfaceUnderTest.test_wrapper_set_state(TaskControllerClient::StateMachineState::WaitForObjectPoolActivateResponse);
	EXPECT_EQ(interfaceUnderTest.test_wrapper_get_state(), TaskControllerClient::StateMachineState::WaitForObjectPoolActivateResponse);
	interfaceUnderTest.update();
	EXPECT_EQ(interfaceUnderTest.test_wrapper_get_state(), TaskControllerClient::StateMachineState::WaitForObjectPoolActivateResponse);
	testFrame.identifier = 0x18CB83F7;
	testFrame.data[0] = 0x91; // Mux
	testFrame.data[1] = 0x01; // It didn't work!
	testFrame.data[2] = 0xFF;
	testFrame.data[3] = 0xFF;
	testFrame.data[4] = 0xFF;
	testFrame.data[5] = 0xFF;
	testFrame.data[6] = 0xFF;
	testFrame.data[7] = 0xFF;
	CANNetworkManager::CANNetwork.process_receive_can_message_frame(testFrame);
	CANNetworkManager::CANNetwork.update();
	EXPECT_NE(interfaceUnderTest.test_wrapper_get_state(), TaskControllerClient::StateMachineState::Connected);

	// Test version request state
	interfaceUnderTest.test_wrapper_set_state(TaskControllerClient::StateMachineState::RequestVersion);
	EXPECT_EQ(interfaceUnderTest.test_wrapper_get_state(), TaskControllerClient::StateMachineState::RequestVersion);
	interfaceUnderTest.update();
	EXPECT_EQ(interfaceUnderTest.test_wrapper_get_state(), TaskControllerClient::StateMachineState::WaitForRequestVersionResponse);

	// Test resetting the state machine
	interfaceUnderTest.restart();
	EXPECT_EQ(interfaceUnderTest.test_wrapper_get_state(), TaskControllerClient::StateMachineState::Disconnected);

	//! @Todo Add other states

	// Test invalid state gets caught by assert
	interfaceUnderTest.test_wrapper_set_state(static_cast<TaskControllerClient::StateMachineState>(241));
	EXPECT_DEATH(interfaceUnderTest.update(), "");

	interfaceUnderTest.terminate();
	CANHardwareInterface::stop();

	CANNetworkManager::CANNetwork.deactivate_control_function(tcPartner);
	CANNetworkManager::CANNetwork.deactivate_control_function(internalECU);
}

TEST(TASK_CONTROLLER_CLIENT_TESTS, ClientSettings)
{
	DerivedTestTCClient interfaceUnderTest(nullptr, nullptr);
	auto blankDDOP = std::make_shared<DeviceDescriptorObjectPool>();

	// Set and test the basic settings for the client
	interfaceUnderTest.configure(blankDDOP, 6, 64, 32, false, false, false, false, false);
	EXPECT_EQ(6, interfaceUnderTest.get_number_booms_supported());
	EXPECT_EQ(64, interfaceUnderTest.get_number_sections_supported());
	EXPECT_EQ(32, interfaceUnderTest.get_number_channels_supported_for_position_based_control());
	EXPECT_EQ(false, interfaceUnderTest.get_supports_documentation());
	EXPECT_EQ(false, interfaceUnderTest.get_supports_implement_section_control());
	EXPECT_EQ(false, interfaceUnderTest.get_supports_peer_control_assignment());
	EXPECT_EQ(false, interfaceUnderTest.get_supports_tcgeo_without_position_based_control());
	EXPECT_EQ(false, interfaceUnderTest.get_supports_tcgeo_with_position_based_control());
	interfaceUnderTest.configure(blankDDOP, 255, 255, 255, true, true, true, true, true);
	EXPECT_EQ(255, interfaceUnderTest.get_number_booms_supported());
	EXPECT_EQ(255, interfaceUnderTest.get_number_sections_supported());
	EXPECT_EQ(255, interfaceUnderTest.get_number_channels_supported_for_position_based_control());
	EXPECT_EQ(true, interfaceUnderTest.get_supports_documentation());
	EXPECT_EQ(true, interfaceUnderTest.get_supports_implement_section_control());
	EXPECT_EQ(true, interfaceUnderTest.get_supports_peer_control_assignment());
	EXPECT_EQ(true, interfaceUnderTest.get_supports_tcgeo_without_position_based_control());
	EXPECT_EQ(true, interfaceUnderTest.get_supports_tcgeo_with_position_based_control());
}

TEST(TASK_CONTROLLER_CLIENT_TESTS, TimeoutTests)
{
	NAME clientNAME(0);
	clientNAME.set_industry_group(2);
	clientNAME.set_ecu_instance(1);
	clientNAME.set_function_code(static_cast<std::uint8_t>(NAME::Function::RateControl));
	auto internalECU = CANNetworkManager::CANNetwork.create_internal_control_function(clientNAME, 0, 0x84);

	ASSERT_FALSE(internalECU->get_address_valid());

	std::vector<isobus::NAMEFilter> vtNameFilters;
	const isobus::NAMEFilter testFilter(isobus::NAME::NAMEParameters::FunctionCode, static_cast<std::uint8_t>(isobus::NAME::Function::TaskController));
	vtNameFilters.push_back(testFilter);

	auto tcPartner = CANNetworkManager::CANNetwork.create_partnered_control_function(0, vtNameFilters);

	CANNetworkManager::CANNetwork.update();

	DerivedTestTCClient interfaceUnderTest(tcPartner, internalECU);
	interfaceUnderTest.initialize(false);

	// Wait a while to build up some run time for testing timeouts later
	while (SystemTiming::get_timestamp_ms() < 6000)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
	}

	// Test disconnecting from trying to send working set master
	interfaceUnderTest.test_wrapper_set_state(TaskControllerClient::StateMachineState::SendWorkingSetMaster, 0);
	EXPECT_EQ(interfaceUnderTest.test_wrapper_get_state(), TaskControllerClient::StateMachineState::SendWorkingSetMaster);
	interfaceUnderTest.update();
	EXPECT_EQ(interfaceUnderTest.test_wrapper_get_state(), TaskControllerClient::StateMachineState::Disconnected);

	// Test disconnecting from trying to send status message
	interfaceUnderTest.test_wrapper_set_state(TaskControllerClient::StateMachineState::SendStatusMessage, 0);
	EXPECT_EQ(interfaceUnderTest.test_wrapper_get_state(), TaskControllerClient::StateMachineState::SendStatusMessage);
	interfaceUnderTest.update();
	EXPECT_EQ(interfaceUnderTest.test_wrapper_get_state(), TaskControllerClient::StateMachineState::Disconnected);

	// Test disconnecting from trying to send request version message
	interfaceUnderTest.test_wrapper_set_state(TaskControllerClient::StateMachineState::RequestVersion, 0);
	EXPECT_EQ(interfaceUnderTest.test_wrapper_get_state(), TaskControllerClient::StateMachineState::RequestVersion);
	interfaceUnderTest.update();
	EXPECT_EQ(interfaceUnderTest.test_wrapper_get_state(), TaskControllerClient::StateMachineState::Disconnected);

	// Test disconnecting from trying to send request structure label message
	interfaceUnderTest.test_wrapper_set_state(TaskControllerClient::StateMachineState::RequestStructureLabel, 0);
	EXPECT_EQ(interfaceUnderTest.test_wrapper_get_state(), TaskControllerClient::StateMachineState::RequestStructureLabel);
	interfaceUnderTest.update();
	EXPECT_EQ(interfaceUnderTest.test_wrapper_get_state(), TaskControllerClient::StateMachineState::Disconnected);

	// Test disconnecting from trying to send request localization label message
	interfaceUnderTest.test_wrapper_set_state(TaskControllerClient::StateMachineState::RequestLocalizationLabel, 0);
	EXPECT_EQ(interfaceUnderTest.test_wrapper_get_state(), TaskControllerClient::StateMachineState::RequestLocalizationLabel);
	interfaceUnderTest.update();
	EXPECT_EQ(interfaceUnderTest.test_wrapper_get_state(), TaskControllerClient::StateMachineState::Disconnected);

	// Test disconnecting from waiting for request version response
	interfaceUnderTest.test_wrapper_set_state(TaskControllerClient::StateMachineState::WaitForRequestVersionResponse, 0);
	EXPECT_EQ(interfaceUnderTest.test_wrapper_get_state(), TaskControllerClient::StateMachineState::WaitForRequestVersionResponse);
	interfaceUnderTest.update();
	EXPECT_EQ(interfaceUnderTest.test_wrapper_get_state(), TaskControllerClient::StateMachineState::Disconnected);

	// Test disconnecting from waiting for structure label response
	interfaceUnderTest.test_wrapper_set_state(TaskControllerClient::StateMachineState::WaitForStructureLabelResponse, 0);
	EXPECT_EQ(interfaceUnderTest.test_wrapper_get_state(), TaskControllerClient::StateMachineState::WaitForStructureLabelResponse);
	interfaceUnderTest.update();
	EXPECT_EQ(interfaceUnderTest.test_wrapper_get_state(), TaskControllerClient::StateMachineState::Disconnected);

	// Test disconnecting from sending delete object pool
	interfaceUnderTest.test_wrapper_set_state(TaskControllerClient::StateMachineState::SendDeleteObjectPool, 0);
	EXPECT_EQ(interfaceUnderTest.test_wrapper_get_state(), TaskControllerClient::StateMachineState::SendDeleteObjectPool);
	interfaceUnderTest.update();
	EXPECT_EQ(interfaceUnderTest.test_wrapper_get_state(), TaskControllerClient::StateMachineState::Disconnected);

	// Test disconnecting while waiting for object pool delete response
	interfaceUnderTest.test_wrapper_set_state(TaskControllerClient::StateMachineState::WaitForDeleteObjectPoolResponse, 0);
	EXPECT_EQ(interfaceUnderTest.test_wrapper_get_state(), TaskControllerClient::StateMachineState::WaitForDeleteObjectPoolResponse);
	interfaceUnderTest.update();
	EXPECT_EQ(interfaceUnderTest.test_wrapper_get_state(), TaskControllerClient::StateMachineState::Disconnected);

	// Test disconnecting while waiting for sending request to transfer the DDOP
	interfaceUnderTest.test_wrapper_set_state(TaskControllerClient::StateMachineState::SendRequestTransferObjectPool, 0);
	EXPECT_EQ(interfaceUnderTest.test_wrapper_get_state(), TaskControllerClient::StateMachineState::SendRequestTransferObjectPool);
	interfaceUnderTest.update();
	EXPECT_EQ(interfaceUnderTest.test_wrapper_get_state(), TaskControllerClient::StateMachineState::Disconnected);

	// Test disconnecting while trying to send the DDOP
	interfaceUnderTest.test_wrapper_set_state(TaskControllerClient::StateMachineState::BeginTransferDDOP, 0);
	EXPECT_EQ(interfaceUnderTest.test_wrapper_get_state(), TaskControllerClient::StateMachineState::BeginTransferDDOP);
	interfaceUnderTest.update();
	EXPECT_EQ(interfaceUnderTest.test_wrapper_get_state(), TaskControllerClient::StateMachineState::Disconnected);

	// Test startup delay
	interfaceUnderTest.test_wrapper_set_state(TaskControllerClient::StateMachineState::WaitForStartUpDelay, 0);
	EXPECT_EQ(interfaceUnderTest.test_wrapper_get_state(), TaskControllerClient::StateMachineState::WaitForStartUpDelay);
	interfaceUnderTest.update();
	EXPECT_EQ(interfaceUnderTest.test_wrapper_get_state(), TaskControllerClient::StateMachineState::WaitForServerStatusMessage);

	// Test no timeout when waiting for the status message initially
	interfaceUnderTest.test_wrapper_set_state(TaskControllerClient::StateMachineState::WaitForServerStatusMessage, 0);
	EXPECT_EQ(interfaceUnderTest.test_wrapper_get_state(), TaskControllerClient::StateMachineState::WaitForServerStatusMessage);
	interfaceUnderTest.update();
	EXPECT_EQ(interfaceUnderTest.test_wrapper_get_state(), TaskControllerClient::StateMachineState::WaitForServerStatusMessage);

	// Test no timeout when waiting for Tx to complete. We will get a callback from transport layer for this
	interfaceUnderTest.test_wrapper_set_state(TaskControllerClient::StateMachineState::WaitForDDOPTransfer, 0);
	EXPECT_EQ(interfaceUnderTest.test_wrapper_get_state(), TaskControllerClient::StateMachineState::WaitForDDOPTransfer);
	interfaceUnderTest.update();
	EXPECT_EQ(interfaceUnderTest.test_wrapper_get_state(), TaskControllerClient::StateMachineState::WaitForDDOPTransfer);

	// Test timeout waiting for object pool transfer response
	interfaceUnderTest.test_wrapper_set_state(TaskControllerClient::StateMachineState::WaitForRequestTransferObjectPoolResponse, 0);
	EXPECT_EQ(interfaceUnderTest.test_wrapper_get_state(), TaskControllerClient::StateMachineState::WaitForRequestTransferObjectPoolResponse);
	interfaceUnderTest.update();
	EXPECT_EQ(interfaceUnderTest.test_wrapper_get_state(), TaskControllerClient::StateMachineState::Disconnected);

	// Test timeout trying to send object pool activation
	interfaceUnderTest.test_wrapper_set_state(TaskControllerClient::StateMachineState::SendObjectPoolActivate, 0);
	EXPECT_EQ(interfaceUnderTest.test_wrapper_get_state(), TaskControllerClient::StateMachineState::SendObjectPoolActivate);
	interfaceUnderTest.update();
	EXPECT_EQ(interfaceUnderTest.test_wrapper_get_state(), TaskControllerClient::StateMachineState::Disconnected);
	interfaceUnderTest.test_wrapper_set_state(TaskControllerClient::StateMachineState::SendObjectPoolActivate);
	EXPECT_EQ(interfaceUnderTest.test_wrapper_get_state(), TaskControllerClient::StateMachineState::SendObjectPoolActivate);
	interfaceUnderTest.update();
	EXPECT_EQ(interfaceUnderTest.test_wrapper_get_state(), TaskControllerClient::StateMachineState::SendObjectPoolActivate);

	// Test timeout waiting to activate object pool
	interfaceUnderTest.test_wrapper_set_state(TaskControllerClient::StateMachineState::WaitForObjectPoolActivateResponse, 0);
	EXPECT_EQ(interfaceUnderTest.test_wrapper_get_state(), TaskControllerClient::StateMachineState::WaitForObjectPoolActivateResponse);
	interfaceUnderTest.update();
	EXPECT_EQ(interfaceUnderTest.test_wrapper_get_state(), TaskControllerClient::StateMachineState::Disconnected);

	// Test timeout while connected
	interfaceUnderTest.test_wrapper_set_state(TaskControllerClient::StateMachineState::Connected, 0);
	EXPECT_EQ(interfaceUnderTest.test_wrapper_get_state(), TaskControllerClient::StateMachineState::Connected);
	interfaceUnderTest.update();
	EXPECT_EQ(interfaceUnderTest.test_wrapper_get_state(), TaskControllerClient::StateMachineState::Disconnected);

	// Test trying to deactivate object pool
	interfaceUnderTest.test_wrapper_set_state(TaskControllerClient::StateMachineState::DeactivateObjectPool, 0);
	EXPECT_EQ(interfaceUnderTest.test_wrapper_get_state(), TaskControllerClient::StateMachineState::DeactivateObjectPool);
	interfaceUnderTest.update();
	EXPECT_EQ(interfaceUnderTest.test_wrapper_get_state(), TaskControllerClient::StateMachineState::Disconnected);
	interfaceUnderTest.test_wrapper_set_state(TaskControllerClient::StateMachineState::DeactivateObjectPool);
	EXPECT_EQ(interfaceUnderTest.test_wrapper_get_state(), TaskControllerClient::StateMachineState::DeactivateObjectPool);
	interfaceUnderTest.update();
	EXPECT_EQ(interfaceUnderTest.test_wrapper_get_state(), TaskControllerClient::StateMachineState::DeactivateObjectPool);

	// Test trying to deactivate object pool
	interfaceUnderTest.test_wrapper_set_state(TaskControllerClient::StateMachineState::WaitForObjectPoolDeactivateResponse, 0);
	EXPECT_EQ(interfaceUnderTest.test_wrapper_get_state(), TaskControllerClient::StateMachineState::WaitForObjectPoolDeactivateResponse);
	interfaceUnderTest.update();
	EXPECT_EQ(interfaceUnderTest.test_wrapper_get_state(), TaskControllerClient::StateMachineState::Disconnected);

	// Test timeout waiting for localization label response
	interfaceUnderTest.test_wrapper_set_state(TaskControllerClient::StateMachineState::WaitForLocalizationLabelResponse, 0);
	EXPECT_EQ(interfaceUnderTest.test_wrapper_get_state(), TaskControllerClient::StateMachineState::WaitForLocalizationLabelResponse);
	interfaceUnderTest.update();
	EXPECT_EQ(interfaceUnderTest.test_wrapper_get_state(), TaskControllerClient::StateMachineState::Disconnected);

	// Test timeout waiting for version request from server
	interfaceUnderTest.test_wrapper_set_state(TaskControllerClient::StateMachineState::WaitForRequestVersionFromServer, 0);
	EXPECT_EQ(interfaceUnderTest.test_wrapper_get_state(), TaskControllerClient::StateMachineState::WaitForRequestVersionFromServer);
	interfaceUnderTest.update();
	EXPECT_EQ(interfaceUnderTest.test_wrapper_get_state(), TaskControllerClient::StateMachineState::RequestLanguage);

	// Test that we can't get stuck in the request language state
	interfaceUnderTest.test_wrapper_set_state(TaskControllerClient::StateMachineState::WaitForLanguageResponse);
	EXPECT_EQ(interfaceUnderTest.test_wrapper_get_state(), TaskControllerClient::StateMachineState::WaitForLanguageResponse);
	interfaceUnderTest.update();
	EXPECT_EQ(interfaceUnderTest.test_wrapper_get_state(), TaskControllerClient::StateMachineState::ProcessDDOP);

	// Test timeout waiting for object pool transfer response
	interfaceUnderTest.test_wrapper_set_state(TaskControllerClient::StateMachineState::WaitForObjectPoolTransferResponse, 0);
	EXPECT_EQ(interfaceUnderTest.test_wrapper_get_state(), TaskControllerClient::StateMachineState::WaitForObjectPoolTransferResponse);
	interfaceUnderTest.update();
	EXPECT_EQ(interfaceUnderTest.test_wrapper_get_state(), TaskControllerClient::StateMachineState::Disconnected);

	// Waiting for object pool transfer response hold state
	interfaceUnderTest.test_wrapper_set_state(TaskControllerClient::StateMachineState::WaitForObjectPoolTransferResponse);
	EXPECT_EQ(interfaceUnderTest.test_wrapper_get_state(), TaskControllerClient::StateMachineState::WaitForObjectPoolTransferResponse);
	interfaceUnderTest.update();
	EXPECT_EQ(interfaceUnderTest.test_wrapper_get_state(), TaskControllerClient::StateMachineState::WaitForObjectPoolTransferResponse);

	// Test timeout waiting to send request version response
	interfaceUnderTest.test_wrapper_set_state(TaskControllerClient::StateMachineState::SendRequestVersionResponse);
	EXPECT_EQ(interfaceUnderTest.test_wrapper_get_state(), TaskControllerClient::StateMachineState::SendRequestVersionResponse);
	interfaceUnderTest.update();
	EXPECT_EQ(interfaceUnderTest.test_wrapper_get_state(), TaskControllerClient::StateMachineState::SendRequestVersionResponse);
	interfaceUnderTest.test_wrapper_set_state(TaskControllerClient::StateMachineState::SendRequestVersionResponse, 0);
	EXPECT_EQ(interfaceUnderTest.test_wrapper_get_state(), TaskControllerClient::StateMachineState::SendRequestVersionResponse);
	interfaceUnderTest.update();
	EXPECT_EQ(interfaceUnderTest.test_wrapper_get_state(), TaskControllerClient::StateMachineState::Disconnected);

	CANNetworkManager::CANNetwork.deactivate_control_function(tcPartner);
	CANNetworkManager::CANNetwork.deactivate_control_function(internalECU);
}

TEST(TASK_CONTROLLER_CLIENT_TESTS, WorkerThread)
{
	NAME clientNAME(0);
	clientNAME.set_industry_group(2);
	clientNAME.set_ecu_instance(1);
	clientNAME.set_function_code(static_cast<std::uint8_t>(NAME::Function::RateControl));
	auto internalECU = CANNetworkManager::CANNetwork.create_internal_control_function(clientNAME, 0, 0x85);

	std::vector<isobus::NAMEFilter> vtNameFilters;
	const isobus::NAMEFilter testFilter(isobus::NAME::NAMEParameters::FunctionCode, static_cast<std::uint8_t>(isobus::NAME::Function::TaskController));
	vtNameFilters.push_back(testFilter);

	auto tcPartner = CANNetworkManager::CANNetwork.create_partnered_control_function(0, vtNameFilters);

	CANNetworkManager::CANNetwork.update();

	DerivedTestTCClient interfaceUnderTest(tcPartner, internalECU);
	EXPECT_NO_THROW(interfaceUnderTest.initialize(true));

	EXPECT_NO_THROW(interfaceUnderTest.terminate());
	CANNetworkManager::CANNetwork.deactivate_control_function(tcPartner); // Account for the pointer in the TC client and the language interface
}

static bool valueRequested = false;
static bool valueCommanded = false;
static std::uint16_t requestedDDI = 0;
static std::uint16_t commandedDDI = 0;
static std::uint16_t requestedElement = 0;
static std::uint16_t commandedElement = 0;
static std::int32_t commandedValue = 0;

bool request_value_command_callback(std::uint16_t element,
                                    std::uint16_t ddi,
                                    std::int32_t &,
                                    void *)
{
	requestedElement = element;
	requestedDDI = ddi;
	valueRequested = true;
	return true;
}

bool value_command_callback(std::uint16_t element,
                            std::uint16_t ddi,
                            std::int32_t value,
                            void *)
{
	commandedElement = element;
	commandedDDI = ddi;
	valueCommanded = true;
	commandedValue = value;
	return true;
}

TEST(TASK_CONTROLLER_CLIENT_TESTS, CallbackTests)
{
	VirtualCANPlugin serverTC;
	serverTC.open();

	CANHardwareInterface::set_number_of_can_channels(1);
	CANHardwareInterface::assign_can_channel_frame_handler(0, std::make_shared<VirtualCANPlugin>());
	CANHardwareInterface::start();

	auto internalECU = test_helpers::claim_internal_control_function(0x86, 0);
	auto TestPartnerTC = test_helpers::force_claim_partnered_control_function(0xF7, 0);

	DerivedTestTCClient interfaceUnderTest(TestPartnerTC, internalECU);
	interfaceUnderTest.initialize(false);

	std::this_thread::sleep_for(std::chrono::milliseconds(50));

	// Get the virtual CAN plugin back to a known state
	CANMessageFrame testFrame = {};
	while (!serverTC.get_queue_empty())
	{
		serverTC.read_frame(testFrame);
	}
	ASSERT_TRUE(serverTC.get_queue_empty());
	// End boilerplate **********************************

	auto blankDDOP = std::make_shared<DeviceDescriptorObjectPool>();
	interfaceUnderTest.configure(blankDDOP, 1, 32, 32, true, false, true, false, true);
	interfaceUnderTest.add_request_value_callback(request_value_command_callback, nullptr);
	interfaceUnderTest.add_value_command_callback(value_command_callback, nullptr);
	interfaceUnderTest.test_wrapper_set_state(TaskControllerClient::StateMachineState::Connected);

	// Status message
	testFrame.identifier = 0x18CBFFF7;
	testFrame.data[0] = 0xFE; // Status mux
	testFrame.data[1] = 0xFF; // Element number, set to not available
	testFrame.data[2] = 0xFF; // DDI (N/A)
	testFrame.data[3] = 0xFF; // DDI (N/A)
	testFrame.data[4] = 0x01; // Status (task active)
	testFrame.data[5] = 0x00; // Command address
	testFrame.data[6] = 0x00; // Command
	testFrame.data[7] = 0xFF; // Reserved
	CANNetworkManager::CANNetwork.process_receive_can_message_frame(testFrame);

	// Create a request for a value.
	testFrame.identifier = 0x18CB86F7;
	testFrame.data[0] = 0x82;
	testFrame.data[1] = 0x04;
	testFrame.data[2] = 0x12;
	testFrame.data[3] = 0x34;
	testFrame.data[4] = 0x00;
	testFrame.data[5] = 0x00;
	testFrame.data[6] = 0x00;
	testFrame.data[7] = 0x00;
	CANNetworkManager::CANNetwork.process_receive_can_message_frame(testFrame);
	CANNetworkManager::CANNetwork.update();
	interfaceUnderTest.update();

	// Ensure the values were passed through to the callback properly
	EXPECT_EQ(true, valueRequested);
	EXPECT_EQ(requestedDDI, 0x3412);
	EXPECT_EQ(requestedElement, 0x48);
	EXPECT_EQ(false, valueCommanded);
	EXPECT_EQ(commandedDDI, 0);
	EXPECT_EQ(commandedElement, 0);

	// Create a command for a value.
	testFrame.identifier = 0x18CB86F7;
	testFrame.data[0] = 0x83;
	testFrame.data[1] = 0x05;
	testFrame.data[2] = 0x19;
	testFrame.data[3] = 0x38;
	testFrame.data[4] = 0x01;
	testFrame.data[5] = 0x02;
	testFrame.data[6] = 0x03;
	testFrame.data[7] = 0x04;
	CANNetworkManager::CANNetwork.process_receive_can_message_frame(testFrame);
	CANNetworkManager::CANNetwork.update();
	interfaceUnderTest.update();

	// Ensure the values were passed through to the callback properly
	EXPECT_EQ(true, valueCommanded);
	EXPECT_EQ(commandedDDI, 0x3819);
	EXPECT_EQ(commandedElement, 0x58);
	EXPECT_EQ(commandedValue, 0x4030201);
	EXPECT_EQ(true, valueRequested);
	EXPECT_EQ(requestedDDI, 0x3412);
	EXPECT_EQ(requestedElement, 0x48);

	// Set value and acknowledge
	testFrame.identifier = 0x18CB86F7;
	testFrame.data[0] = 0x2A;
	testFrame.data[1] = 0x05;
	testFrame.data[2] = 0x29;
	testFrame.data[3] = 0x48;
	testFrame.data[4] = 0x08;
	testFrame.data[5] = 0x07;
	testFrame.data[6] = 0x06;
	testFrame.data[7] = 0x05;
	CANNetworkManager::CANNetwork.process_receive_can_message_frame(testFrame);
	CANNetworkManager::CANNetwork.update();
	interfaceUnderTest.update();

	EXPECT_EQ(true, valueCommanded);
	EXPECT_EQ(commandedDDI, 0x4829);
	EXPECT_EQ(commandedElement, 0x52);
	EXPECT_EQ(commandedValue, 0x5060708);
	EXPECT_EQ(true, valueRequested);
	EXPECT_EQ(requestedDDI, 0x3412);
	EXPECT_EQ(requestedElement, 0x48);

	valueRequested = false;
	requestedDDI = 0;
	requestedElement = 0;

	// Test negative number
	testFrame.identifier = 0x18CB86F7;
	testFrame.data[0] = 0x2A;
	testFrame.data[1] = 0x05;
	testFrame.data[2] = 0x29;
	testFrame.data[3] = 0x48;
	testFrame.data[4] = 0x11;
	testFrame.data[5] = 0x01;
	testFrame.data[6] = 0x00;
	testFrame.data[7] = 0xF0;
	CANNetworkManager::CANNetwork.process_receive_can_message_frame(testFrame);
	CANNetworkManager::CANNetwork.update();
	interfaceUnderTest.update();

	EXPECT_EQ(true, valueCommanded);
	EXPECT_EQ(commandedDDI, 0x4829);
	EXPECT_EQ(commandedElement, 0x52);
	EXPECT_EQ(commandedValue, -268435183);

	valueCommanded = false;
	commandedDDI = 0;
	commandedValue = 0;
	interfaceUnderTest.remove_request_value_callback(request_value_command_callback, nullptr);

	// Create a request for a value.
	testFrame.identifier = 0x18CB86F7;
	testFrame.data[0] = 0x82;
	testFrame.data[1] = 0x04;
	testFrame.data[2] = 0x12;
	testFrame.data[3] = 0x34;
	testFrame.data[4] = 0x00;
	testFrame.data[5] = 0x00;
	testFrame.data[6] = 0x00;
	testFrame.data[7] = 0x00;
	CANNetworkManager::CANNetwork.process_receive_can_message_frame(testFrame);
	CANNetworkManager::CANNetwork.update();
	interfaceUnderTest.update();
	// This time the callback should be gone.
	EXPECT_EQ(false, valueRequested);
	EXPECT_EQ(requestedDDI, 0);
	EXPECT_EQ(requestedElement, 0);

	valueCommanded = false;
	commandedDDI = 0;
	commandedElement = 0;
	commandedValue = 0x0;
	interfaceUnderTest.remove_value_command_callback(value_command_callback, nullptr);

	// Create a command for a value.
	testFrame.identifier = 0x18CB86F7;
	testFrame.data[0] = 0x83;
	testFrame.data[1] = 0x05;
	testFrame.data[2] = 0x19;
	testFrame.data[3] = 0x38;
	testFrame.data[4] = 0x01;
	testFrame.data[5] = 0x02;
	testFrame.data[6] = 0x03;
	testFrame.data[7] = 0x04;
	CANNetworkManager::CANNetwork.process_receive_can_message_frame(testFrame);
	CANNetworkManager::CANNetwork.update();
	interfaceUnderTest.update();

	// Now since the callback has been removed, no command should have happened
	EXPECT_EQ(false, valueCommanded);
	EXPECT_EQ(commandedDDI, 0);
	EXPECT_EQ(commandedElement, 0);
	EXPECT_EQ(commandedValue, 0);

	// Test time interval measurement commands
	interfaceUnderTest.add_request_value_callback(request_value_command_callback, nullptr);
	interfaceUnderTest.add_value_command_callback(value_command_callback, nullptr);
	// Create a command
	testFrame.identifier = 0x18CB86F7;
	testFrame.data[0] = 0xA4;
	testFrame.data[1] = 0x05;
	testFrame.data[2] = 0x19;
	testFrame.data[3] = 0x38;
	testFrame.data[4] = 0x01; // 1ms
	testFrame.data[5] = 0x00;
	testFrame.data[6] = 0x00;
	testFrame.data[7] = 0x00;
	CANNetworkManager::CANNetwork.process_receive_can_message_frame(testFrame);
	CANNetworkManager::CANNetwork.update();
	interfaceUnderTest.update();

	std::this_thread::sleep_for(std::chrono::milliseconds(2));

	interfaceUnderTest.update();
	EXPECT_EQ(true, valueRequested);
	EXPECT_EQ(requestedDDI, 0x3819);

	// Toggle states to clear the commands list
	interfaceUnderTest.test_wrapper_set_state(TaskControllerClient::StateMachineState::SendStatusMessage); // Arbitrary
	interfaceUnderTest.test_wrapper_set_state(TaskControllerClient::StateMachineState::Disconnected); // Clear commands
	interfaceUnderTest.test_wrapper_set_state(TaskControllerClient::StateMachineState::Connected); // Arbitrary
	valueRequested = false;
	requestedDDI = 0;
	requestedElement = 0;

	// Create on change thresholds
	testFrame.identifier = 0x18CB86F7;
	testFrame.data[0] = 0xA8;
	testFrame.data[1] = 0x05;
	testFrame.data[2] = 0x19;
	testFrame.data[3] = 0x39;
	testFrame.data[4] = 0x01; // Change value of 1
	testFrame.data[5] = 0x00;
	testFrame.data[6] = 0x00;
	testFrame.data[7] = 0x00;
	CANNetworkManager::CANNetwork.process_receive_can_message_frame(testFrame);
	CANNetworkManager::CANNetwork.update();
	interfaceUnderTest.update();

	EXPECT_EQ(true, valueRequested);
	EXPECT_EQ(requestedDDI, 0x3919);

	// Toggle states to clear the commands list
	interfaceUnderTest.test_wrapper_set_state(TaskControllerClient::StateMachineState::Disconnected); // Clear commands
	interfaceUnderTest.test_wrapper_set_state(TaskControllerClient::StateMachineState::Connected); // Arbitrary
	valueRequested = false;
	requestedDDI = 0;
	requestedElement = 0;

	// Create max thresholds
	testFrame.identifier = 0x18CB86F7;
	testFrame.data[0] = 0xA7;
	testFrame.data[1] = 0x05;
	testFrame.data[2] = 0x19;
	testFrame.data[3] = 0x3A;
	testFrame.data[4] = 0x10; // Max of 16
	testFrame.data[5] = 0x00;
	testFrame.data[6] = 0x00;
	testFrame.data[7] = 0x00;
	CANNetworkManager::CANNetwork.process_receive_can_message_frame(testFrame);
	CANNetworkManager::CANNetwork.update();
	interfaceUnderTest.update();

	EXPECT_EQ(true, valueRequested);
	EXPECT_EQ(requestedDDI, 0x3A19);

	// Toggle states to clear the commands list
	interfaceUnderTest.test_wrapper_set_state(TaskControllerClient::StateMachineState::Disconnected); // Clear commands
	interfaceUnderTest.test_wrapper_set_state(TaskControllerClient::StateMachineState::Connected); // Arbitrary
	valueRequested = false;
	requestedDDI = 0;
	requestedElement = 0;

	// Create min thresholds
	testFrame.identifier = 0x18CB86F7;
	testFrame.data[0] = 0xA6;
	testFrame.data[1] = 0x05;
	testFrame.data[2] = 0x19;
	testFrame.data[3] = 0x3B;
	testFrame.data[4] = 0x10; // Min of 16
	testFrame.data[5] = 0x00;
	testFrame.data[6] = 0x00;
	testFrame.data[7] = 0x00;
	CANNetworkManager::CANNetwork.process_receive_can_message_frame(testFrame);
	CANNetworkManager::CANNetwork.update();
	interfaceUnderTest.update();

	EXPECT_EQ(true, valueRequested);
	EXPECT_EQ(requestedDDI, 0x3B19);

	interfaceUnderTest.test_wrapper_set_state(TaskControllerClient::StateMachineState::Disconnected); // Clear commands
	interfaceUnderTest.test_wrapper_set_state(TaskControllerClient::StateMachineState::Connected); // Arbitrary
	valueRequested = false;
	requestedDDI = 0;
	requestedElement = 0;

	// Test distance thresholds
	testFrame.identifier = 0x18CB86F7;
	testFrame.data[0] = 0xA5;
	testFrame.data[1] = 0x05;
	testFrame.data[2] = 0x19;
	testFrame.data[3] = 0x3B;
	testFrame.data[4] = 0x10; // Distance of 10
	testFrame.data[5] = 0x00;
	testFrame.data[6] = 0x00;
	testFrame.data[7] = 0x00;
	CANNetworkManager::CANNetwork.process_receive_can_message_frame(testFrame);
	CANNetworkManager::CANNetwork.update();
	interfaceUnderTest.update();

	EXPECT_FALSE(valueRequested);

	interfaceUnderTest.set_distance(15);
	interfaceUnderTest.update();

	EXPECT_FALSE(valueRequested);

	interfaceUnderTest.set_distance(16);
	interfaceUnderTest.update();

	EXPECT_TRUE(valueRequested);
	EXPECT_EQ(requestedDDI, 0x3B19);
	valueRequested = false;
	requestedDDI = 0;
	requestedElement = 0;

	// Test same value doesn't re-send the value
	interfaceUnderTest.set_distance(16);
	interfaceUnderTest.update();

	EXPECT_FALSE(valueRequested);

	// Reset
	interfaceUnderTest.test_wrapper_set_state(TaskControllerClient::StateMachineState::Disconnected); // Clear commands
	interfaceUnderTest.test_wrapper_set_state(TaskControllerClient::StateMachineState::Connected); // Arbitrary
	valueRequested = false;
	requestedDDI = 0;
	requestedElement = 0;

	// Request a value change using the public interface
	interfaceUnderTest.on_value_changed_trigger(0x4, 0x3);
	interfaceUnderTest.update();

	EXPECT_EQ(true, valueRequested);
	EXPECT_EQ(requestedDDI, 0x03);
	EXPECT_EQ(requestedElement, 0x4);

	CANHardwareInterface::stop();

	CANNetworkManager::CANNetwork.deactivate_control_function(TestPartnerTC);
	CANNetworkManager::CANNetwork.deactivate_control_function(internalECU);
}

TEST(TASK_CONTROLLER_CLIENT_TESTS, LanguageCommandFallback)
{
	VirtualCANPlugin serverTC;
	serverTC.open();

	CANHardwareInterface::set_number_of_can_channels(1);
	CANHardwareInterface::assign_can_channel_frame_handler(0, std::make_shared<VirtualCANPlugin>());
	CANHardwareInterface::start();

	auto internalECU = test_helpers::claim_internal_control_function(0xFC, 0);
	auto TestPartnerTC = test_helpers::force_claim_partnered_control_function(0xFB, 0);
	auto TestPartnerVT = test_helpers::force_claim_partnered_control_function(0xFA, 0);

	DerivedTestTCClient interfaceUnderTest(TestPartnerTC, internalECU, TestPartnerVT);
	interfaceUnderTest.initialize(false);

	std::this_thread::sleep_for(std::chrono::milliseconds(50));

	// Get the virtual CAN plugin back to a known state
	CANMessageFrame testFrame = {};
	while (!serverTC.get_queue_empty())
	{
		serverTC.read_frame(testFrame);
	}
	ASSERT_TRUE(serverTC.get_queue_empty());

	auto blankDDOP = std::make_shared<DeviceDescriptorObjectPool>();
	interfaceUnderTest.configure(blankDDOP, 1, 32, 32, true, false, true, false, true);

	// Force a status message out of the TC which states it's version 4
	testFrame.identifier = 0x18CBFFFB;
	testFrame.data[0] = 0x10; // Mux
	testFrame.data[1] = 0x04; // Version number (Version 4)
	testFrame.data[2] = 0xFF; // Max boot time (Not available)
	testFrame.data[3] = 0x1F; // Supports all options
	testFrame.data[4] = 0x00; // Reserved options = 0
	testFrame.data[5] = 0x01; // Number of booms for section control (1)
	testFrame.data[6] = 0x20; // Number of sections for section control (32)
	testFrame.data[7] = 0x10; // Number channels for position based control (16)
	CANNetworkManager::CANNetwork.process_receive_can_message_frame(testFrame);
	CANNetworkManager::CANNetwork.update();

	interfaceUnderTest.test_wrapper_set_state(TaskControllerClient::StateMachineState::RequestLanguage);
	ASSERT_EQ(interfaceUnderTest.test_wrapper_get_state(), TaskControllerClient::StateMachineState::RequestLanguage);
	interfaceUnderTest.update();

	serverTC.read_frame(testFrame);

	EXPECT_EQ(testFrame.identifier, 0x18EAFBFC); // Make sure we got the request for language, target the TC

	// Now just sit here and wait for the timeout to occur, 2s
	std::this_thread::sleep_for(std::chrono::milliseconds(2001));
	interfaceUnderTest.update();
	interfaceUnderTest.update();

	// Now we should see another request, this time to the VT
	serverTC.read_frame(testFrame);
	EXPECT_EQ(testFrame.identifier, 0x18EAFAFC); // Make sure we got the request for language, target the VT

	// Now get really crazy and don't respond to that
	std::this_thread::sleep_for(std::chrono::milliseconds(6001));
	interfaceUnderTest.update();

	// Test that we didn't get stuck in the request language state
	EXPECT_EQ(interfaceUnderTest.test_wrapper_get_state(), TaskControllerClient::StateMachineState::ProcessDDOP);
	CANNetworkManager::CANNetwork.deactivate_control_function(TestPartnerTC);
	CANNetworkManager::CANNetwork.deactivate_control_function(TestPartnerVT);
	CANNetworkManager::CANNetwork.deactivate_control_function(internalECU);

	CANHardwareInterface::stop();
	CANNetworkManager::CANNetwork.update();
}

static bool default_process_data_callback(std::uint16_t elementNumber,
                                          std::uint16_t DDI,
                                          TaskControllerClient::DefaultProcessDataSettings &returnedSettings,
                                          void *)
{
	// Handle two specific default process data variables as an example.
	// These are two variables in the bin object, which is element 3 in the object pool.
	if (3 == elementNumber)
	{
		switch (DDI)
		{
			case static_cast<std::uint16_t>(isobus::DataDescriptionIndex::MaximumVolumeContent):
			case static_cast<std::uint16_t>(isobus::DataDescriptionIndex::ActualVolumeContent):
			{
				returnedSettings.timeTriggerInterval_ms = 1000;
				returnedSettings.enableTimeTrigger = true;
				return true;
			}
			break;

			default:
			{
			}
			break;
		}
	}
	return false;
}

TEST(TASK_CONTROLLER_CLIENT_TESTS, DefaultProcessDataTest)
{
	auto ddop = std::make_shared<DeviceDescriptorObjectPool>();
	ddop->set_task_controller_compatibility_level(3);
	ASSERT_TRUE(ddop->deserialize_binary_object_pool(DerivedTestTCClient::testBinaryDDOP, sizeof(DerivedTestTCClient::testBinaryDDOP)));

	VirtualCANPlugin serverTC;
	serverTC.open();

	CANHardwareInterface::set_number_of_can_channels(1);
	CANHardwareInterface::assign_can_channel_frame_handler(0, std::make_shared<VirtualCANPlugin>());
	CANHardwareInterface::start();

	auto internalECU = test_helpers::claim_internal_control_function(0x80, 0);
	auto TestPartnerTC = test_helpers::force_claim_partnered_control_function(0xDF, 0);

	DerivedTestTCClient interfaceUnderTest(TestPartnerTC, internalECU);

	std::this_thread::sleep_for(std::chrono::milliseconds(50));
	interfaceUnderTest.update();

	CANMessageFrame testFrame = {};

	ASSERT_TRUE(internalECU->get_address_valid());
	ASSERT_TRUE(TestPartnerTC->get_address_valid());
	interfaceUnderTest.configure(ddop, 1, 32, 32, true, false, true, false, true);
	interfaceUnderTest.initialize(false);
	interfaceUnderTest.add_default_process_data_requested_callback(default_process_data_callback, &interfaceUnderTest);
	interfaceUnderTest.test_wrapper_set_state(TaskControllerClient::StateMachineState::Connected);

	// Force a status message out of the TC
	testFrame.identifier = (0x18CBFF00 | static_cast<std::uint32_t>(TestPartnerTC->get_address()));
	testFrame.dataLength = CAN_DATA_LENGTH;
	testFrame.data[0] = 0xFE; // Status mux
	testFrame.data[1] = 0xFF; // Element number, set to not available
	testFrame.data[2] = 0xFF; // DDI (N/A)
	testFrame.data[3] = 0xFF; // DDI (N/A)
	testFrame.data[4] = 0x01; // Status (task active)
	testFrame.data[5] = 0x00; // Command address
	testFrame.data[6] = 0x00; // Command
	testFrame.data[7] = 0xFF; // Reserved
	CANNetworkManager::CANNetwork.process_receive_can_message_frame(testFrame);
	CANNetworkManager::CANNetwork.update();

	// Send a request for the default process data DDI
	testFrame.identifier = (0x18CB0000 | (static_cast<std::uint32_t>(internalECU->get_address()) << 8) | static_cast<std::uint32_t>(TestPartnerTC->get_address()));
	testFrame.data[0] = 0x02; // Mux + Element LSNibble
	testFrame.data[1] = 0x00; // Element MSB
	testFrame.data[2] = 0xFF; // DDI
	testFrame.data[3] = 0xDF; // DDI
	testFrame.data[4] = 0x00;
	testFrame.data[5] = 0x00;
	testFrame.data[6] = 0x00;
	testFrame.data[7] = 0x00;
	CANNetworkManager::CANNetwork.process_receive_can_message_frame(testFrame);
	CANNetworkManager::CANNetwork.update();
	interfaceUnderTest.update();

	CANNetworkManager::CANNetwork.deactivate_control_function(TestPartnerTC);
	CANNetworkManager::CANNetwork.deactivate_control_function(internalECU);

	CANHardwareInterface::stop();
	CANNetworkManager::CANNetwork.update();
}
