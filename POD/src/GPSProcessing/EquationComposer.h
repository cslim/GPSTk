#pragma once

#include"EquationBase.h"
#include<memory>

namespace pod
{
    typedef std::unique_ptr<EquationBase> eq_uptr;
    typedef std::list<eq_uptr> equationsList;

    class EquationComposer 
    {

    public:
        EquationComposer() 
        {}


        virtual ~EquationComposer() {};

        virtual void Prepare(gpstk::gnssRinex& gData) ;

        virtual void updateH(gpstk::gnssRinex& gData, gpstk::Matrix<double>& H);

        virtual void updatePhi(gpstk::Matrix<double>& Phi) const;

        virtual void updateQ(gpstk::Matrix<double>& Q) const;
        
        virtual void updateR(const gpstk::gnssRinex& gData,  gpstk::Matrix<double>& R);

        virtual void updateMeas(const gpstk::gnssRinex& gData, gpstk::Vector<double>& prefitResiduas);

        virtual void updateKfState(gpstk::Vector<double>& state, gpstk::Matrix<double>& cov) const;

        virtual void storeKfState(const gpstk::Vector<double>& state, const gpstk::Matrix<double>& cov);

        virtual void initKfState(gpstk::Vector<double>& state, gpstk::Matrix<double>& cov) const;

        virtual void saveResiduals(gpstk::gnssRinex& gData, gpstk::TypeID type, gpstk::Vector<double>& postfitResiduals);

        virtual int getNumUnknowns() const;
        
        virtual gpstk::TypeIDSet & currentUnkNowns()
        {
            return currUnknowns;
        }

        virtual gpstk::TypeID & measType()
        {
            return measurmentsType;
        }

        virtual EquationComposer& addEquation(std::unique_ptr<EquationBase> eq)
        {
            equations.push_back(std::move(eq));
            return *this;
        }

        virtual void clear()
        {
            equations.clear();
        }

        //virtual int getNumMeasurments() const ;

    protected: 

        //values and its covariance processed so far
        struct filterData
        {
            // Default constructor initializing the data in the structure
            filterData() : value(0.0) {};
            
            //value
            double value = 0.0;
            std::map<gpstk::TypeID, double> valCov;
        };
        /// Map holding the information regarding every variable
        std::map<gpstk::TypeID, filterData> coreData;

        bool firstTime;

        equationsList equations;

        gpstk::TypeIDSet currUnknowns;

        gpstk::TypeID measurmentsType;
        
        int numUnknowns;

        int numMeas;

    };
    typedef std::shared_ptr<EquationComposer> eqComposer_sptr;
}