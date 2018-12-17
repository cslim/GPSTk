#include "CodeSmoother2.h"

using namespace std;
using namespace gpstk;

namespace pod
{
    CodeSmoother2::CodeSmoother2()
    {

    }

    CodeSmoother2::CodeSmoother2( std::list<CodeSmoother>& codeSmoothers, 
        std::list<process_uptr>& csMarkers,
        int interval)
        :window(interval) 
    {
        for(auto& it : csMarkers)
            this->scMarkers.push_back(std::move(it));

        for (auto& it : codeSmoothers)
            this->smoothers.push_back(it);
    }


    CodeSmoother2::~CodeSmoother2()
    {
    }

    CodeSmoother2& CodeSmoother2::addScMarker( gpstk::ProcessingClass& scMarker)
    {
        process_uptr u(&scMarker);
        scMarkers.push_back(std::move(u));
        return *this;
    }

    CodeSmoother2& CodeSmoother2::addScMarker(process_uptr scMarker)
    {
        scMarkers.push_back(std::move(scMarker));
        return *this;
    }

    CodeSmoother2& CodeSmoother2::addSmoother(gpstk::CodeSmoother& smoother)
    {
        smoothers.push_back(smoother);
        return *this;
    }

    gpstk::IRinex& CodeSmoother2::Process(gpstk::IRinex& gData)
    {
         //first, mark cycle slips
        for (auto& it : this->scMarkers)
            gData >> *it;

         //then, smooth pseudoranges
        for (auto& it : smoothers)
            gData >> it;

        return gData;
    }

    CodeSmoother2& CodeSmoother2::setInterval(int interval)
    {
        window = interval;
        for (auto& it : smoothers)
            it.setMaxWindowSize(window);
        return (*this);
    }

}
