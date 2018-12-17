#ifndef POD_SYNC_OBS_H
#define POD_SYNC_OBS_H

#include"Synchronize.hpp"

namespace pod
{
    class SyncObs : public gpstk::Synchronize
    {
    public:
        SyncObs(const std::list<std::string>& files,
            gpstk::IRinex& roverData,
            const double tol = 1 );

        virtual ~SyncObs();

        //get the const reference to BASE receiver observation header data
        virtual const  gpstk::Rinex3ObsHeader& getRefHeader() const
        { return header; }

        virtual gpstk::IRinex& Process(gpstk::IRinex& gData);

    private:

        //move to next reference station data file
        void toNextFile();
        
        // current reference station data stream
        gpstk::Rinex3ObsStream rin;
        
        //reference station data files
        std::list<std::string> rinFiles;
        
        //eference station file name
        std::string curFile;
        
        // current reference station data header 
        gpstk::Rinex3ObsHeader header;
    };
}
#endif // !POD_SYNC_OBS_H
