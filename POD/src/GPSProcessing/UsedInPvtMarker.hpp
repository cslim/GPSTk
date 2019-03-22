#pragma once

#include"ProcessingClass.hpp"

namespace pod
{
	//Class to mark which satellites is treated as probably useable. 
	//This is doesn't mean that such satellites exactly were used in navigation solution.
	//This flag mean, that such satellites are ponentially can be used 
	//in solution during reprocessing (while forward - backward processing).
	//Only non-zero residuals are indicates that  particular satellite has been used in pvt computation.
	class UsedInPvtMarker: public gpstk::ProcessingClass
	{


	public:
		UsedInPvtMarker(gpstk::TypeID tp) :type(tp) {};
		UsedInPvtMarker() :type(gpstk::TypeID::satStatus){};
		
		~UsedInPvtMarker() {};

		// mark all SV in IRinex as useable in PVT  
		gpstk::IRinex& Process(gpstk::IRinex& gRin) override;
		
		// mark all SV in SatTypePtrMap as useable in PVT  
		gpstk::SatTypePtrMap& markAsUsed(gpstk::SatTypePtrMap& satData) const;
		
		// keep only Satellites marked as useable (TypeID::satStatus)
		gpstk::SatTypePtrMap& keepOnlyUsed(gpstk::SatTypePtrMap& satData) const;
		
		//
		gpstk::SatTypePtrMap& CleanScFlags(gpstk::SatTypePtrMap& satData) const;

		gpstk::SatTypePtrMap& CleanSatArcFlags(gpstk::SatTypePtrMap& satData) const;

		UsedInPvtMarker& updateLastEpoch(const gpstk::IRinex& gRin);

		virtual std::string getClassName(void) const
		{
			return "UsedInPvtMarker";
		}

	private:
		gpstk::TypeID type;
		
		//last epoch time
		gpstk::CommonTime preEpoch;
		//sats in view on last epoch 
		gpstk::SatIDSet preEpochSats;
	};

}