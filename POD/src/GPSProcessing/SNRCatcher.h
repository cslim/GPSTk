#pragma once
#include "ProcessingClass.hpp"
#include<deque>
#include<map>

namespace pod
{


    class SNRCatcher :
        public gpstk::ProcessingClass
    {
    public:

        SNRCatcher();
        //
        SNRCatcher(const gpstk::TypeID & type,
            const gpstk::TypeID& resType,
            double maxgap, 
            double tresh,
            int maxCount);

        virtual ~SNRCatcher() {};

        virtual gpstk::gnssSatTypeValue& Process(gpstk::gnssSatTypeValue& gData)
            throw(gpstk::ProcessingException)
        {
            Process(gData.header.epoch, gData.body); return gData;
        };


        /** Returns a gnnsRinex object, adding the new data generated when
        *  calling this object.
        *
        * @param gData    Data object holding the data.
        */
        virtual gpstk::gnssRinex& Process(gpstk::gnssRinex& gData)
            throw(gpstk::ProcessingException);

        virtual  gpstk::satTypeValueMap& Process(const gpstk::CommonTime& epoch,
            gpstk::satTypeValueMap& gData)
            throw(gpstk::ProcessingException);

        /** Method that implements the LI cycle slip detection algorithm
        *
        * @param epoch     Time of observations.
        * @param sat       SatID.
        * @param tvMap     Data structure of TypeID and values.
        * @param li        Current LI observation value.
        */
        virtual double getDetection(const gpstk::CommonTime& epoch,
            const gpstk::SatID& sat,
            gpstk::typeValueMap& tvMap,
            double snr);

        /// Returns a string identifying this object.
        virtual std::string getClassName(void) const;

    private:
        /// Type of observable.
        gpstk::TypeID obsType;

        /// Types of result.
        gpstk::TypeID resultType1;
        
        /// Maximum buffer size.
        int maxBufferSize;

        /// Minimum size of buffer. It is always set to 5
        static const int minBufferSize;

        /// Maximum interval of time allowed between two successive epochs,
        /// in seconds.
        double deltaTMax;


        /// Saturation threshold to declare cycle slip, in meters.
        double satThreshold;

        struct filterData
        {
            // Default constructor initializing the data in the structure
            filterData()
            {};

            std::deque<gpstk::CommonTime> epochs; ///< Epochs of previous SNR observables.
            std::deque<double> buffer;  ///< Values of previous SNR observables.
        };
        /// Map holding the information regarding every satellite
        std::map<gpstk::SatID, filterData> data;

    };
}

