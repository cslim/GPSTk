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
       void setUseC1(bool useC1);

       bool getUseC1() const
       { return useC1; }

       virtual gpstk::IRinex& Process(gpstk::IRinex& gData) override;

       virtual std::string getClassName(void) const override { return "ProcessLinear"; }

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

        gpstk::SatTypePtrMap& Process(gpstk::SatTypePtrMap& gData) ;

        bool useC1;

    };
}

#endif // !POD_LINEAR_COMB_LIST_H