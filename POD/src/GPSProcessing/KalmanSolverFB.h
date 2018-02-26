#pragma once
#include "KalmanSolver.h"
#include"ProcessingClass.hpp"

namespace pod
{
    class KalmanSolverFB :
        public gpstk::SolverLMS
    {
    public:
        KalmanSolverFB();
        KalmanSolverFB(eqComposer_sptr eqs);

        virtual ~KalmanSolverFB();

        virtual std::string getClassName() const override
        {
            return "KalmanSolverFB";
        }

        /// Solution
        virtual const  Vector<double>& Solution() const
        {
            return solver.Solution();
        }

        virtual  Vector<double>& Solution()
        {
            return solver.Solution();
        }
     
        /// Postfit-residuals.
        virtual const Vector<double>& PostfitResiduals() const override
        {
            return solver.PostfitResiduals();
        }
        virtual Vector<double>& PostfitResiduals() override
        {
            return solver.PostfitResiduals();
        }

        /// Covariance matrix
        virtual const Matrix<double>& CovMatrix()const
        {
            return solver.CovMatrix();
        }

        virtual Matrix<double>& CovMatrix()
        {
            return solver.CovMatrix();
        }
        double getSolution(const TypeID& type) const override
        {
            return solver.getSolution(type);
        }

        double getVariance(const TypeID& type) const override
        {
            return solver.getVariance(type);
        }
        gpstk::gnssEquationDefinition getDefaultEqDefinition() const override
        {
            return solver.getDefaultEqDefinition();
        }
        KalmanSolverFB& setCodeLims(const std::list<double>& codeLims)
        {
            codeLimits = codeLims;
            return *this;
        }

        gpstk::gnssRinex & Process(gpstk::gnssRinex & gRin);


        bool lastProcess(gpstk::gnssRinex & gRin);

        //Reprocess the data stored during a previous 'Process()' call.
        void reProcess(int numCycles);

        //Reprocess the data stored during a previous 'Process()' call.
        void reProcess(void);

        //This method checks the residuals and modifies 'gData' accordingly.
    private:
        void checkLimits(gpstk::gnssRinex& gData, const TypeID& type, double codeLimit);

#pragma region Fields

        // Number of processed measurements.
    public: int processedMeasurements;

            //Number of measurements rejected because they were off limits.
    public: int rejectedMeasurements;

            //List storing the limits for postfit residuals in code.
    protected: std::list<double> codeLimits;

               //Number of measurements rejected because they were off limits.
    private: std::list<gpstk::gnssRinex> ObsData;

             // is current iteration first
    private: bool firstIteration;

    private: KalmanSolver solver;

#pragma endregion


    };
}

