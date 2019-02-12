#ifndef POD_GNSSDATA_STORE_H
#define POD_GNSSDATA_STORE_H

#include"CommonTime.hpp"
#include"SP3EphemerisStore.hpp"
#include"IonoModelStore.hpp"
#include"CorrectCodeBiases.hpp"
#include"EOPStore.hpp"
#include"ConfDataReader.hpp"
#include"ComputeIonoModel.hpp"
#include"IonoModelStore.hpp"
#include"IonexStore.hpp"

#include<memory>
#include<string>
#include<map>

namespace pod
{ 
    typedef gpstk::ComputeIonoModel::IonoModelType IonoModelType;
    
    //
    enum CarrierBand
    {
        L1 = 1,
        L2,
        L5
    };
	enum TropoModelType
	{
		Simple = 1,
		SimpleWithGradients,
		Advanced
	};

    // desired type of GNSS solution
    enum SlnType
    {
        Standalone = 1,
        CODE_DIFF,
        PD_Float,
        PD_Fixed,
        PPP_Float=16,
        PPP_Fixed=17,

        NONE_SOLUTION = 0,
    };

    extern std::map<SlnType, std::string>  slnType2Str;
    extern std::map<CarrierBand, std::string> carrierBand2Str;

    //class to store processing configuration and input data  
    struct GnssDataStore
    {
        //GnssDataStore static data initializer
        class Initializer
        {
           
        public:
            Initializer();
        };
        static Initializer GnssDataInitializer;

#pragma region Constructors
    public: GnssDataStore(gpstk::ConfDataReader& confReader) :confReader(&confReader)
    {
    }

    public:  ~GnssDataStore()
    {
    }

#pragma endregion

#pragma region Methods

    public: bool loadApprPos();
    public: void checkObservable();
    public: void LoadData(const char* path);

    private: bool initReader(const char* path);

    private: bool loadIono();
    private: bool loadBceIonoModel();
    private: bool loadIonoMap();

    private: bool loadEphemeris();
    private: bool loadFcn();
    private: bool loadClocks();
    private: bool loadEOPData();
    private: bool loadCodeBiades();

    public: std::list<std::string> getObsFiles(const std::string & siteID) const;

#pragma endregion

#pragma region Fields

             // pointer to  configuration file reader
    public: gpstk::ConfDataReader* confReader;

            //object to handle precise ephemeris and clocks
    public: gpstk::SP3EphemerisStore SP3EphList;

            //Earth orintation parameters store
    public: gpstk::EOPStore eopStore;

            //GPS Navigation Message based ionospheric models store
    public: gpstk::IonoModelStore bceIonoStore;

            //ionosphere map store
    public: gpstk::IonexStore ionexStore;

            // compute the  values related to a given GNSS ionospheric model.
    public: gpstk::ComputeIonoModel ionoCorrector;


            //path to approximate position and code clock bias file
    public: std::string apprPosFile;

            //store of  approximate position and code clock bias 
    public: std::map<gpstk::CommonTime, gpstk::Xvt, std::less<gpstk::CommonTime>> apprPos;

            //class to corrects observables from differential code biases
    public: gpstk::CorrectCodeBiases DCBData;

            // receiver dynamic mode
    public: enum Dynamics
    {
        Static = 0,
        Kinematic,
        RandomWalk,
        Spaceborne,
    };

            //pocessing-spacific options
    public: struct ProcessOpts
    {

        //rover receiver site ID
        std::string SiteRover;

        //base receiver site ID
        std::string SiteBase;

        // directory with generic files
        std::string genericFilesDirectory;

        // working directory
        std::string workingDir;

        // Broadcast ephemeris directory
        std::string bceDir;

        // is output SV-depended data?
        bool fullOutput = false;

        // Estimate zenith troposperic delay?
        bool computeTropo = false;

		// Compute tropospheric delay gradient
		TropoModelType tropoModelType = TropoModelType::Simple;

        // Is data relate to the spacecraft-based receiver?
        bool isSpaceborneRcv = false;

        // Day of year. Used for tropospheric model object  initialization 
        int DoY = 0;

        // Compute approximate position by standalone positioning engin or import it from file
        bool isComputeApprPos = true;

        // Satellite systems used for position computation 
        gpstk::SatSystSet systems;

        // Satellite systems used for position computation 
        std::set<CarrierBand> carrierBands;

        // Use C1 pseudoranges  for position computation instead of P1
        bool useC1 = false;

        // S1 (L1 C/No) threshold for position computation 
        unsigned char maskSNR = 0;

        // Elevation mask (degrees) for position computation
        double maskEl = 10;

        // Receiver dynamic mode
        Dynamics dynamics = Kinematic;

        // Desired type of GNSS solution
        SlnType slnType = SlnType::Standalone;

        // Use carrier-smoothing of code pseudoranges
        bool isSmoothCode = false;

    } opts;

#pragma endregion

    };

    typedef   std::shared_ptr< pod::GnssDataStore> GnssDataStore_sptr;
}
#endif // !POD_GNSSDATA_STORE_H
