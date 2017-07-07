#ifndef POD_PPP_SOLVER_LEOFWBW_H
#define POD_PPP_SOLVER_LEOFWBW_H

#include "PPPSolverLEO.h"
using namespace gpstk;
namespace POD
{
    class PPPSolverLEOFwBw : public PPPSolverLEO
    {
    public:

        /** Common constructor.
        *
        * @param useNEU   If true, will compute dLat, dLon, dH coordinates;
        *                 if false (the default), will compute dx, dy, dz.
        */
        PPPSolverLEOFwBw(bool useNEU = false);


        /** Returns a reference to a gnnsSatTypeValue object after
        *  solving the previously defined equation system.
        *
        * @param gData    Data object holding the data.
        */
        virtual gnssSatTypeValue& Process(gnssSatTypeValue& gData)
            throw(ProcessingException);


        /** Returns a reference to a gnnsRinex object after solving
        *  the previously defined equation system.
        *
        * @param gData    Data object holding the data.
        */
        virtual gnssRinex& Process(gnssRinex& gData)
            throw(ProcessingException);


        /** Reprocess the data stored during a previous 'Process()' call.
        *
        * @param cycles     Number of forward-backward cycles (1 by default).
        *
        * \warning The minimum number of cycles allowed is "1". In fact, if
        * you introduce a smaller number, 'cycles' will be set to "1".
        */
        virtual void ReProcess(int cycles)
            throw(ProcessingException);


        /** Reprocess the data stored during a previous 'Process()' call.
        *
        * This method will reprocess data trimming satellites whose postfit
        * residual is bigger than the limits indicated by limitsCodeList and
        * limitsPhaseList.
        */
        virtual void ReProcess(void)
            throw(ProcessingException);


        /** Process the data stored during a previous 'ReProcess()' call, one
        *  item at a time, and always in forward mode.
        *
        * @param gData      Data object that will hold the resulting data.
        *
        * @return FALSE when all data is processed, TRUE otherwise.
        */
        virtual bool LastProcess(gnssSatTypeValue& gData)
            throw(ProcessingException);


        /** Process the data stored during a previous 'ReProcess()' call, one
        *  item at a time, and always in forward mode.
        *
        * @param gData      Data object that will hold the resulting data.
        *
        * @return FALSE when all data is processed, TRUE otherwise.
        */
        virtual bool LastProcess(gnssRinex& gData)
            throw(ProcessingException);


        /// Gets the list storing the limits for postfit residuals in code.
        virtual std::list<double> getCodeList(void) const
        {
            return limitsCodeList;
        };


        /** Sets a list storing the limits for postfit residuals in code.
        *
        * @param codeList   List with limits for postfit residuals in code.
        *
        * \warning Limits will be applied in the same order they were added.
        */
        virtual PPPSolverLEOFwBw& setCodeList(std::list<double> codeList)
        {
            limitsCodeList = codeList; return (*this);
        };


        /** Adds a postfit residuals limit to list storing limits for code.
        *
        * @param codeLimit   New limit for postfit residuals in code.
        *
        * \warning Limits will be applied in the same order they were added.
        */
        virtual PPPSolverLEOFwBw& addCodeLimit(double codeLimit)
        {
            limitsCodeList.push_back(codeLimit); return (*this);
        };


        /// Clears the list storing the limits for postfit residuals in code.
        virtual PPPSolverLEOFwBw& clearCodeList(void)
        {
            limitsCodeList.clear(); return (*this);
        };


        /// Gets the list storing the limits for postfit residuals in phase.
        virtual std::list<double> getPhaseList(void) const
        {
            return limitsPhaseList;
        };


        /** Sets a list storing the limits for postfit residuals in phase.
        *
        * @param phaseList   List with limits for postfit residuals in phase.
        *
        * \warning Limits will be applied in the same order they were added.
        */
        virtual PPPSolverLEOFwBw& setPhaseList(std::list<double> phaseList)
        {
            limitsPhaseList = phaseList; return (*this);
        };


        /** Adds a postfit residuals limit to list storing limits for phase.
        *
        * @param phaseLimit   New limit for postfit residuals in phase.
        *
        * \warning Limits will be applied in the same order they were added.
        */
        virtual PPPSolverLEOFwBw& addPhaseLimit(double phaseLimit)
        {
            limitsPhaseList.push_back(phaseLimit); return (*this);
        };


        /// Clears the list storing the limits for postfit residuals in phase.
        virtual PPPSolverLEOFwBw& clearPhaseList(void)
        {
            limitsPhaseList.clear(); return (*this);
        };


        /// Returns the number of processed measurements.
        virtual int getProcessedMeasurements(void) const
        {
            return processedMeasurements;
        };


        /// Returns the number of measurements rejected because they were
        /// off limits.
        virtual int getRejectedMeasurements(void) const
        {
            return rejectedMeasurements;
        };


        /** Sets if a NEU system will be used.
        *
        * @param useNEU  Boolean value indicating if a NEU system will
        *                be used
        *
        */
        virtual PPPSolverLEOFwBw& setNEU(bool useNEU);


        /// Returns a string identifying this object.
        virtual std::string getClassName(void) const;


        /// Destructor.
        virtual ~PPPSolverLEOFwBw() {};


    private:


        /// Boolean indicating if this is the first iteration of this filter.
        bool firstIteration;


        /// List holding the information regarding every observation.
        std::list<gnssRinex> ObsData;


        /// Set storing the TypeID's that we want to keep.
        TypeIDSet keepTypeSet;


        /// Number of processed measurements.
        int processedMeasurements;


        /// Number of measurements rejected because they were off limits.
        int rejectedMeasurements;


        /// List storing the limits for postfit residuals in code.
        std::list<double> limitsCodeList;


        /// List storing the limits for postfit residuals in phase.
        std::list<double> limitsPhaseList;


        /// This method checks the limits and modifies 'gData' accordingly.
        void checkLimits(gnssRinex& gData, double codeLimit, double phaseLimit);


        // Some methods that we want to hide
        virtual int Compute(const Vector<double>& prefitResiduals,
                            const Matrix<double>& designMatrix)
            throw(InvalidSolver)
        {
            return 0;
        };


        virtual PPPSolverLEOFwBw& setDefaultEqDefinition(
            const gnssEquationDefinition& eqDef)
        {
            return (*this);
        };


        virtual PPPSolverLEOFwBw& Reset(const Vector<double>& newState,
                                        const Matrix<double>& newErrorCov)
        {
            return (*this);
        };


    }; // End of class 'PPPSolverLEOFwBw'
}
   //@}

#endif //!POD_PPP_SOLVER_LEOFWBW_H