#pragma once

#include"Matrix.hpp"
#include"Vector.hpp"
#include<set>

namespace pod
{
	//
	class MatrixExtensions
	{
	public:
		static void removeColumns(gpstk::Matrix<double>& m, std::set<int> cols);

		static void removeRows(gpstk::Matrix<double>& m, std::set<int> rows);

		static void removeElms(gpstk::Vector<double>& v, std::set<int> elms);

	};
}