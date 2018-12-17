#pragma once
#include"FilterParameter.h"
#include"RefSatHandler.h"

#include"ARSimple.hpp"
#include"ARMLambda.hpp"

#include<memory>

namespace pod
{
    class AmbiguityHandler
    {
    public:
        AmbiguityHandler(
            const ParametersSet & ambiguites,
            const gpstk::Vector<double> &state,
            const  gpstk::Matrix<double> &cov,
            int n_core);

        ~AmbiguityHandler() {};
        
        template<class T, typename = std::enable_if_t<std::is_base_of<gpstk::ARBase, T>::value>>
        AmbiguityHandler & setArMethod()
        {
            pAR.reset(new T());
            return *this;
        };

        void fixL1L2(gpstk::IRinex& gData);
        
        const gpstk::Vector<double> & CoreParamFixed()
        {
            return coreParamFixed;
        };
        
        const gpstk::Vector<double> & DDFixedAmb()
        {
            return ddFixedAmb;
        };

        const gpstk::Matrix<double> & DDAmbCov()
        {
            return ddAmbCov;
        };

    private:
        
        void storeDDAmbiguities(
            gpstk::IRinex & gData, 
            const gpstk::Vector<double> &ddFixedAmb, 
            const gpstk::SatIDSet &refSVs) const;

        gpstk::Vector<double>  fixDDAmbSeparately(
            const gpstk::Vector<double> & ddAmbFloat,
            const gpstk::Matrix<double> & ddCov) const;

        //pointer to current set of ambiguites
        const ParametersSet *pAmbs;

        //pointer to single difference float solution
        const gpstk::Vector<double> *pSdFloatSolution;

        //pointer to single difference float solution covariance
        const gpstk::Matrix<double> *pSdCov;

        //number of core variables
        int coreNum;

        //pointer to object for integer ambiguities resolution
        std::unique_ptr<gpstk::ARBase> pAR;
        
        //object to choose reference satellites for double difference computation
        RefSatHandler refSatsHandler;
        
        //single diffrerence to double differnce transition matrix
        gpstk::Matrix<double> SD2DD;

        //Core parameters values, obtained by fixed ambiguities
        gpstk::Vector<double> coreParamFixed;

        //Double difference fixed ambiguities
        gpstk::Vector<double> ddFixedAmb;
        
        //Double difference ambiguities covarince
        gpstk::Matrix<double> ddAmbCov;
    };
}

