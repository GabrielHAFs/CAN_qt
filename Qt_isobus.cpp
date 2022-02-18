#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include "Backend.h"

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    system("ip link set can0 down");

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    system("ip link set can0 up type can bitrate 500000 loopback on");

    QGuiApplication app(argc, argv);
    
    qmlRegisterType<BackEnd>("io.qt.examples.backend", 1, 0, "BackEnd");

    QQmlApplicationEngine engine;
    const QUrl url(QStringLiteral("qrc:/mainwindow.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}

