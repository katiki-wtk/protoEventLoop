#include <unordered_map>
#include "mydialog.h"


#include <QApplication>
#include <QDateTime>
#include <QDebug>
#include <QMutex>
#include <QThread>

static MyDialog * w {nullptr};

#include "signalslot.h"


int main(int argc, char *argv[])
{
    qSetMessagePattern("%{time hh:mm:ss.z} -- %{qthreadptr} -- %{category} -- %{message}");

    QApplication a(argc, argv);

    test::test_signals();

    return 0;

    /*
    w = new MyDialog{};
    w->show();
    a.exec();
    delete w;
    */
    return 0;
}


