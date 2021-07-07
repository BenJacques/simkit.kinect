
#include "mainwindow.h"
#include <QApplication>
#include <loguru.hpp>

int main(int argc, char* argv[]) {

    loguru::init(argc, argv);
    // Put every log message in "everything.log":
    loguru::add_file("everything.log", loguru::Append, loguru::Verbosity_MAX);

    // Only log INFO, WARNING, ERROR and FATAL to "latest_readable.log":
    loguru::add_file("latest_readable.log", loguru::Truncate, loguru::Verbosity_INFO);

    // Only show most relevant things on stderr:
    loguru::g_stderr_verbosity = 1;

    QApplication a(argc, argv);
    MainWindow w;
    w.show();  
    return a.exec();
}
