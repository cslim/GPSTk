#include"Solution.h"
#include <direct.h>
#include<windows.h>
#include "Shlwapi.h"

#include"FsUtils.h"
#include"ComputeStatistic.h"
using namespace std;
using namespace gpstk;
namespace pod
{
	namespace fs = std::experimental::filesystem;

    Solution::Solution(const char* path) :
        BasicFramework("pod",
                       "discr"),
        // Option initialization. "true" means a mandatory option
        confFile(CommandOption::stdType,
                 'c',
                 "conffile",
                 " [-c|--conffile]    Name of configuration file ('config.txt' by default).",
                 false)
    {
        data.reset(new GnssDataStore(confReader));
        data->LoadData(path);

        solver.setConfigData(data);
    }

    void Solution::process()
    {
        try
        {
            solver.process();
        }
        catch (gpstk::Exception & e)
        {
            cerr << "An exception has occured. Processing stopped." << endl;
            cerr << e.getLocation() << endl;
            cerr << e.getText() << endl;

            GPSTK_RETHROW(e);
        }
    }
    void Solution::saveStatistic()
    {
        auto fName = solver.fileName();
        auto& gMap = solver.getData();

        fs::path dbPath(data->opts.workingDir + "\\" + fName + ".txt");
        TypeIDSet typeSet{ TypeID::recX,TypeID::recY,TypeID::recZ };
        ComputeStatistic st(solver.desiredSlnType(), typeSet);

        Vector<double> sln;
        Matrix<double> covar;
        st.compute(gMap, sln, covar);
        int summ(0);
        double avgSvNum(0);
       
        //calculate number of desired sln types
        for_each(
            gMap.begin(),
            gMap.end(),
            [&](const auto & ep)
        {
            if ((SlnType)(int)ep.second.slnData.getValue(TypeID::recSlnType) == solver.desiredSlnType())
            {
                summ++;
                avgSvNum += ep.second.satData.size();
            }
        }
        );

        //calculate 3D RMS
        double rms3d = sqrt(covar(0, 0) + covar(1, 1) + covar(2, 2));

        ofstream wrt(dbPath.string(), ostream::out | ostream::app);
        string sep = ",";

        //print time interval 
        string fmt = "%04Y-%02m-%02d %02H:%02M:%02S";
        wrt << CivilTime(gMap.getInitialTime()).printf(fmt) << sep << CivilTime(gMap.getFinalTime()).printf(fmt) << sep;
        
        //XYZ coordinates
        for (auto x : sln)
            wrt << std::fixed << std::setw(13) << std::setprecision(4) << std::setfill(' ') << x << sep;
        
        //number of good solutions
        wrt << summ << sep << gMap.size() << sep << std::setprecision(1) << avgSvNum / summ << sep;

        //print rms3d
        wrt << std::fixed << std::scientific << std::setprecision(3) << std::setfill(' ') << rms3d << sep;
        
        //compute corr. matrix
        auto corr = ComputeStatistic::corrMatrix(covar);
        //print std.dev.
        for (size_t i = 0; i < covar.rows(); i++)
            wrt << std::fixed << std::scientific << std::setprecision(4) << std::setfill(' ') << sqrt(covar(i, i)) << sep;

        // print correlation coeff.: xx,xz,yz
        for (size_t i = 0; i < corr.rows(); i++)
            for (size_t j = 0; j < i; j++)
                wrt << std::fixed << std::scientific << std::setprecision(4) << std::setfill(' ') << corr(i, j) << sep;

        wrt << endl;
    }

    void Solution::saveToDb()
    {
        auto fName = solver.fileName();
        auto& gMap = solver.getData();
        gMap.title = fName;
        gMap.updateMetadata();

        fs::path dbPath(data->opts.workingDir + "\\" + fName + ".db");

        //delete curtrent solution database file, if exists
        //string cmd = "del \"" + dbPath.string() + "\"";
        //system(cmd.c_str());

        //insert solution data into DB 
        SQLiteAdapter db(dbPath.string());
        db.addNewFile(gMap);
    }

    void Solution::chekObs()
    {
        data->checkObservable();
    }
}