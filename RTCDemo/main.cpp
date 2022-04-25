#include "mainwindow.h"
#include <QApplication>
#include <QLocale>
#include <QTranslator>
#include <winsock.h>

bool running = true;
bool DisableMMCSS = false;
MainWindow* g_mainWindow = nullptr;

int main(int argc, char *argv[])
{
#ifdef _DEBUG
    AllocConsole();
    FILE* file;
    freopen_s(&file, "CONOUT$", "w", stdout);
    freopen_s(&file, "CONOUT$", "w", stderr);
#endif

    WSADATA wsaData;
    int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0)
    {
        qFatal("WSAStartup failed: %d\n", iResult);
    }

    QApplication a(argc, argv);
    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = "RTCDemo_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName)) {
            a.installTranslator(&translator);
            break;
        }
    }

    MainWindow w;
    w.show();
    auto result = a.exec();
    WSACleanup();
    return result;
}
