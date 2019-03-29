#include"BitSetProxy.hpp"
#include <algorithm>    // std::swap
#include <boost/dynamic_bitset.hpp>
#include<iostream>
namespace pod
{
	BitSetProxy::BitSetProxy(uchar * inp, int offset, int len)
	{
		initialize(inp, offset, len);
	}
	


	void BitSetProxy::initialize(uchar * inp, int offset, int len)
	{
		m_bits.resize(CHAR_BIT*len);
		for (int i = 0; i < len; ++i)
		{
			uchar cur = inp[i + offset];
			int offset = (i + 1)* CHAR_BIT;

			for (int bit = 0; bit < CHAR_BIT; ++bit)
			{
				m_bits[--offset] = cur & 1;
				//++offset;   // Move to next bit in b
				cur >>= 1;  // Move to next bit in array
			}
		}
	}

	boost::dynamic_bitset<uchar>& BitSetProxy::
		reverseSelection(int i0, int len)
	{
		auto sel = m_bits >> i0;

		sel.resize(len);

		for (int i = 0; i < sel.size() / 2; i++)
		{
			bool temp = sel[i];
			sel[i] = sel[sel.size() - i - 1];
			sel[sel.size() - i - 1] = temp;
		}

		return sel;

	}

	uint  BitSetProxy::getUint32(int i0, int len)
	{
		auto sel = reverseSelection(i0, len);
		uint mask = 1u; 
		uint res = 0u;

		for (int i = 0; i < sel.size(); i++)
		{
			if (sel[i])
				res |= mask;
			mask <<= 1;
		}
		return res;
	}

	int BitSetProxy::getInt32(int i0, int len)
	{

		auto sel = reverseSelection(i0, len);
		uint mask = 1u;
		uint res = 0u;
		for (int i = 0; i < sel.size() - 1; i++)
		{
			if (sel[i])
				res |= mask;

			mask <<= 1;
		}
		return sel[sel.size() - 1] ? -res : res;
	}

	ulong BitSetProxy::getUInt64(int i0, int len)
	{
		auto sel = reverseSelection(i0, len);
		ulong mask = 1ul;
		ulong res = 0ul;
		for (int i = 0; i < sel.size(); i++)
		{
			if (sel[i])
				res |= mask;

			mask <<= 1;
		}
		return res;
	}

	//std::string getString(int i0, int len)
	//{

	//}

	std::ostream& operator<<(std::ostream& o, const BitSetProxy& bitSet)
	{
		for (int i = 0; i < bitSet.bits().size(); i++)
		{
			if (i % 8 == 0)std::cout << ' ';
			o << bitSet.bits()[i];
		}
		return o;
	}

}