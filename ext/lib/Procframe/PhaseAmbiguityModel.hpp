//============================================================================
//
//  This file is part of GPSTk, the GPS Toolkit.
//
//  The GPSTk is free software; you can redistribute it and/or modify
//  it under the terms of the GNU Lesser General Public License as published
//  by the Free Software Foundation; either version 3.0 of the License, or
//  any later version.
//
//  The GPSTk is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with GPSTk; if not, write to the Free Software Foundation,
//  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110, USA
//  
//  Copyright 2004, The University of Texas at Austin
//  Dagoberto Salazar - gAGE ( http://www.gage.es ). 2007, 2008
//
//============================================================================

//============================================================================
//
//This software developed by Applied Research Laboratories at the University of
//Texas at Austin, under contract to an agency or agencies within the U.S. 
//Department of Defense. The U.S. Government retains all rights to use,
//duplicate, distribute, disclose, or release this software. 
//
//Pursuant to DoD Directive 523024 
//
// DISTRIBUTION STATEMENT A: This software has been approved for public 
//                           release, distribution is unlimited.
//
//=============================================================================

/**
 * @file PhaseAmbiguityModel.hpp
     *  This class compute the elements of Phi and Q matrices corresponding
	 *  to a phase ambiguity variable: Constant stochastic model within
	 *  cycle slips and white noise stochastic model when a cycle slip
	 *  happens.
 */

#ifndef GPSTK_PHASEAMBIGUITYMODEL_HPP
#define GPSTK_PHASEAMBIGUITYMODEL_HPP

#include "StochasticModel.hpp"

namespace gpstk
{

	/** This class compute the elements of Phi and Q matrices corresponding
	 *  to a phase ambiguity variable: Constant stochastic model within
	 *  cycle slips and white noise stochastic model when a cycle slip
	 *  happens.
	 *
	 * @sa StochasticModel, ConstantModel, WhiteNoiseModel
	 *
	 * \warning By default, this class expects each satellite to have
	 * 'TypeID::satArc' data inserted in the GNSS Data Structure. Such data
	 * are generated by 'SatArcMarker' objects. Use 'setWatchSatArc()'
	 * method to change this behaviour and use cycle slip flags directly.
	 * By default, the 'TypeID' of the cycle slip flag is 'TypeID::CSL1'.
	 */
	class PhaseAmbiguityModel : public IStochasticModel
	{
	public:


		/** Common constructor
		 *
		 * @param sigma   Standard deviation (sigma) of white noise process
		 *
		 */
		PhaseAmbiguityModel(double sigma = 2e7)
			: variance(sigma*sigma), cycleSlip(false), watchSatArc(true),
			csFlagType(TypeID::CSL1) {};


		/// Set the value of white noise sigma
		virtual PhaseAmbiguityModel& setSigma(double sigma)
		{
			variance = sigma * sigma; return (*this);
		}


		/** Feed the object with information about occurrence of cycle slips.
		 *
		 * @param cs   Boolean indicating if there is a cycle slip at current
		 *             epoch.
		 *
		 */
		virtual PhaseAmbiguityModel& setCS(bool cs)
		{
			cycleSlip = cs; return (*this);
		};

		/** get the information about current cycle slip state.
		*
		*/
		virtual bool  getCS() const
		{
			return cycleSlip;
		};

		/// Set whether satellite arc will be used instead of cycle slip flag
		virtual PhaseAmbiguityModel& setWatchSatArc(bool watchArc)
		{
			watchSatArc = watchArc; return (*this);
		};


		/** This method sets the 'TypeID' of the cycle slip flag to be used.
		 *
		 * @param type       Type of cycle slip flag to be used.
		 *
		 * \warning Method 'setWatchSatArc()' must be set 'false' for this
		 * method to have any effect.
		 */
		virtual PhaseAmbiguityModel& setCycleSlipFlag(const TypeID& type)
		{
			csFlagType = type; return (*this);
		};


		/// Get the 'TypeID' of the cycle slip flag being used.
		virtual TypeID getCycleSlipFlag(void)
		{
			return csFlagType;
		};


		/// Get element of the state transition matrix Phi
		virtual double getPhi() const override;


		/// Get element of the process noise matrix Q
		virtual double getQ()const override;



		/** This method provides the stochastic model with all the available
		 *  information and takes appropriate actions.
		 *
		 * @param sat        Satellite.
		 * @param gData      Data object holding the data.
		 *
		 */
		virtual void Prepare(const SatID& sat,
			IRinex& gData)
		{
			checkCS(sat, gData.getBody(), gData.getHeader().source); return;
		};


		/// Destructor
		virtual ~PhaseAmbiguityModel() {};


	private:


		/// White noise variance
		double variance;

		/// Boolean stating if there is a cycle slip at current epoch
		bool cycleSlip;

		/// Whether satellite arcs will be watched. False by default
		bool watchSatArc;

		/// 'TypeID' of the cycle slip flag being used.
		TypeID csFlagType;

		/// Map holding information regarding every satellite
		std::map<SourceID, std::map<SatID, double> > satArcMap;


		/** This method checks if a cycle slip happened.
		 *
		 * @param sat        Satellite.
		 * @param data       Object holding the data.
		 * @param source     Object holding the source of data.
		 *
		 */
		virtual void checkCS(const SatID& sat,
			SatTypePtrMap& data,
			SourceID& source);


	}; // End of class 'PhaseAmbiguityModel'



}  // End of namespace gpstk
#endif // GPSTK_PHASEAMBIGUITYMODEL_HPP

