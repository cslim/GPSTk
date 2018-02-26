#ifndef POD_SYNC_OBS_H
#define POD_SYNC_OBS_H

#include"Synchronize.hpp"

namespace pod
{
    class SyncObs : public gpstk::Synchronize
    {
    public:
        SyncObs(const std::list<std::string>& files,
            gpstk::gnssRinex& roverData,
            const double tol = 1 );

        virtual ~SyncObs();

        //get the const reference to BASE receiver observation header data
        virtual const  gpstk::Rinex3ObsHeader& getRefHeader() const
        { return header; }

        virtual gpstk::gnssRinex& Process(gpstk::gnssRinex& gData);

    private:
        void toNextFile();

        gpstk::Rinex3ObsStream rin;
        
        std::list<std::string> rinFiles;
        
        std::string curFile;

        gpstk::Rinex3ObsHeader header;
    };
}
#endif // !POD_SYNC_OBS_H
