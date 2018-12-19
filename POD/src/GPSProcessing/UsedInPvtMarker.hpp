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
		gpstk::SatTypePtrMap& mark(gpstk::SatTypePtrMap& gRin) const;
		gpstk::SatTypePtrMap& keepOnlyUsed(gpstk::SatTypePtrMap& gRin) const;
	private:
		gpstk::TypeID type;
	};

}