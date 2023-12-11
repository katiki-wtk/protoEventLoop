#include <unordered_map>
#include "mydialog.h"


#include <QApplication>
#include <QDateTime>
#include <QDebug>
#include <QMutex>
#include <QThread>

static MyDialog * w {nullptr};

#include "signalslot.h"

#include <future>
// a non-optimized way of checking for prime numbers:
bool is_prime (int x) {
    std::cout << "is_prime launched" << std::endl;
    for (int i=2; i<x; ++i) if (x%i==0) return false;
    return true;
}

#define TEST_SIGNALSLOT
//#define TEST_EVENTLOOP

int main(int argc, char *argv[])
{
    qSetMessagePattern("%{time hh:mm:ss.z} -- %{qthreadptr} -- %{category} -- %{message}");

    QApplication a(argc, argv);

#ifdef TEST_SIGNALSLOT
    test::test_signals();
#endif

#ifdef TEST_EVENTLOOP
/*    w = new MyDialog{};
    w->show();
    a.exec();
    delete w;
*/

    // call function asynchronously:
    std::future<bool> fut = std::async (is_prime,444444443);

    std::cout << "go" << std::endl;

    bool x = fut.get();     // retrieve return value

    std::cout << "\n444444443 " << (x?"is":"is not") << " prime.\n";

    // do something while waiting for function to set future:
    std::cout << "checking, please wait";
    std::chrono::milliseconds span (100);
    while (fut.wait_for(span)==std::future_status::timeout)
        std::cout << '.' << std::flush;


#endif

    return 0;
}


