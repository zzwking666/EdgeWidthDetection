#include "DlgLicenseIssuer.h"

#include <QApplication>
#include <QCoreApplication>
#include <QCryptographicHash>
#include <QString>
#include <algorithm>
#include <cctype>
#include <iostream>
#include <sstream>
#include <string>

#include "hoei_CPUInfo.hpp"
#include "hoei_MotherBoardInfo.hpp"

namespace
{
	constexpr char kProductName[] = "EdgeWidthDetection";

	std::string toUpper(const std::string& str)
	{
		std::string result;
		result.reserve(str.size());
		for (unsigned char ch : str)
		{
			result.push_back(static_cast<char>(std::toupper(ch)));
		}
		return result;
	}

	bool isValidMotherboardUuid(const std::string& uuid)
	{
		if (uuid.empty())
			return false;

		const std::string upper = toUpper(uuid);
		if (upper == "UNKNOWN UUID")
			return false;
		if (upper == "FFFFFFFF-FFFF-FFFF-FFFF-FFFFFFFFFFFF")
			return false;
		if (upper.find("O.E.M") != std::string::npos)
			return false;
		if (upper.find("TO BE FILLED") != std::string::npos)
			return false;

		return true;
	}

	std::string getSystemDiskSerialNumber()
	{
		try
		{
			char buffer[256];
			FILE* pipe = _popen("wmic path Win32_DiskDrive where Index=0 get SerialNumber", "r");
			if (!pipe)
				return {};

			std::string result;
			while (fgets(buffer, sizeof(buffer), pipe) != nullptr)
			{
				result += buffer;
			}
			_pclose(pipe);

			std::istringstream stream(result);
			std::string line;
			std::getline(stream, line);
			std::getline(stream, line);

			line.erase(std::remove_if(line.begin(), line.end(), [](unsigned char ch) {
				return std::isspace(ch) || ch == '.';
			}), line.end());

			return line;
		}
		catch (...)
		{
			return {};
		}
	}

	std::string generateMachineCode()
	{
		std::string mbUuid = rw::hoei::MotherBoardInfoFactory::GetMotherboardUniqueID();
		std::string cpuModel = rw::hoei::CPUInfoFactory::GetCPUModel();
		std::string diskSerial = getSystemDiskSerialNumber();

		if (!isValidMotherboardUuid(mbUuid))
			mbUuid = "UNKNOWN_MB";

		std::string combined = std::string(kProductName) + "|" + mbUuid + "|" + cpuModel + "|" + diskSerial;
		QByteArray data(combined.data(), static_cast<int>(combined.size()));
		return QCryptographicHash::hash(data, QCryptographicHash::Sha256).toHex().toStdString();
	}
}

int main(int argc, char* argv[])
{
	if (argc == 2 && std::strcmp(argv[1], "--machine-code") == 0)
	{
		QCoreApplication app(argc, argv);
		std::cout << generateMachineCode() << std::endl;
		return 0;
	}

	QApplication app(argc, argv);
	DlgLicenseIssuer dlg;
	dlg.show();
	return app.exec();
}
