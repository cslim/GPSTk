#include "SNRCatcher.h"
using namespace gpstk;

namespace pod
{

    // Minimum buffer size. It is always set to 5
    const int SNRCatcher::minBufferSize = 5;

    SNRCatcher::SNRCatcher() : obsType(gpstk::TypeID::S1),
        resultType1(gpstk::TypeID::CSL1), resultType2(gpstk::TypeID::CSL2),
        deltaTMax(901.0), satThreshold(15), maxBufferSize(50)
    {};

    gnssRinex& SNRCatcher::Process(gnssRinex& gData)
        throw(ProcessingException)
    {

        try
        {
            Process(gData.header.epoch, gData.body);

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
   satTypeValueMap& SNRCatcher::Process( const CommonTime& epoch,
                                            satTypeValueMap& gData,
                                            const short& epochflag )
      throw(ProcessingException)
   {

      try
      {

         double value1(0.0);

         SatIDSet satRejectedSet;

         for (auto& it: gData)
         {
            try
            {
                  // Try to extract the values
               value1 = it.second(obsType);
            }
            catch(...)
            {
                  // If some value is missing, then schedule this satellite
                  // for removal
               satRejectedSet.insert( it.first );
               continue;
            }


               // If everything is OK, then get the new values inside the
               // structure. This way of computing it allows concatenation of
               // several different cycle slip detectors
            it.second[resultType1] += getDetection( epoch, it.first, it.second, value1);

            if ( it.second[resultType1] > 1.0 )
            {
               it.second[resultType1] = 1.0;
            }

               // We will mark both cycle slip flags
            it.second[resultType2] = it.second[resultType1];

         }

            // Remove satellites with missing data
         gData.removeSatID(satRejectedSet);

         return gData;

      }
      catch(Exception& u)
      {
            // Throw an exception if something unexpected happens
         ProcessingException e( getClassName() + ":"
                                + u.what() );

         GPSTK_THROW(e);

      }

   }  // End of method 'SNRCatcher::Process()'

   double SNRCatcher::getDetection(const gpstk::CommonTime& epoch,
       const gpstk::SatID& sat,
       gpstk::typeValueMap& tvMap,
       const double& snr)
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
           {
               avg += it;
           }
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
