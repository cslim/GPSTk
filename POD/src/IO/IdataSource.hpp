#pragma once

#include<memory>
#include<string>

namespace pod
{
	class IdataSource
	{
	public:
		virtual std::string readLine() = 0;
		
		virtual unsigned char readByte()  = 0;
		virtual unsigned short readWord() = 0;

		void readBytes(unsigned char *buff, int len) 
		 {
			readBytes(buff, 0, len);
		};

		virtual void readBytes(unsigned char *buff, int offset, int len) = 0;

	};
	typedef std::unique_ptr<IdataSource> data_source_uptr;
}