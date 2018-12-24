#pragma once

#include"ProcessingClass.hpp"

namespace pod
{
	class UsedInPvtMarker: public gpstk::ProcessingClass
	{


	public:
		UsedInPvtMarker(gpstk::TypeID tp) :type(tp) {};
		UsedInPvtMarker() :type(gpstk::TypeID::satStatus){};
		
		~UsedInPvtMarker() {};

		gpstk::IRinex& Process(gpstk::IRinex& gRin) override;
		gpstk::SatTypePtrMap& markAsUsed(gpstk::SatTypePtrMap& satData) const;
		gpstk::SatTypePtrMap& keepOnlyUsed(gpstk::SatTypePtrMap& satData) const;
		gpstk::SatTypePtrMap& CleanScFlags(gpstk::SatTypePtrMap& satData) const;
		gpstk::SatTypePtrMap& CleanSatArcFlags(gpstk::SatTypePtrMap& satData) const;

		virtual std::string getClassName(void) const
		{
			return "UsedInPvtMarker";
		}

	private:
		gpstk::TypeID type;
	};

}