#include "mainwindow.h"
#include "mystyle.h"
#include "test.h"
#include <QApplication>
#include <QCommandLineParser>
#include <QCommandLineOption>

int main(int argc, char *argv[])
{

    //----------test start-----------
//    Test test;
//    test.test1();
//    return 0;
    //----------test end-----------

    Q_INIT_RESOURCE(resources);

//    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));//置于QApplication实例之前解决乱码问题

//    QApplication::setStyle(QStyleFactory::create("Fusion"));

    QApplication app(argc, argv);

    QCoreApplication::setApplicationName("jookdb");
    QCoreApplication::setApplicationVersion(VERSION_STR);
    QCommandLineParser parser;
    parser.setApplicationDescription(QCoreApplication::applicationName());
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addPositionalArgument("file", "The file to open.");
    parser.process(app);

    if (!parser.positionalArguments().isEmpty()){
        QString filename=parser.positionalArguments().first();
        QObject::connect(MainWindow::instance(),&MainWindow::inited,[=](){
            MainWindow::instance()->loadFile(filename);
        });
    }
    MainWindow::instance()->show();

    return app.exec();
}
