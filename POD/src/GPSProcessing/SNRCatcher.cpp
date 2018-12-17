#include "SNRCatcher.h"
using namespace gpstk;

namespace pod
{

    // Minimum buffer size. It is always set to 5
    const int SNRCatcher::minBufferSize = 5;

    SNRCatcher::SNRCatcher() : obsType(TypeID::S1),
        resultType1(gpstk::TypeID::CSL1), deltaTMax(901.0),
        satThreshold(10), maxBufferSize(30)
    {};

    SNRCatcher::SNRCatcher(const TypeID& obsType, 
        const TypeID& resType,
        double maxgap, 
        double tresh, 
        int maxCount)
        : obsType(obsType), resultType1(resType), deltaTMax(maxgap),
        satThreshold(tresh), maxBufferSize(maxCount)
    {};

    IRinex& SNRCatcher::Process(IRinex& gData)
        throw(ProcessingException)
    {
        try
        {
            Process(gData.getHeader().epoch, gData.getBody());
            return gData;
        }
        catch (Exception& u)
        {
            // Throw an exception if something unexpected happens
            ProcessingException e(getClassName() + ":"
                + u.what());

            GPSTK_THROW(e);
        }

    }  // End of method 'SNRCatcher::Process()'

      /* Returns a satTypeValueMap object, adding the new data generated
       *  when calling this object.
       *
       * @param epoch     Time of observations.
       * @param gData     Data object holding the data.
       * @param epochflag Epoch flag.
       */
	SatTypePtrMap& SNRCatcher::Process(const CommonTime& epoch,
        SatTypePtrMap& gData)
        throw(ProcessingException)
    {
        try
        {
            auto & rejTableItem = rejectedSatsTable[epoch];
            double value1(0.0);

            SatIDSet satRejectedSet;
            SatIDSet affectedSatSet;

            for (auto& it : gData)
            {
                try
                {
                    // Try to extract the values
                    value1 = it.second->get_value()(obsType);
                }
                catch (...)
                {
                    // If some value is missing, then schedule this satellite
                    // for removal
                    satRejectedSet.insert(it.first);
                    continue;
                }

                if (getDetection(epoch, it.first, it.second->get_value(), value1) > .0)
                    affectedSatSet.insert(it.first);
            }

            // Remove satellites with missing data
            gData.removeSatID(satRejectedSet);

            rejTableItem.insert(satRejectedSet.begin(), satRejectedSet.end());

            //palliative guard in case of sharp drop SNR for huge amount of observable satellites
            if (affectedSatSet.size() < 3)
                for (auto&& sv : affectedSatSet)
                {
                    rejTableItem.insert(sv);
                    auto & it = gData[sv];
                    it->get_value()[resultType1] += 1.0;
                    if (it->get_value()[resultType1] > 1.0)
                        it->get_value()[resultType1] = 1.0;
                }

            return gData;

        }
        catch (Exception& u)
        {
            // Throw an exception if something unexpected happens
            ProcessingException e(getClassName() + ":"
                + u.what());

            GPSTK_THROW(e);

        }

    }  // End of method 'SNRCatcher::Process()'

   double SNRCatcher::getDetection(const gpstk::CommonTime& epoch,
       const gpstk::SatID& sat,
       gpstk::typeValueMap& tvMap,
       double snr)
   {
       bool reportCS(false);

       // Difference between current and former epochs, in sec
       double currentDeltaT(0.0);
       auto & currData = data[sat];
       size_t s(currData.epochs.size());

       // Get the difference between current epoch and LAST epoch,
       // in seconds, but first test if we have epoch data inside LIData
       if (s > 0)
       {
           currentDeltaT = (epoch - currData.epochs.back());
       }
       else
       {
           // This will yield a very big value
           currentDeltaT = (epoch - CommonTime::BEGINNING_OF_TIME);
       }

       if (currentDeltaT > deltaTMax)
       {
           // We reset buffer with the following lines
           currData.epochs.clear();
           currData.buffer.clear();

           // current buffer size should be updated
           s = currData.epochs.size();

       }
       // Check if we have enough data to start processing.
       if (s >= (size_t)minBufferSize)
       {
           double avg(0);
           for (auto it : currData.buffer)
               avg += it;

           avg /= s;
           if ((avg - snr) > satThreshold /*&& snr < 40*/)
               reportCS = true;
       }
       if (!reportCS)
       {       // Store current epoch at the end of deque
           currData.epochs.push_back(epoch);

           // Store current value of LI at the end of deque
           currData.buffer.push_back(snr);

           // Update current buffer size
           s = currData.epochs.size();
       }

       // Check if we have exceeded maximum window size
       if (s > size_t(maxBufferSize))
       {
           // Get rid of oldest data, which is at the beginning of deque
           currData.epochs.pop_front();
           currData.buffer.pop_front();
       }


       if (reportCS)
       {
           return 1.0;
       }
       else
       {
           return 0.0;
       }
   }

   // Returns a string identifying this object.
   std::string SNRCatcher::getClassName() const
   {
       return "SNRCatcher";
   }
}
