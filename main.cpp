#include "mainwindow.h"

#include <QApplication>
#include <QSurfaceFormat>

int main(int argc, char *argv[])
{
    QSurfaceFormat fmt;
    fmt.setDepthBufferSize(24);
    fmt.setStencilBufferSize(8);
    QSurfaceFormat::setDefaultFormat(fmt);

    QApplication a(argc, argv);
    MainWindow w;

    w.resize(1200, 800);
    w.show();

    // A .scad path on the command line (also what Windows passes when the
    // file type is associated with 3DScad.exe) opens as the current document.
    const QStringList args = QCoreApplication::arguments();
    if (args.size() > 1)
        w.openScadFile(args.at(1));
    return a.exec();
}
