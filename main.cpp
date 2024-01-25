#include <unordered_map>
#include "mydialog.h"
#include <type_traits>

#include <QApplication>
#include <QDateTime>
#include <QDebug>
#include <QMutex>
#include <QThread>

static MyDialog * w {nullptr};

#include "ut_eventloop.h"
#include "ut_signalslot.h"

//#define TEST_SIGNALSLOT
//#define TEST_EVENTLOOP

int main(int argc, char *argv[])
{
    qSetMessagePattern("%{time hh:mm:ss.z} -- %{qthreadptr} -- %{category} -- %{message}");

    QApplication a(argc, argv);

#ifdef TEST_SIGNALSLOT
    tests::test_all();
#endif

#ifdef TEST_EVENTLOOP
    ut_eventloop::test_all();




#endif
       w = new MyDialog{};
    w->show();
    a.exec();
    delete w;

    return 0;
}


