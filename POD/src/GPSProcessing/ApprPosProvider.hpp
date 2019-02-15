#pragma once 
#include"RinexEpoch.h"
#include"XvtStore.hpp"
#include"Position.hpp"

namespace pod
{
	class IApprPosProvider
	{
	public:
		
		enum PositionSource
		{
			FromConfig= 1,
			ComputeForEachEpoch,
			ComputeForFirstEpoch,
			LoadFromFile,
		};

		static std::map<PositionSource, std::string> posSource2Str;

		static int ComputeApprSol(const gpstk::IRinex & gRin, const gpstk::XvtStore<gpstk::SatID>& Eph, gpstk::Vector<double> & pos);

		IApprPosProvider() = default;
		virtual ~IApprPosProvider() = default;

		virtual int getPosition(const gpstk::IRinex & gRin, gpstk::Position& pos) = 0;

		virtual PositionSource getSource() const = 0;

	};
	typedef std::unique_ptr<IApprPosProvider> posProvider_uptr;

	class ComputeApprPos : public  IApprPosProvider
	{

	public:
		ComputeApprPos(gpstk::XvtStore<gpstk::SatID>& eStore, const gpstk::Position & pos)
			:ephStore(&eStore), prePos(pos)
		{}
		ComputeApprPos(gpstk::XvtStore<gpstk::SatID>& eStore)
			:ephStore(&eStore), prePos(gpstk::Position::CENTER_OF_EARTH)
		{}

		// Inherited via IApprPosProvider
		virtual IApprPosProvider & setNominalPosition(const gpstk::Position & pos)
		{
			prePos = pos;
			return *this;
		}

		virtual gpstk::Position getNominalPosition() const
		{
			return prePos;
		}

		virtual int getPosition(const gpstk::IRinex & gRin, gpstk::Position & pos) override;

		virtual PositionSource getSource() const override
		{
			return PositionSource::ComputeForEachEpoch;
		}

	private:

		gpstk::XvtStore<gpstk::SatID>* ephStore;

		std::map<gpstk::CommonTime, gpstk::Xvt> pvtStore;

		gpstk::Position prePos;

	};

	class ComputeOnePos : public IApprPosProvider
	{
	public:
		ComputeOnePos(gpstk::XvtStore<gpstk::SatID>& eStore)
			:ephStore(&eStore), isFirstTime(true)
		{}

		// Inherited via IApprPosProvider
		virtual int getPosition(const gpstk::IRinex & gRin, gpstk::Position & pos) override;

		virtual PositionSource getSource() const override
		{
			return PositionSource::ComputeForFirstEpoch;
		}

	private:

		gpstk::XvtStore<gpstk::SatID>* ephStore;

		bool isFirstTime;

		gpstk::Position apprPos;
	};

	class PositionFromFile : public IApprPosProvider
	{

	public:

		PositionFromFile(std::string path)
		{
			loadApprPos(path);
		}
		
		int getPosition(const gpstk::IRinex & gRin, gpstk::Position & pos) override;
		
		int size()
		{
			return pvtStore.size();
		}
		virtual PositionSource getSource() const override
		{
			return PositionSource::LoadFromFile;
		}
	private:
		bool loadApprPos(const std::string & path);

		std::map<gpstk::CommonTime, gpstk::Xvt> pvtStore;

	};

	class ApprPosSimple : public IApprPosProvider
	{

	public:
		ApprPosSimple( const gpstk::Position & pos): apprPos(pos)
		{};
		int getPosition(const gpstk::IRinex & gRin, gpstk::Position & pos) override
		{
			pos = apprPos;
			return 0;
		}

		virtual PositionSource getSource() const override
		{
			return PositionSource::FromConfig;
		}

	private :
		gpstk::Position apprPos;
	};

}