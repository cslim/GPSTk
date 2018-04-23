#include "CustomSolution.h"

#include"SingleSolution.h"
#include"CdDiffSolution.h"
#include"PdFloatSolution.h"
#include"PPPSolutionBase.h"
#include"PODSolution.h"
#include"PPPSolution.h"

namespace pod
{
    GnssSolution_uptr CustomSolution::Factory(GnssDataStore_sptr dataStore)
    {
        switch (dataStore->opts.slnType)
        {
        case pod::Standalone:
            return std::make_unique<SingleSolution>(dataStore);
        case pod::CODE_DIFF:
            return std::make_unique<CdDiffSolution>(dataStore);
        case pod::PD_Float:
            return std::make_unique<PdFloatSolution>(dataStore);
            break;
        case pod::PD_Fixed:
            break;
        case pod::PPP_Float:
            if (dataStore->opts.isSpaceborneRcv)
                return std::make_unique<PODSolution>(dataStore);
            else
                return std::make_unique<PPPSolution>(dataStore);
            break;
        case pod::PPP_Fixed:
            break;
        default:
            break;
        }
        std::string sExc = "Processing for Solution type " + slnType2Str[dataStore->opts.slnType]+" is not supported.";

        GPSTK_THROW(InvalidRequest(sExc))

    }
    
    CustomSolution::CustomSolution() :GnssSolution(nullptr, .0), ptr(nullptr)
    {
    }

    CustomSolution::CustomSolution(GnssDataStore_sptr dataStore):GnssSolution(nullptr, .0)
    {
        ptr = Factory(dataStore);
    }

    CustomSolution::~CustomSolution()
    {
    }
}
