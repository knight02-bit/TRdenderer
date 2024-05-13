#include <QApplication>
#include "TRenderer.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    TRenderer w;
    w.show();
    return a.exec();
}
