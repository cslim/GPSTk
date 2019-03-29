#pragma once

#include"boost\asio.hpp"
#include"IdataSource.hpp"

namespace pod
{
	class SerialDataSource:public IdataSource
	{
	public:
		/**
		 * Constructor.
		 * \param port device name, example "/dev/ttyUSB0" or "COM4"
		 * \param baud_rate communication speed, example 9600 or 115200
		 * \throws boost::system::system_error if cannot open the
		 * serial device
		 */
		SerialDataSource(std::string port, unsigned int baud_rate)
			: io(), serial(io, port)
		{
			serial.set_option(boost::asio::serial_port_base::baud_rate(baud_rate));
		}


		/**
		 * Write a string to the serial device.
		 * \param s string to write
		 * \throws boost::system::system_error on failure
		 */
		void writeString(std::string s);

		/**
		 * Blocks until a line is received from the serial device.
		 * Eventual '\n' or '\r\n' characters at the end of the string are removed.
		 * \return a string containing the received line
		 * \throws boost::system::system_error on failure
		 */
		virtual std::string readLine() override;
		
		virtual unsigned char readByte() override;

		virtual unsigned short readWord() override;

		virtual void readBytes(unsigned char *buff, int offset, int len) override;

	private:
		boost::asio::io_service io;
		boost::asio::serial_port serial;
	};
}