#ifndef POD_LINEAR_COMB_LIST_H
#define POD_LINEAR_COMB_LIST_H
#include"ProcessingClass.hpp"
#include"DataStructures.hpp"
#include"LinearCombination.h"

#include<list>
#include <memory>
typedef std::unique_ptr<pod::LinearCombination> linear_ptr;

namespace pod
{
    class ProcessLinear : public gpstk::ProcessingClass
    {
    public:
        ProcessLinear() {};
       virtual ~ProcessLinear() {};

       virtual gpstk::gnssSatTypeValue& Process(gpstk::gnssSatTypeValue& gData) override;

       virtual gpstk::gnssRinex& Process(gpstk::gnssRinex& gData) override;

       virtual std::string getClassName(void) const override { return "LinearCombList"; }

       ProcessLinear& clear()
       {
           combs.clear();
           return (*this);
       }
       
       ProcessLinear& add(linear_ptr comb)
       {
           combs.push_back(std::move(comb));
           return (*this);
       }

    protected:
        std::list<linear_ptr> combs;

        gpstk::satTypeValueMap& Process(gpstk::satTypeValueMap& gData) ;

    };
}

#endif // !POD_LINEAR_COMB_LIST_H