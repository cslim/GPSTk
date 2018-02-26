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
    public: virtual std::string  fileName() const
    {
        return data->SiteBase+ "-"+ data->SiteRover + "_cd.txt";
    }
    public: virtual void process() override;

    protected: virtual void updateRequaredObs() override;

    protected: void configureSolver() override;

               //code smoothers for Ref. receiver obseravtions
    protected: CodeSmoother2 codeSmootherRef;

    //protected: gpstk::Position refPos;

    };
}
#endif // !POD_CDDIFFSOLUTION_H
