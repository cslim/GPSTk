#pragma once
#include"IdataSource.hpp"
#include"RinexEpoch.h"
#include"Rtcm3MessageBase.hpp"
#include"typenames.hpp"

#include<memory>

namespace pod
{

    class Rtcm3MessageBase;
	typedef std::unique_ptr<Rtcm3MessageBase> rtcm3_msg_uptr;
	class Rtcm3Decoder
	{
	public:
		static const uchar PREAMBLE = 0b11010011;
		static const int MAX_MSG_LEN = 1024;

		Rtcm3Decoder(data_source_uptr ptr)
			:source(std::move(ptr)) {}
		~Rtcm3Decoder() = default;

		// start decoding data from source specific
		void run();

		//get
		const std::set<rtcm3_msg_uptr>& messages() const
		{
			return msgsToParse;
		}

		template<typename T>	
		Rtcm3Decoder& addMessage()
		{
			static_assert(std::is_base_of<Rtcm3MessageBase, T>::value);
			msgsToParse.emplace(std::make_unique<T>());
			return *this;
		}

		void setEpochReceivedCallback(void(*func)(gpstk::IRinex&))
		{
			onEpochReceived =func;
		}
		
		const gpstk::IRinex& currEpoch() const
		{
			return currEp;
		}
		
		gpstk::IRinex& currEpoch()
		{
			return currEp;
		}

	private:

		void(*onEpochReceived)(gpstk::IRinex&);
		data_source_uptr source;
		std::set<rtcm3_msg_uptr> msgsToParse;
		gpstk::RinexEpoch currEp;
	};

}
