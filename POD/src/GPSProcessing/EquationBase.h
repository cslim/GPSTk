#pragma once
#include"DataStructures.hpp"
#include"FilterParameter.h"

namespace pod
{
    class EquationBase
    {
    public:
        EquationBase()
        {};
        virtual ~EquationBase() {};
       
        /* update state of equations with new observational data */
        virtual  void Prepare(gpstk::gnssRinex& gData) = 0;
        
        /*Check, if unknown parameters currently observable, if so,
        put the corresponding TypeID into 'TypeIDSet'
        */
        virtual void updateEquationTypes(gpstk::gnssRinex& gData, gpstk::TypeIDSet& eqTypes) = 0;
        
        /* return set of TypeID, corresponding unknown parameters  for given equations */
        virtual  gpstk::TypeIDSet getEquationTypes() const = 0;

        /*return set of  satellite  currently used in processing, 
         return empty 'SatIDSet' by default should be overrided in classes,
         handled satellite-specific parameters (phase ambiguities, for example)
         */
        virtual  ParametersSet getAmbSet() const
        { return ParametersSet(); } ;
        
        /* Put the values in state tarnsition matrix, starting with specific index,
           index will be incremented inside this method
        */
        virtual void updatePhi(gpstk::Matrix<double>& Phi, int& index) const = 0;
        
        /*Put process noise components into corresponding matrix, 
          starting with specific index, index will be incremented inside this method
        */
        virtual void updateQ(gpstk::Matrix<double>& Q, int& index) const = 0;
        
        /* Put default values of state vector and it's covariance into corresponding matrices, 
           starting with specific index, index will be incremented inside this method
         */
        virtual void defStateAndCovariance(gpstk::Vector<double>& x, gpstk::Matrix<double>& P, int& index) const = 0;

        /* Put  partials of the measurements with respect to the unknowns into the design (geometry) matrix 
        starting with specific indices, indices will be incremented inside this method
        */
        virtual void updateH(gpstk::gnssRinex& gData, gpstk::Matrix<double>& H, int& col_0, int& row_0)
        {
        };

        /* return number of unknowns
        */
        virtual int getNumUnknowns() const = 0;

    };
}
