#include"SerialDataSource.hpp"

using namespace boost;

namespace pod
{

	unsigned char SerialDataSource::readByte()
	{
		unsigned char c;
		asio::read(serial, asio::buffer(&c, 1));
		return c;
	}
	
	unsigned short SerialDataSource::readWord()
	{
		unsigned short c;
		asio::read(serial, asio::buffer(&c, 2));
		return c;
	}

	void SerialDataSource::readBytes(unsigned char* buff, int offset, int len)
	{
		//unsigned char* newptr = buff + offset;
		asio::read(serial, asio::buffer(buff + offset, len));
	}

	std::string SerialDataSource::readLine()
	{
		//Reading data char by char, code is optimized for simplicity, not speed
		char c;
		std::string result;
		for (;;)
		{
			asio::read(serial, asio::buffer(&c, 1));
			switch (c)
			{
			case '\r':
				break;
			case '\n':
				return result;
			default:
				result += c;
			}
		}
	}

	void SerialDataSource::writeString(std::string s)
	{
		boost::asio::write(serial, boost::asio::buffer(s.c_str(), s.size()));
	}
}