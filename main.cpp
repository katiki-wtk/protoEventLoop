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
#include "ut_fdtimers.h"

#define TEST_SIGNALSLOT
#define TEST_EVENTLOOP
#define TEST_FDTIMERS

int main(int argc, char *argv[])
{
    qSetMessagePattern("%{time hh:mm:ss.z} -- %{qthreadptr} -- %{category} -- %{message}");

    QApplication a(argc, argv);

#ifdef TEST_FDTIMERS
    ut_fdtimers::test_all();
#endif


#ifdef TEST_SIGNALSLOT
    // tests::test_all();
#endif

#ifdef TEST_EVENTLOOP
    // ut_eventloop::test_all();

/*    w = new MyDialog{};
    w->show();
    a.exec();
    delete w;
*/
#endif

    return 0;
}


