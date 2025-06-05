#include <QApplication>
#include "MainWindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    app.setApplicationName("WordStar Editor");
    app.setApplicationVersion("1.0");
    app.setOrganizationName("Your Organization");
    
    MainWindow window;
    window.show();
    
    return app.exec();
}