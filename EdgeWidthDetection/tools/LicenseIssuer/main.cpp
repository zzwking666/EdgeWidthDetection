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
#include "LicensePolicy.hpp"

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

	if (argc == 4 && std::strcmp(argv[1], "--activation-code") == 0)
	{
		QCoreApplication app(argc, argv);
		const QString expiryText = QString::fromLocal8Bit(argv[3]);
		const QDate expiryDate = expiryText.compare(QStringLiteral("permanent"), Qt::CaseInsensitive) == 0
			? QDate(2099, 12, 31) : QDate::fromString(expiryText, QStringLiteral("yyyy-MM-dd"));
		if (!expiryDate.isValid())
		{
			std::cerr << "有效期应为 yyyy-MM-dd 或 permanent" << std::endl;
			return 2;
		}
		try
		{
			std::cout << DlgLicenseIssuer::generateActivationCodeForMachine(argv[2], expiryDate) << std::endl;
			return 0;
		}
		catch (const std::exception& e)
		{
			std::cerr << "生成激活码失败：" << e.what() << std::endl;
			return 1;
		}
	}

	if (argc == 7 && std::strcmp(argv[1], "--export-policy") == 0)
	{
		QCoreApplication app(argc, argv);
		LicensePolicy::Data data;
		data.firstDeadline = QDate::fromString(QString::fromLocal8Bit(argv[4]), QStringLiteral("yyyy-MM-dd"));
		data.secondDeadline = QDate::fromString(QString::fromLocal8Bit(argv[5]), QStringLiteral("yyyy-MM-dd"));
		data.thirdDeadline = QDate::fromString(QString::fromLocal8Bit(argv[6]), QStringLiteral("yyyy-MM-dd"));
		QString error;
		if (!LicensePolicy::save(QString::fromLocal8Bit(argv[3]), QString::fromLocal8Bit(argv[2]), data, error))
		{
			std::cerr << "导出授权策略失败：" << error.toLocal8Bit().constData() << std::endl;
			return 1;
		}
		std::cout << "授权策略已导出" << std::endl;
		return 0;
	}

	QApplication app(argc, argv);
	DlgLicenseIssuer dlg;
	dlg.show();
	return app.exec();
}
