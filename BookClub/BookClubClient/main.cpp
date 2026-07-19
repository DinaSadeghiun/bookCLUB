<<<<<<< HEAD
#include <QGuiApplication>
=======
#include <QApplication>
>>>>>>> feature-signup-roles
#include <QQmlApplicationEngine>

int main(int argc, char *argv[])
{
<<<<<<< HEAD
    QGuiApplication app(argc, argv);
=======
    QApplication app(argc, argv);
>>>>>>> feature-signup-roles

    QQmlApplicationEngine engine;
    const QUrl url(QStringLiteral("qrc:/main.qml"));

    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
                         if (!obj && url == objUrl)
                             QCoreApplication::exit(-1);
                     }, Qt::QueuedConnection);

    engine.load(url);

    return app.exec();
}
