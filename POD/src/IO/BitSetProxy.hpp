#pragma once
#include <boost/dynamic_bitset.hpp>
#include "typenames.hpp"
#include <iostream>

namespace pod
{

	class BitSetProxy
	{
	public:
		BitSetProxy() {}
		BitSetProxy(uchar *, int offset, int len );
		~BitSetProxy() = default ;
		
		boost::dynamic_bitset<uchar> & bits()
		{
			return m_bits;
		}

		const boost::dynamic_bitset<uchar> & bits() const
		{
			return m_bits;
		}
		
		uint getUint32(int i0, int len);
		int getInt32(int i0, int len);
		ulong getUInt64(int i0, int len);
		//std::string getString(int i0, int len);

	private:
		void initialize(uchar *, int offset, int len);
		//void initialize(uchar *begin, uchar *end);
		boost::dynamic_bitset<uchar> m_bits;
		boost::dynamic_bitset<uchar>& reverseSelection(int i0, int len);
	};

	std::ostream&  operator<<(std::ostream& i, const BitSetProxy& bitSet);


}