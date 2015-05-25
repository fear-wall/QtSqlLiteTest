#include "QtSqlLiteTest.h"
#include <QApplication>
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QtSqlLiteTest w;
    w.show();

    return a.exec();
}
