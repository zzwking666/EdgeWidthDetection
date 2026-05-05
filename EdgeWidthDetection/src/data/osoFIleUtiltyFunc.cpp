#include "osoFIleUtiltyFunc.hpp"

#include "Utilty.hpp"

rw::imgPro::EliminationInfoFunc::ClassIdWithConfigMap cdm::ScoreConfigConvert::toClassIdWithEliConfigMap(
	double areaFactor, double scoreFactor)
{
	rw::imgPro::EliminationInfoFunc::ClassIdWithConfigMap result{};

	rw::imgPro::EliminationInfoGetConfig configBase;
	configBase.isUsingArea = true;
	configBase.isUsingScore = true;
	configBase.areaFactor = areaFactor;
	configBase.scoreFactor = scoreFactor;
	configBase.areaIsUsingComplementarySet = false;
	configBase.scoreIsUsingComplementarySet = false;

	for (int i = ClassId::minNum; i <= ClassId::maxNum; i++)
	{
		result[i] = configBase;
	}

	result[ClassId::edge].isUsingArea = false;
	result[ClassId::edge].isUsingScore = true;

	return result;
}

rw::imgPro::DefectResultInfoFunc::ClassIdWithConfigMap cdm::ScoreConfigConvert::toClassIdWithDefConfigMap(
)
{
	rw::imgPro::DefectResultInfoFunc::ClassIdWithConfigMap result;
	rw::imgPro::DefectResultInfoFunc::Config configBase;
	configBase.isEnable = false;
	for (int i = ClassId::minNum; i <= ClassId::maxNum; i++)
	{
		result[i] = configBase;
	}

	result[ClassId::edge].isEnable = true;

	return result;
}
