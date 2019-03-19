#include "SyncObs.h"

using namespace gpstk;

namespace pod
{
    SyncObs::SyncObs(const std::list<std::string>& files,
        gpstk::IRinex& roverData,
        const double tol)
        :Synchronize(roverData,tol), rinFiles(files)
    {
        //initialize  gpstk::Synchronize object
        Synchronize::setReferenceSource(rin);
        
        //prepare ref. station data stream
        toNextFile();
    }
    
    SyncObs::~SyncObs()
    {
    }
    
    void SyncObs::toNextFile()
    {
        if (rinFiles.size() == 0)
        {
            // If rinFiles is empty now, we issue an exception
            SynchronizeException e("RINEX file list is empty now. ");
            GPSTK_THROW(e);
        }

        //extract firts filename from list
        curFile = rinFiles.front();
        rinFiles.pop_front();

        //open new input stream for ref. station data
        if (rin.is_open())
        {
            rin.close();
            rin.headerRead = false; 
        }
           // rin = Rinex3ObsStream(curFile, std::ios::in);

        rin.open(curFile, std::ios::in);
        rin >> header;
        firstTime = true;
    }

	IRinex&  SyncObs::Process(IRinex& gData)
    {
        if (rin.fail())
        {
            toNextFile();
        }

        while (true)
        {
            try
            {
                Synchronize::Process(gData);
            }
            catch (gpstk::SynchronizeException &e)
            {
                toNextFile();
                continue;
            }
            break;
        }
        return gData;
    }
}