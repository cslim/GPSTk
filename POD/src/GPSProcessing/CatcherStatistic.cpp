#include "CatcherStatistic.h"
#include<iostream>

using namespace std;
using namespace gpstk;

namespace pod
{
    void CatcherStatistic::logStatistic(const std::list<gpstk::ProcessingClass*> & pList) const
    {
        dataTable table;

        for (auto catcher : pList)
        {
            auto cName = catcher->getClassName();

            for (auto&& epoch : catcher->getRejSats())
            {
                table[epoch.first].push_back(dataRecord(cName, epoch.second));
            }

            ofstream f;
            f.open(file);

            for (auto&& it : table)
            {
                f << CivilTime(it.first).asString() << "\t";
                for (auto&& it : it.second)
                {
                    f << it.className << "\t" << it.satRejected.size() << "\t";
                }
                f << endl;
            }
            f.close();
        }
    }
}