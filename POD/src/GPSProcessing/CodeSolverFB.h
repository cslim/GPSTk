#pragma once
#include "CodeKalmanSolver.hpp"
namespace pod
{
    class CodeSolverFB :
        public gpstk::CodeKalmanSolver
    {
    public:
        CodeSolverFB();
        virtual ~CodeSolverFB();

    public:  CodeSolverFB& setCodeLims(const std::list<double>& codeLims)
    {
        codeLimits = codeLims;
        return *this;
    }

    public: gpstk::gnssRinex & Process(gpstk::gnssRinex & gRin);

    public: bool lastProcess(gpstk::gnssRinex & gRin);

            //Reprocess the data stored during a previous 'Process()' call.
    public: void reProcess(int numCycles);

            //Reprocess the data stored during a previous 'Process()' call.
    public: void reProcess(void);

            //This method checks the residuals and modifies 'gData' accordingly.
    private: void checkLimits(gpstk::gnssRinex& gData, double codeLimit);

            //This method checks the residuals and assign weights in 'gData' accordingly.
    private: void updateWeights(gpstk::gnssRinex& gData);

#pragma region Fields

            // Number of processed measurements.
    public: int processedMeasurements;

            //Number of measurements rejected because they were off limits.
    public: int rejectedMeasurements;

            // weigh (or not)  measurement based on it's residuals obtained on the previous iteration
    public: bool isWeighByResid;

            //List storing the limits for postfit residuals in code.
    protected: std::list<double> codeLimits;

               //Number of measurements rejected because they were off limits.
    private: std::list<gpstk::gnssRinex> ObsData;

               // is current iteration first
    private: bool firstIteration;
#pragma endregion


    };
}

