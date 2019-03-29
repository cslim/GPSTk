#pragma once

namespace pod
{
	class RtcmUtils
	{
	public:

		static unsigned crc24q_hash(unsigned char *data, int len);

		static void crc24q_sign(unsigned char *data, int len);

		static bool crc24q_check(unsigned char *data, int len);

	};
}