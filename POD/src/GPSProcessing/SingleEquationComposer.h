#pragma once
#include "EquationComposer.h"
#include"InterSystemBias.h"

namespace pod
{

    class SingleEquationComposer //: public EquationComposer
    {
        
    public:
        SingleEquationComposer() {};
        

        virtual ~SingleEquationComposer() {};


        //virtual  void Prepare(gpstk::gnssRinex& gData) override;

        /** This method will be called in "Process" method of "solvers" objects.
        *  It returns equations, corresponds  solver type and the  observations set, 
        *  which contains in the input gnssRinex object.

        * @param gData    Data object holding the data.
        */


    protected:
        InterSystemBias isbHandler;

    };
}

