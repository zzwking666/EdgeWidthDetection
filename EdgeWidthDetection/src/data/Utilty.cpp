#include "Utilty.hpp"

QString Utility::cameraIp1 = "1";
QString Utility::cameraIp2 = "2";

int ModBusAddress::outPutWidthAddress = 1000;
int ModBusAddress::outPutSpeedAddress = 1002;
int ModBusAddress::readPLCbaojingxinxiAddress = 2000;

std::unordered_map<rw::imgPro::ClassId, rw::imgPro::ClassIdName> ClassId::classIdNameMap = {
	{ClassId::edge,"边缘"}
};

std::vector<rw::imgPro::ClassId> ClassId::classids = {
	ClassId::edge
};