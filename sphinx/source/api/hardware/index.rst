.. _API HardwareInterface:

HardwareInterface API
=====================

This guide covers how to connect the library to your hardware in more depth.

.. contents:: Contents
   :depth: 2
   :local:

How the Hardware Interface Works
---------------------------------

The :code:`CANHardwareInterface` class was created to provide a common queuing and thread layer for running the CAN stack and all CAN drivers to simplify integration and crucially to provide a consistent, safe order of operations for all the function calls needed to properly drive the stack.

For example, although receiving and sending CAN messages is multi-threaded, the stack avoids a lot of complexity by making the consumption of the receive message queue and update of protocols effectively single threaded so no expensive mutexing is needed in the core of the stack.

The CANHardwareInterface also is designed to deal with the generic base class for a CAN driver, called :code:`CANHardwarePlugin`, so that it too is completely hardware agnostic.

The hardware interface works as follows at a high level:

* The hardware interface runs a thread that fills a receive queue. This ensures that messages are taken from the hardware or socket in a timely manor, and are saved for processing.
* The hardware interface runs the network manager's main, periodic thread.
	This thread operates in three steps.

	1. The periodic thread drains the receive queue. Messages are processed on this thread by the network manager. Many callbacks to the application happen on this thread.
	2. The main update routine of the network manager is run. This executes all main protocol logic.
	3. The periodic thread tries to send messages out of the transmit queue until a transmit fails.

	The thread then repeats the same steps a few milliseconds later.

.. _choosing-a-can-driver:

Choosing a CAN Driver with CMake
--------------------------------

The library contains built-in hardware integrations for popular CAN interfaces, such as SocketCAN and PEAK.

When compiling with CMake, a default CAN driver plug-in will be selected for you based on your OS, but when compiling you can explicitly choose to use one of the natively supported CAN drivers by supplying the :code:`CAN_DRIVER`` variable.

- :code:`-DCAN_DRIVER=SocketCAN` for Socket CAN support (This is the default for Linux)
- :code:`-DCAN_DRIVER=WindowsPCANBasic` for the windows PEAK PCAN drivers (This is the default for Windows)
- :code:`-DCAN_DRIVER=MacCANPCAN` for the MacCAN PEAK PCAN driver (This is the default for Mac OS)
- :code:`-DCAN_DRIVER=TWAI` for the ESP TWAI driver (This is the preferred ESP32 driver)
- :code:`-DCAN_DRIVER=MCP2515` for the MCP2515 CAN controller
- :code:`-DCAN_DRIVER=WindowsInnoMakerUSB2CAN` for the InnoMaker USB2CAN adapter (Windows)
- :code:`-DCAN_DRIVER=TouCAN` for the Rusoku TouCAN (Windows)
- :code:`-DCAN_DRIVER=SYS_TEC` for a SYS TEC sysWORXX USB CAN adapter (Windows)
- :code:`-DCAN_DRIVER=NTCAN` for the NTCAN driver (Windows)
- :code:`-DCAN_DRIVER=VSCAN` for the VSCAN made by VSCOM (Windows/Linux)

Or specify multiple using a semicolon separated list: :code:`-DCAN_DRIVER="<driver1>;<driver2>"`

If your target hardware is not listed above, you can easily integrate your own hardware by :ref:`implementing a few simple functions <writing-your-own-can-driver>`.

You can include the header file :code:`isobus/hardware_integration/available_can_drivers.hpp` to get access to the CAN drivers that have been included via your CMake configuration.

Using the Hardware Interface
----------------------------

To use the :code:`CANHardwareInterface`, simply create at least one CAN plugin, set the number of CAN channels to be at least 1, assign your driver(s) it to a CAN channel index, and call :code:`start`.

.. code-block:: c++

	std::shared_ptr<isobus::CANHardwarePlugin> canDriver = std::make_shared<isobus::SocketCANInterface>("can0");

	isobus::CANHardwareInterface::set_number_of_can_channels(1);
	isobus::CANHardwareInterface::assign_can_channel_frame_handler(0, canDriver);

	isobus::CANHardwareInterface::start();

You may also want to check the return value of :code:`start` and check the return value of :code:`canDriver->get_is_valid()` to know if everything is working.

You can see this done within our examples `located here. <https://github.com/Open-Agriculture/AgIsoStack-plus-plus/tree/main/examples>`_

.. _writing-your-own-can-driver:

Writing Your Own CAN Driver
----------------------------

If the library does not support your specific CAN hardware, you can easily add support for your hardware by implementing a few functions.

A new CAN driver just needs to inherit from :code:`CANHardwarePlugin` and implement all the functions defined in it (don't worry, there's only 5).

`Here is the class you'll want to inherit from. <https://github.com/Open-Agriculture/AgIsoStack-plus-plus/blob/main/hardware_integration/include/isobus/hardware_integration/can_hardware_plugin.hpp>`_

Then, you can add it to your application and use it just like the built-in plugins.
