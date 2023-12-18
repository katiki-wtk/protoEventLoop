#include <iostream>

#include "signalslot.h"

#include "ut_signalslot.h"

using namespace test;


namespace tests
{

struct ExcComInterface
{
    void anotherSlot(int a, double b) {
        std::cout << __FUNCTION__ << a << b << std::endl;
    }


    void differentSlot(int a, int b) {
        std::cout << __FUNCTION__ << a << b<< std::endl;
    }
};


struct PsuInterface {
    void slot1(int a, double b) {
        std::cout << __FUNCTION__ << a << b << std::endl;
    }
};


struct NonTrivialFunctor {

    ~NonTrivialFunctor() {
        std::cout << __FUNCTION__ << "DTOR !!" << std::endl;

    }

    void operator()(int a, double d) {
        std::cout << __FUNCTION__ << "NonTrivialFunctor: " << a << d << std::endl;
    }
};

void simple_function(int a, double b)
{
    std::cout << __FUNCTION__ << ": a=" << a << ", b=" << b << std::endl;
}




void test_pointer_to_member_function()
{
    Signal<int, double> mySignal;

    ExcComInterface rcv;

    mySignal.connect<&ExcComInterface::anotherSlot>(&rcv);
    Connection conn = mySignal.connect<&ExcComInterface::differentSlot>(&rcv);
    mySignal.notify(5, 28.8);
    conn.disconnect();
}


void test_nontrivial_functor()
{
    Signal<int, double> mySignal;
    mySignal.connect(NonTrivialFunctor{});
    mySignal.notify(28, 19.77);

}

void test_global_case_with_multiple_signals_and_slots()
{

    Signal<int, double> mySignal;

    ExcComInterface rcv;

    PsuInterface psu;

    std::cout << "******* Connect Test";
    mySignal.connect<&ExcComInterface::anotherSlot>(&rcv);
    Connection conn = mySignal.connect<&ExcComInterface::differentSlot>(&rcv);
    mySignal.connect<&PsuInterface::slot1>(&psu);
    mySignal.connect(simple_function);

    mySignal.notify(5, 28.8);

    std::cout << "******* Disconnect Test";
    conn.disconnect();
    mySignal.notify(9,99);


    std::cout << "******* Lambda test";


    mySignal.connect([&mySignal](int i, double d) {
        std::cout << "FunctionPointer: i=" << i << ", d=" << d;
        std::cout << "Beware of recursivity....";
    });

    mySignal.connect(NonTrivialFunctor{});

    mySignal.notify(28, 19.77);

    std::cout << "******* Functor Test";
    struct Functor
    {
        void operator()(int i, double d)
        {
            std::cout << "Functor: i=" << i << ", d=" << d;
        }
    };

    Functor functor;
    mySignal.connect(functor);
    mySignal.notify(19, 75);

}

void test_all()
{
    test_pointer_to_member_function();
    test_global_case_with_multiple_signals_and_slots();
    test_nontrivial_functor();
}
}
