#ifndef CODE_PROCESS_DATA_H
#define CODE_PROCESS_DATA_H
#include<map>
#include<set>

#include"Matrix.hpp"
#include"Vector.hpp"
#include"SatID.hpp"
#include"RinexObsID.hpp"
#include"Triple.hpp"
#include"TypeID.hpp"

using namespace std;
using namespace gpstk;

namespace pod
{
    typedef map<TypeID, RinexObsID> type2ID;
    typedef map<SatID::SatelliteSystem, type2ID> ObsTypes;
    struct CodeProcSvData
    {
        struct SvDataItem
        {
            bool use;
            Triple pos;
            Triple alph;
            double pr = 0, resid = 0, snr = 0, el = 0;
        };
      
    private:
       
        static bool init();

    public:
        static ObsTypes obsTypes;
        static class _init
        {
        public:
            _init();
        } _initializer;

        CodeProcSvData(/*double scrMak = 30, double elMask = 5*/) 
           // :CNoMask(30), elMask(elMask)
        { }

        ///
        ///try add item to data 
        bool tryAdd(const SatID & id, const SvDataItem & item);
        
        //
        //try remove item by SatID 
        bool tryRemove(const SatID & id);
        
        ///
        ///get number of satellies with 'use==true'
        int getNumUsedSv() const;
        
        ///
        ///get number of satellies with 'use==true' for given satellie system
        int CodeProcSvData::getNumUsedSv(SatID::SatelliteSystem sys) const;
        
        ///
        ///get number of parameters for autonomous code position computation 
        int getParamNum() const;
        
        ///
        /// update solution vector length, according to constellations,
        /// available for solution computation
        void updateSolutionLength(Vector<double> & sol) const;
        
        ///
        ///add system clock corrections to PR residuals vector
        double appendResid(Vector<double> & sol, SatID::SatelliteSystem sys) const;
        
        ///
        ///forming  a system of equations for autonomous code position computation 
        int getEquations(Matrix<double>& P, Matrix<double>& W, Vector<double>& resid);
        
        ///
        /// set all 'use' memebers to true
        void resetUseFlags(bool newValue = true);
        
        ///
        /// apply C/No mask
        void applyCNoMask(double CNoMask);
       
        ///
        void clear();

        //double CNoMask, elMask;
        set<SatID::SatelliteSystem> satSyst;

        map<SatID, SvDataItem> data;

        friend std::ostream& operator<<(std::ostream& os, const CodeProcSvData& svData);
    };
}

#endif // !CODE_PROCESS_DATA_H
