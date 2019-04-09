#ifndef CODE_PROC_SV_DATA_H
#define CODE_PROC_SV_DATA_H
#include<map>
#include<set>

#include"Matrix.hpp"
#include"Vector.hpp"
#include"SatID.hpp"
#include"RinexObsID.hpp"
#include"Triple.hpp"
#include"TypeID.hpp"



namespace pod
{
    typedef std::map<gpstk::TypeID, gpstk::RinexObsID> type2ID;
    typedef std::map<gpstk::SatID::SatelliteSystem, type2ID> ObsTypes;
    struct CodeProcSvData
    {
        struct SvDataItem
        {
            bool use;
			gpstk::Triple pos;
			gpstk::Triple alph;
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
        bool tryAdd(const gpstk::SatID & id, const SvDataItem & item);
        
        //
        //try remove item by gpstk::SatID 
        bool tryRemove(const gpstk::SatID & id);
        
        ///
        ///get number of satellies with 'use==true'
        size_t getNumUsedSv() const;
        
        ///
        ///get number of satellies with 'use==true' for given satellie system
        size_t CodeProcSvData::getNumUsedSv(gpstk::SatID::SatelliteSystem sys) const;
        
        ///
        ///get number of parameters for autonomous code position computation 
        size_t getParamNum() const;
        
        ///
        /// update solution vector length, according to constellations,
        /// available for solution computation
        void updateSolutionLength(gpstk::Vector<double> & sol) const;
        
        ///
        ///add system clock corrections to PR residuals vector
        double appendResid(gpstk::Vector<double> & sol, gpstk::SatID::SatelliteSystem sys) const;
        
        ///
        ///forming  a system of equations for autonomous code position computation 
        int getEquations(gpstk::Matrix<double>& P, gpstk::Matrix<double>& W, gpstk::Vector<double>& resid);
        
        ///
        /// set all 'use' memebers to true
        void resetUseFlags(bool newValue = true);
        
        ///
        /// apply C/No mask
        void applyCNoMask(double CNoMask);
       
        ///
        void clear();

		std::set<gpstk::SatID::SatelliteSystem> satSyst;

		std::map<gpstk::SatID, SvDataItem> data;

        friend std::ostream& operator<<(std::ostream& os, const CodeProcSvData& svData);
    };
}

#endif // !CODE_PROC_SV_DATA_H
