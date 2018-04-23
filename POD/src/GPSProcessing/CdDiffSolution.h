#ifndef POD_CDDIFFSOLUTION_H
#define POD_CDDIFFSOLUTION_H

#include "GnssDataStore.hpp"
#include "SingleSolution.h"

namespace pod
{
    class CdDiffSolution : public SingleSolution
    {
    public:
        CdDiffSolution(GnssDataStore_sptr prt);
        virtual ~CdDiffSolution();
        virtual std::string  fileName() const
        {
            return data->SiteBase + "-" + data->SiteRover + "_cd.txt";
        }

        virtual SlnType desiredSlnType() const override
        {
            return SlnType::CODE_DIFF;
        }

        virtual void process() override;

    protected:
        virtual void updateRequaredObs() override;

        void configureSolver() override;

        //code smoothers for Ref. receiver obseravtions
        CodeSmoother2 codeSmootherRef;

    };
}
#endif // !POD_CDDIFFSOLUTION_H
