#pragma once

#include"EquationBase.h"
#include"FilterParameter.h"
#include"GnssDataStore.hpp"

#include<memory>


namespace pod
{
    typedef std::unique_ptr<EquationBase> eq_uptr;
    typedef std::list<eq_uptr> equationsList;

    class EquationComposer 
    {

    public:
        ///to map opbservables TypeID to weight factor
        static const std::map<gpstk::TypeID, double> weigthFactors;

        EquationComposer() {};
        EquationComposer(SlnType st): slnType(st) {};
        virtual ~EquationComposer() {};

        ///prepare equations according current data set 'gData'
        virtual void Prepare(gpstk::gnssRinex& gData) ;

        /// compose design matrix
        virtual void updateH(gpstk::gnssRinex& gData, gpstk::Matrix<double>& H);
        
        /// compose state transition matrix
        virtual void updatePhi(gpstk::Matrix<double>& Phi) const;
       
        /// compose process noise matrix
        virtual void updateQ(gpstk::Matrix<double>& Q) const;
        
        /// compose measurments errors matrix
        virtual void updateW(const gpstk::gnssRinex& gData,  gpstk::Matrix<double>& R);

        /// compose vector of measurements (prefit residuals)
        virtual void updateMeas(const gpstk::gnssRinex& gData, gpstk::Vector<double>& prefitResiduas);

        /// compose current state vector and covariance matrix
        virtual void updateKfState(gpstk::Vector<double>& state, gpstk::Matrix<double>& cov) const;

        /// store current state vector and covariance matrix
        virtual void storeKfState(const gpstk::Vector<double>& state, const gpstk::Matrix<double>& cov);
       
        /// compose current state vector and covariance matrix with default values
        virtual void initKfState(gpstk::Vector<double>& state, gpstk::Matrix<double>& cov) const;
        
        /// insert current residuals vector into GNSS data structure
        virtual void saveResiduals(gpstk::gnssRinex& gData, gpstk::Vector<double>& postfitResiduals) const;

        /// get curent number of unknowns
        virtual int getNumUnknowns() const;
        
        /// get current set unknowns TypeID's
        virtual ParametersSet & currentUnknowns()
        {
            return unknowns;
        }

        virtual gpstk::TypeIDSet & measTypes()
        {
            return measurmentsTypes;
        }
        virtual const gpstk::TypeIDSet & measTypes() const
        {
            return measurmentsTypes;
        }

        virtual gpstk::TypeIDSet & residTypes()
        {
            return residualsTypes;
        }

        virtual const gpstk::TypeIDSet & residTypes() const
        {
            return residualsTypes;
        }

        virtual const ParametersSet & currentAmb() const
        {
            return currAmb;
        }
        
        virtual SlnType  getSlnType() const
        {
            return slnType;
        }

        virtual EquationComposer&  setSlnType(SlnType sType)
        {
             slnType = sType;
             return *this;
        }
        
        /// add new equation to equation list
        virtual EquationComposer& addEquation(std::unique_ptr<EquationBase> eq)
        {
            equations.push_back(std::move(eq));
            return *this;
        }
        
        /// erase equation list
        virtual void clearEquations()
        {
            equations.clear();
        }

        /// erase stored data
        virtual void clearData()
        {
            filterData.clear();
        }
        /// menage satellite-specific data 
        virtual void keepOnlySv(const gpstk::SatIDSet& svs);
        virtual void clearSvData(const gpstk::SatIDSet& svs);
        virtual void clearSvData() ;
     

    protected: 

        ///values and its covariance processed so far
        struct FilterData
        {
            // Default constructor initializing the data in the structure
            FilterData() : value(0.0) {};
            
            //value
            double value = 0.0;
            std::map<FilterParameter, double> valCov;
        };

        /// Map holding the information regarding every variable
        std::map<FilterParameter, FilterData> filterData;

        /// is equation system composes first time?
        bool firstTime;

        /// list of equations
        equationsList equations;
        
        /// current set of unknowns
        ParametersSet unknowns;

        /// current set of ambiguities
        ParametersSet currAmb;
        
        /// type of measurements
        gpstk::TypeIDSet measurmentsTypes;

        /// type ID of postfit residuals
        gpstk::TypeIDSet residualsTypes;
        
        /// number of unknowns
        int numUnknowns;

        /// number of measurments
        int numMeas;

        SlnType slnType;

    };
    typedef std::shared_ptr<EquationComposer> eqComposer_sptr;
}