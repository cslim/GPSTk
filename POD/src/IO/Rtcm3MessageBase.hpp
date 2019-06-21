#pragma once
#include"Rtcm3Decoder.hpp"
#include"BitSetProxy.hpp"

#include<memory>

namespace pod
{

	class Rtcm3Decoder;
	class Rtcm3MessageBase;
	typedef std::unique_ptr<Rtcm3MessageBase> rtcm3_msg_uptr;
	class Rtcm3MessageBase
	{

	public:

		static std::set<rtcm3_msg_uptr> init_msg_set();
		static std::map<int, rtcm3_msg_uptr> init_msg_map();

		static const std::set<rtcm3_msg_uptr> all_msgs;
		static const std::map<int, rtcm3_msg_uptr> id2msg;


		Rtcm3MessageBase() = delete;
		Rtcm3MessageBase(int msg_id) :id(msg_id) {};

		virtual ~Rtcm3MessageBase() = default;

		unsigned short getID()
		{
			return id;
		}

		virtual rtcm3_msg_uptr clone() const = 0;

		virtual bool parse(BitSetProxy &buffer, Rtcm3Decoder& decoder) = 0;

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


	class Rtcm3_1008 :public Rtcm3MessageBase
	{

	public:
		Rtcm3_1008() :Rtcm3MessageBase(1008) {}
		//Rtcm3_1008(const Rtcm3MessageBase& othr) :Rtcm3MessageBase(1008) {}

		virtual bool parse(BitSetProxy &buffer, Rtcm3Decoder& decoder) override;
		virtual rtcm3_msg_uptr clone() const override { return std::make_unique<Rtcm3_1008>(); }
	};

	class Rtcm3_GpsObs : public Rtcm3MessageBase
	{
	public:
		 Rtcm3_GpsObs(int msg_id) :Rtcm3MessageBase(msg_id) {};
		 bool parseHeader(BitSetProxy &buffer, Rtcm3Decoder& decoder);
		 virtual bool parseSatData(BitSetProxy &buffer, Rtcm3Decoder& decoder) = 0;
	protected:
		double tow;
		uint nSats;
	};

	class Rtcm3_1004 :public Rtcm3_GpsObs
	{
	public:
		Rtcm3_1004() :Rtcm3_GpsObs(1004) {}

		virtual bool parse(BitSetProxy &buffer, Rtcm3Decoder& decoder) override;
		virtual bool parseSatData(BitSetProxy &buffer, Rtcm3Decoder& decoder)  override;
		virtual rtcm3_msg_uptr clone() const override { return std::make_unique<Rtcm3_1004>(); }
	protected:
		
	};
}
