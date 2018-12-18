#ifndef POD_LINEAR_COMBINATIONS_H
#define POD_LINEAR_COMBINATIONS_H
#include<iostream>
#include"DataStructures.hpp"

using namespace gpstk;

namespace pod
{
    class LinearCombination
    {
    public:
        static double getIonoFreeWaveLength(const gpstk::SatID &sv, int band1, int band2);

        LinearCombination() {};
        virtual ~LinearCombination() {};

        void setUseC1(bool useC1)
        {
            this->useC1 = useC1;
        }

        bool getUseC1() const
        {
            return (this->useC1);
        }

        bool getCombination(const SatID & sv, const gnssRinex & gRin, double & value) const
        {
            return getCombination(sv, gRin.body, value);
        }

        bool getCombination(const SatID & sv, const satTypeValueMap & gData, double & value) const
        {
            value = NAN;
            auto tvm = gData.find(sv);
            if (tvm == gData.end()) return false;

            return getCombination(sv, tvm->second, value);
        }

        virtual bool getCombination(const SatID & sv, const typeValueMap & tvMap, double & value) const = 0;

        virtual TypeID getType() const = 0;

    protected:
        bool useC1 = true;
    };

    class MWoubenna :public LinearCombination
    {
    public:
        MWoubenna() {};
       virtual ~MWoubenna() {};
       virtual bool getCombination(const SatID & sv, const typeValueMap & tvMap, double & value) const override;
       virtual TypeID getType() const override;
    };

    class PDelta :public LinearCombination
    {
    public:
        PDelta() {};
        virtual ~PDelta() {};
        virtual bool getCombination(const SatID & sv, const typeValueMap & tvMap, double & value) const override;
        virtual TypeID getType() const override;
    };

    class LDelta :public LinearCombination
    {
    public:
        LDelta() {};
        virtual ~LDelta() {};
        virtual bool getCombination(const SatID & sv, const typeValueMap & tvMap, double & value) const override;
        virtual TypeID getType() const override;
    };

    class PCCombimnation :public LinearCombination
    {
    public:
        PCCombimnation() {};
        virtual ~PCCombimnation() {};
        virtual bool getCombination(const SatID & sv, const typeValueMap & tvMap, double & value) const override;
        virtual TypeID getType() const override;
    };

    class LCCombimnation :public LinearCombination
    {
    public:
        LCCombimnation() {};
        virtual ~LCCombimnation() {};
        virtual bool getCombination(const SatID & sv, const typeValueMap & tvMap, double & value) const override;
        virtual TypeID getType() const override;
    };

    class LICombimnation :public LinearCombination
    {
    public:
        LICombimnation() {};
        virtual ~LICombimnation() {};
        virtual bool getCombination(const SatID & sv, const typeValueMap & tvMap, double & value) const override;
        virtual TypeID getType() const override;
    };

	class PrefitCode :public LinearCombination
	{
	public:
		PrefitCode() = delete;
		PrefitCode(bool isPreciseModel)
			:isPrecise(isPreciseModel)
		{};

	protected:
		bool isPrecise;

	};

    class PrefitC1 :public PrefitCode
    {
    public:
		PrefitC1() = delete;
		PrefitC1(bool isPreciseModel):
			PrefitCode(isPreciseModel)
		{};

        virtual ~PrefitC1() {};
        virtual bool getCombination(const SatID & sv, const typeValueMap & tvMap, double & value) const override;
        virtual TypeID getType() const override;

    };

    class PrefitP1 :public PrefitCode
    {
    public:
		PrefitP1() = delete;
		PrefitP1(bool isPreciseModel) :
			PrefitCode(isPreciseModel)
		{};
        virtual ~PrefitP1() {};
        virtual bool getCombination(const SatID & sv, const typeValueMap & tvMap, double & value) const override;
        virtual TypeID getType() const override;
	protected:
		bool isPrecise;
    };

    class PrefitP2 :public PrefitCode
    {
    public:
		PrefitP2() = delete;
		PrefitP2(bool isPreciseModel) :
			PrefitCode(isPreciseModel)
		{};
        virtual ~PrefitP2() {};
        virtual bool getCombination(const SatID & sv, const typeValueMap & tvMap, double & value) const override;
        virtual TypeID getType() const override;
    };

    class PrefitPC :public PrefitCode
    {
    public:
		PrefitPC() = delete;
		PrefitPC(bool isPreciseModel) :
			PrefitCode(isPreciseModel)
		{};
        virtual ~PrefitPC() {};
        virtual bool getCombination(const SatID & sv, const typeValueMap & tvMap, double & value) const override;
        virtual TypeID getType() const override;
    };

    class PrefitLC :public LinearCombination
    {
    public:
        PrefitLC() {};
        virtual ~PrefitLC() {};
        virtual bool getCombination(const SatID & sv, const typeValueMap & tvMap, double & value) const override;
        virtual TypeID getType() const override;
    };

	class PrefitL1 :public LinearCombination
	{
	public:
		PrefitL1() {};
		virtual ~PrefitL1() {};
		virtual bool getCombination(const SatID & sv, const typeValueMap & tvMap, double & value) const override;
		virtual TypeID getType() const override;
	};

	class PrefitL2 :public LinearCombination
	{
	public:
		PrefitL2() {};
		virtual ~PrefitL2() {};
		virtual bool getCombination(const SatID & sv, const typeValueMap & tvMap, double & value) const override;
		virtual TypeID getType() const override;
	};

    class CodeIonoDelayL1 :public LinearCombination
    {
    public:
        CodeIonoDelayL1() {};
        virtual ~CodeIonoDelayL1() {};
        virtual bool getCombination(const SatID & sv, const typeValueMap & tvMap, double & value) const override;
        virtual TypeID getType() const override;
    };

    class PhaseIonoDelayL1 :public LinearCombination
    {
    public:
        PhaseIonoDelayL1() {};
        virtual ~PhaseIonoDelayL1() {};
        virtual bool getCombination(const SatID & sv, const typeValueMap & tvMap, double & value) const override;
        virtual TypeID getType() const override;
    };
}

#endif // !POD_LINEAR_COMBINATIONS_H