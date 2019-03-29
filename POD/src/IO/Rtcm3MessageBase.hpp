#pragma once
#include"Rtcm3Decoder.hpp"

namespace pod
{
	class Rtcm3Decoder;
	class Rtcm3MessageBase
	{
	public:
		Rtcm3MessageBase() =delete;
		Rtcm3MessageBase(int msg_id) :id(msg_id){};
		virtual ~Rtcm3MessageBase() = default;
		
		unsigned short getID()
		{
			return id;
		}

		virtual bool parse(unsigned char *buffer, Rtcm3Decoder& decoder) = 0;

#pragma region Comparation support
		bool operator==(const Rtcm3MessageBase& right) const
		{
			return ((id == right.id) && (id == right.id));
		}

		/// operator != for Rtcm3MessageBase
		bool operator!=(const Rtcm3MessageBase& right) const
		{
			return !(operator==(right));
		}

		/// operator < for Rtcm3MessageBase : order by system, then number
		bool operator<(const Rtcm3MessageBase& right) const
		{
			return (id < right.id);
		}

		/// operator > for Rtcm3MessageBase
		bool operator>(const Rtcm3MessageBase& right) const
		{
			return (!operator<(right) && !operator==(right));
		}

		/// operator <= for Rtcm3MessageBase
		bool operator<=(const Rtcm3MessageBase& right) const
		{
			return (operator<(right) || operator==(right));
		}

		/// operator >= for Rtcm3MessageBase
		bool operator>=(const Rtcm3MessageBase& right) const
		{
			return !(operator<(right));
		}
#pragma endregion


	protected:
		unsigned short id;
	};
	class Rtcm3Message1008 :public Rtcm3MessageBase
	{

	public:
		Rtcm3Message1008() :Rtcm3MessageBase(1008) {}

		virtual bool parse(unsigned char *buffer, Rtcm3Decoder& decoder) override;
	};
}
