#include "Utilty.hpp"

QString Utility::cameraIp1 = "1";
QString Utility::cameraIp2 = "2";

int ModBusAddress::shicekuanduAddress = 0;
int ModBusAddress::shedingbiaozhunzhiAddress = 2;
int ModBusAddress::changdujiangeAddress = 4;
int ModBusAddress::paizhaochangdujiangeAddress = 6;
int ModBusAddress::bujinyiquanmaichongshuAddress = 8;
int ModBusAddress::luojuAddress = 10;

int ModBusAddress::readPLCbaojingxinxiAddress = 2000;

std::unordered_map<rw::imgPro::ClassId, rw::imgPro::ClassIdName> ClassId::classIdNameMap = {
	{ClassId::edge,"边缘"}
};

std::vector<rw::imgPro::ClassId> ClassId::classids = {
	ClassId::edge
};