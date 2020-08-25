#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

class Backend : public QObject
{
    Q_OBJECT
public:
    explicit Backend(QObject *parent = nullptr) : QObject(parent) {}

signals:
    void reloadQML();
};


void clearAndReload(QQmlApplicationEngine &engine){
    for(QObject *obj : engine.rootObjects()){
        engine.rootObjects().removeOne(obj);
        obj->deleteLater();
    }
    engine.clearComponentCache();
    engine.load(QUrl::fromLocalFile(QString(QMLSOURCEDIR )+ "/main.qml"));
}

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QGuiApplication app(argc, argv);

    Backend b;

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("Backend", &b);

    QObject::connect(&b, &Backend::reloadQML, [&engine]()->void{clearAndReload(engine);});

    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}

#include "main.moc"
