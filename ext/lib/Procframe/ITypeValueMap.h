#pragma once
#include"typeValueMap.hpp"
#include<memory>
namespace gpstk
{
	class ITypeValueMap
	{

	public:
		virtual ~ITypeValueMap() {};
		virtual typeValueMap& get_value() = 0;
		virtual const typeValueMap& get_value() const = 0;
	};

	class TypeValueMapPtr : public ITypeValueMap
	{
	public:
		TypeValueMapPtr(typeValueMap * p) :ptr(p) {};
		TypeValueMapPtr(typeValueMap & p) :ptr(&p) {};

		virtual typeValueMap& get_value() override;

		virtual const typeValueMap& get_value() const override;
	protected:
		typeValueMap* ptr;
	};
}

