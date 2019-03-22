#include "ITypeValueMap.h"
namespace gpstk
{

	typeValueMap& TypeValueMapPtr::get_value() 
	{
		return *ptr;
	};

	const typeValueMap& TypeValueMapPtr::
		get_value() const
	{
		return *ptr;
	};
}