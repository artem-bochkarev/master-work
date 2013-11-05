#include "qapp.h"
#include <QtWidgets/QApplication>
#include <QtCore/QDir>
#include <QtCore/QFileInfo>

int main(int argc, char *argv[])
{
	//QStringList paths = QCoreApplication::libraryPaths();

	qputenv("QT_QPA_PLATFORM_PLUGIN_PATH", QByteArray((QFileInfo(argv[0]).dir().path() + QDir::separator() + "platforms").toStdString().c_str()));

	//std::string str(QFileInfo(argv[0]).dir().path().toStdString());
	//paths.append(QFileInfo(argv[0]).dir().path() + QDir::separator() + "platforms");
	//QCoreApplication::setLibraryPaths(paths);
    QApplication a(argc, argv);
	//a.addLibraryPath("C:\Qt\qt5\qtbase\plugins\platforms");
    QApp w;
    w.show();
    return a.exec();
}
