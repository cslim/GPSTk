#include "AmbiguityHandler.h"
#include"WinUtils.h"

using namespace gpstk;

namespace pod
{
    AmbiguityHandler:: AmbiguityHandler(
        const AmbiguitySet & ambiguites,
        const gpstk::Vector<double> &state,
        const  gpstk::Matrix<double> &cov,
        int n_core)
        :pAmbs(&ambiguites), pSdFloatSolution(&state), pSdCov(&cov), coreNum(n_core),
        pAR(std::make_unique<gpstk::ARMLambda>())
    {
        assert(coreNum + pAmbs->size() == pSdFloatSolution->size());
        assert(pSdCov->rows() == pSdCov->cols());
        assert(pSdFloatSolution->size() == pSdCov->rows());
    };

    void  AmbiguityHandler::fixL1L2(gpstk::gnssRinex& gData)
    {
        //fill the vector of core parameters with float ambiguities
        Vector<double> coreParamsFloat(coreNum, .0);
        for (int k = 0; k < coreNum; k++)
            coreParamsFloat(k) = (*pSdFloatSolution)(k);

        //fill sets of satellite systems and observation types,
        //presented in current set of carrier phase measurements
        SatSystSet ss;
        TypeIDSet types;
        for (const auto &it : *pAmbs)
        {
            ss.insert(it.sv.system);
            types.insert(it.type);
        }
        //total number of single differenses 
        int sd_num = pAmbs->size();
        //total number of double differenses 
        int dd_num = sd_num - ss.size()*types.size();


        Vector<double> DDfloatAmb(dd_num, .0);
        ddAmbCov = Matrix<double>(dd_num, dd_num, .0);
        Matrix<double> parDDAmbCov(coreNum, dd_num, .0);

        Matrix<double> SD2DD(dd_num + coreNum, sd_num + coreNum, .0);

        // core part of DD to SD transition matrix -  identity matrix
        for (int j = 0; j < coreNum; j++)
            SD2DD(j, j) = 1.0;

        //set of reference satellites
        SatIDSet refSVs;
        //ambiguities part of DD to SD transition matrix
        const auto SD2DDamb = refSatsHandler.getSD2DDMatrix(gData, *pAmbs, refSVs);
        // DBOUT_LINE("SD2DDamb\n" << SD2DDamb.diagCopy());

         //put the ambiguities part of DD to SD into main DD to SD transition matrix
        for (int i = 0; i < types.size(); i++)
        {
            for (int j = 0; j < SD2DDamb.rows(); j++)
                for (int k = 0; k < SD2DDamb.cols(); k++)
                {
                    int j1 = j + coreNum + SD2DDamb.rows()*i;
                    int k1 = k + coreNum + SD2DDamb.cols()*i;
                    SD2DD(j1, k1) = SD2DDamb(j, k);
                }
        }

        //DBOUT_LINE("SD2DD\n" << SD2DD);

        //transform float solution and its covarince into DD form
        auto DDfloatSolution = SD2DD*(*pSdFloatSolution);
        auto trSD2DD = transpose(SD2DD);
        auto DDCov = SD2DD*(*pSdCov)*trSD2DD;

        DBOUT_LINE("covDD\n" << DDCov)

            for (int i = 0; i < dd_num; i++)
            {
                //extract float ambiguities DD  
                DDfloatAmb(i) = DDfloatSolution(coreNum + i);

                //extract ambiguities - ambiguities part of DD covarince matrix
                for (int j = 0; j < dd_num; j++)
                    ddAmbCov(i, j) = DDCov(coreNum + i, coreNum + j);

                //extract core - ambiguities part of DD covarince matrix
                for (int k = 0; k < coreNum; k++)
                    parDDAmbCov(k, i) = DDCov(k, coreNum + i);
            }

        //select ambiguities resolution method
        setArMethod<ARMLambda>();

        //resolve the carrier-phas ambiguities as integer
        ddFixedAmb = pAR->resolveIntegerAmbiguity(DDfloatAmb, ddAmbCov);
        //ddFixedAmb = fixDDAmbSeparately(DDfloatAmb, ddAmbCov);
        DBOUT_LINE("float DD amb\n" << DDfloatAmb);
        DBOUT_LINE("fixed DD amb\n" << ddFixedAmb);

        //update core parameters values with integer ambiguities
        coreParamFixed = coreParamsFloat - parDDAmbCov*inverseChol(ddAmbCov)*(DDfloatAmb - ddFixedAmb);


        DBOUT_LINE("float Params\n" << coreParamsFloat);
        DBOUT_LINE("fixed Params\n" << coreParamFixed);
        
        storeDDAmbiguities(gData, ddFixedAmb, refSVs);

    }
    void AmbiguityHandler::storeDDAmbiguities(
        gpstk::gnssRinex & gData,
        const Vector<double> &ddFixedAmb, 
        const gpstk::SatIDSet &refSVs) const
    {
        int i(0);
        for (const auto & amb : *pAmbs)
        {
            auto  &it = gData.body.find(amb.sv);
            if (it != gData.body.end())
            {
                if (refSVs.find(amb.sv) == refSVs.end())
                    gData.body[amb.sv][amb.type] = ddFixedAmb(i++);
                else
                    gData.body[amb.sv][amb.type] = 0.0;
            }
        }
    }
    gpstk::Vector<double>  AmbiguityHandler::fixDDAmbSeparately(
        const gpstk::Vector<double> & ddAmbFloat,
        const gpstk::Matrix<double> & ddCov) const
    {
        Vector<double> ddAmbFixed(ddAmbFloat.size(), .0);

        const auto types = Ambiguity::get_all_types(*pAmbs);
        const auto sv_by_ss = Ambiguity::get_sv_by_ss(*pAmbs);
        int n_sv = pAmbs->size() / types.size();

        int i(0);
        for (const auto &t : types)
            for (const auto &ss : sv_by_ss)
            {

                int currNumDD(ss.second.size() - 1);
                Vector<double> currDdAmbFloat(currNumDD, .0);
                Matrix<double> currDdAmbCov(currNumDD, currNumDD, .0);

                for (size_t k = 0; k < currNumDD; k++)
                {
                    currDdAmbFloat(k) = ddAmbFloat(i + k);
                    for (size_t m = 0; m < currNumDD; m++)
                        currDdAmbCov(k, m) = currDdAmbCov(m, k) = ddCov(i + k, i + m);
                }

                auto currDdAmbFixed = pAR->resolveIntegerAmbiguity(currDdAmbFloat, currDdAmbCov);

                for (size_t k = 0; k < currNumDD; k++)
                    ddAmbFixed(i + k) = currDdAmbFixed(k);
                DBOUT_LINE("ddAmbFixed\n" << ddAmbFixed);
                i += currNumDD;
            }
        return ddAmbFixed;
    }
}
