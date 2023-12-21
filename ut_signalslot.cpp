#include <iostream>
#include <string>

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

    void slot2(std::string s)
    {

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


/*!
 * \brief simple_function
 */
void simple_function(int a, double b)
{
    std::cout << __FUNCTION__ << ": a=" << a << ", b=" << b << std::endl;
}

void simple_function_2(int a, double& b)
{
    std::cout << __FUNCTION__ << ": a=" << a << ", b=" << b << std::endl;

}



/*!
 * \brief test_simple_function
 */
void test_simple_function()
{
    // Declare signal with respective prototype
    Signal<int, double> mySignal;

    // Connect Signal to its simple_function slot
    mySignal.connect(simple_function);

    // Activate signal
    mySignal.notify(12, 20.23);


    // Activate signal again
    mySignal(12, 23.24);
}


/*!
 * \brief test_pointer_to_member_function
 */
void test_pointer_to_member_function()
{
    Signal<int, double> mySignal;

    ExcComInterface rcv;

    mySignal.connect<&ExcComInterface::anotherSlot>(&rcv);
    Connection conn = mySignal.connect<&ExcComInterface::differentSlot>(&rcv);
    mySignal.notify(5, 28.8);

    // Discounnect 'conn' connection
    conn.disconnect();

    mySignal(99, 30.);
}


void test_nontrivial_functor()
{
    Signal<int, double> mySignal;
    mySignal.connect(NonTrivialFunctor{});
    mySignal.notify(28, 19.77);

    PsuInterface psu;
    Signal<std::string> mySignal2;
    mySignal2.connect<&PsuInterface::slot2>(&psu);

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
    test_simple_function();

    test_pointer_to_member_function();
    test_global_case_with_multiple_signals_and_slots();
    test_nontrivial_functor();
}
}
