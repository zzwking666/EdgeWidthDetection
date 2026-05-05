#include "Utilty.hpp"

QString Utility::cameraIp1 = "1";
QString Utility::cameraIp2 = "2";

std::unordered_map<rw::imgPro::ClassId, rw::imgPro::ClassIdName> ClassId::classIdNameMap = {
	{ClassId::edge,"边缘"}
};

std::vector<rw::imgPro::ClassId> ClassId::classids = {
	ClassId::edge
};