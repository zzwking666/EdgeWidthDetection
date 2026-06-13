#include "Utilty.hpp"

QString Utility::cameraIp1 = "2";
QString Utility::cameraIp2 = "3";

int ModBusAddress::shiceyahenkuanduAddress = 0;
int ModBusAddress::shedingyahenbiaozhunkuanduzhiAddress = 0;
int ModBusAddress::paizhaojiangejuliAddress = 0;
int ModBusAddress::daizichangdubiaozhunAddress = 0;
int ModBusAddress::jimiqiyiquanmaichongshuAddress = 0;
int ModBusAddress::jimiqiyiquanzhouchangAddress = 0;
int ModBusAddress::bujinyiquanmaichongshuAddress = 0;
int ModBusAddress::bujinluojuAddress = 0;
int ModBusAddress::daoyidongbuchangAddress = 0;
int ModBusAddress::zidongdaoyidongzuidajuliAddress = 0;
int ModBusAddress::daojiakeyidongdezuidajuliAddress = 0;
int ModBusAddress::daoyidongsudumaichongpinlvAddress = 0;
int ModBusAddress::shibiezhongxindianyutuxiangzhongxindianchazhiAddress = 0;

int ModBusAddress::readPLCbaojingxinxiAddress = 2000;
int ModBusAddress::readPLCkeyipaizhaoxinhaoAddress = 0;
int ModBusAddress::readPLCtingzhipaizhaoxinhaoAddress = 0;
int ModBusAddress::readPLCdaizishicechangduAddress = 0;

std::unordered_map<rw::imgPro::ClassId, rw::imgPro::ClassIdName> ClassId::classIdNameMap = {
	{ClassId::edge,"边缘"}
};

std::vector<rw::imgPro::ClassId> ClassId::classids = {
	ClassId::edge
};