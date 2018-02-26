#pragma once
#include "CodeSmoother.hpp"
#include<memory>


namespace pod
{
    typedef std::unique_ptr<gpstk::ProcessingClass> process_uptr;
   
    class CodeSmoother2 : public gpstk::ProcessingClass
    {
    public:

        CodeSmoother2();

        CodeSmoother2(std::list<gpstk::CodeSmoother>& smoothers, std::list<process_uptr>& markers, int interval = 600);

        virtual ~CodeSmoother2();

        virtual CodeSmoother2& addScMarker(gpstk::ProcessingClass& scMarker);

        virtual CodeSmoother2& addScMarker(process_uptr scMarker);

        virtual CodeSmoother2& addSmoother(gpstk::CodeSmoother& smoother);
        
        virtual gpstk::gnssSatTypeValue& Process(gpstk::gnssSatTypeValue& gData) override;

        virtual gpstk::gnssRinex& Process(gpstk::gnssRinex& gData) override;
 
        virtual std::string getClassName(void) const override
        {
            return "CodeSmoother2";
        }

        virtual int getInterval() const
        {
            return window;
        }

        virtual CodeSmoother2& setInterval(int interval);
        

    private:
        //
        std::list<gpstk::CodeSmoother> smoothers;
        std::list<process_uptr> scMarkers;

        // smoothing window in samples
        int window ;
    };
}

