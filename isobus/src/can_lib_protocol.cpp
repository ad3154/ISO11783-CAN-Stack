#include "can_lib_protocol.hpp"

#include <algorithm>

namespace isobus
{
	std::vector<CANLibProtocol*> CANLibProtocol::protocolList;

	CANLibProtocol::CANLibProtocol() :
	  initialized(false)
	{
		protocolList.push_back(this);
	}

	CANLibProtocol::~CANLibProtocol()
	{
		auto protocolLocation = find(protocolList.begin(), protocolList.end(), this);

		if (protocolList.end() != protocolLocation)
		{
			protocolList.erase(protocolLocation);
		}
	}

	bool CANLibProtocol::get_is_initialized() const
	{
		return initialized;
	}

	bool CANLibProtocol::get_protocol(std::uint32_t index, CANLibProtocol *&returnedProtocol)
	{
		returnedProtocol = nullptr;

		if (index < protocolList.size())
		{
			returnedProtocol = protocolList[index];
		}
		return (nullptr != returnedProtocol);
	}

	std::uint32_t CANLibProtocol::get_number_protocols()
	{
		return protocolList.size();
	}

	void CANLibProtocol::initialize(CANLibBadge<CANNetworkManager>)
	{
		initialized = true;
	}

} // namespace isobus