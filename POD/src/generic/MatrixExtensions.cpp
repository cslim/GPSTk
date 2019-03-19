#include"MatrixExtensions.h"

namespace pod
{

	void MatrixExtensions::removeColumns(gpstk::Matrix<double>& m, std::set<int> cols)
	{
		int newCols = m.cols() - cols.size();
		gpstk::Matrix<double> m1(m.rows(), newCols, .0);
		int k = 0;

		for (size_t i = 0; i < m.cols(); i++)
		{
			if (cols.find(i) != cols.end())
				continue;

			for (size_t j = 0; j < m.rows(); j++)
				m1(j, k) = m(j, i);
			k++;
		}
		m = m1;
	}

	void MatrixExtensions::removeRows(gpstk::Matrix<double>& m, std::set<int> rows)
	{
		int newRows = m.rows() - rows.size();
		gpstk::Matrix<double> m1(newRows, m.cols(), .0);
		int k = 0;

		for (size_t i = 0; i < m.rows(); i++)
		{
			if (rows.find(i) != rows.end())
				continue;

			for (size_t j = 0; j < m.cols(); j++)
				m1(k, j) = m(i, j);
			k++;
		}
		m = m1;
	}

	void MatrixExtensions::removeElms(gpstk::Vector<double>& v, std::set<int> elms)
	{
		int newSize = v.size() - elms.size();
		gpstk::Vector<double> v1(newSize, .0);

		int k = 0;
		for (size_t i = 0; i < v.size(); i++)
		{
			if (elms.find(i) != elms.end())
				continue;
			v1(k) = v(i);
			k++;
		}
		v = v1;
	}
}