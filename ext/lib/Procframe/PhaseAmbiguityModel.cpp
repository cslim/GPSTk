#include"PhaseAmbiguityModel.hpp"

namespace gpstk
{


	// Get element of the state transition matrix Phi
	double PhaseAmbiguityModel::getPhi() const
	{

		// Check if there is a cycle slip
		if (cycleSlip)
		{
			return 0.0;
		}
		else
		{
			return 1.0;
		}

	}  // End of method 'PhaseAmbiguityModel::getPhi()'



	   // Get element of the process noise matrix Q
	double PhaseAmbiguityModel::getQ() const
	{

		// Check if there is a cycle slip
		if (cycleSlip)
		{
			return variance;
		}
		else
		{
			return 0.0;
		}

	}  // End of method 'PhaseAmbiguityModel::getQ()'



	   /* This method checks if a cycle slip happened.
		*
		* @param sat        Satellite.
		* @param data       Object holding the data.
		* @param source     Object holding the source of data.
		*
		*/
	void PhaseAmbiguityModel::checkCS(const SatID& sat,
		SatTypePtrMap& data,
		SourceID& source)
	{

		try
		{

			// By default, assume there is no cycle slip
			setCS(false);

			// Check if satellite is present at this epoch
			if (data.find(sat) == data.end())
			{
				// If satellite is not present, declare CS and exit
				setCS(true);

				return;
			}


			if (!watchSatArc)
			{
				// In this case, we only use cycle slip flags
				// Check if there was a cycle slip
				if (data(sat)(csFlagType) > 0.0)
				{
					setCS(true);
				}

			}
			else
			{
				// Check if this satellite has previous entries
				if (satArcMap[source].find(sat) == satArcMap[source].end())
				{
					// If it doesn't have an entry, insert one
					satArcMap[source][sat] = 0.0;
				};

				// Check if arc number is different than arc number in storage
				if (data(sat)(TypeID::satArc) != satArcMap[source][sat])
				{
					setCS(true);
					satArcMap[source][sat] = data(sat)(TypeID::satArc);
				}

			}
		}
		catch (Exception& e)
		{
			setCS(true);
		}

		return;

	} // End of method 'PhaseAmbiguityModel::checkCS()'


}