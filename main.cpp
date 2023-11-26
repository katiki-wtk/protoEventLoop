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

    // Premier cas: connexion simple

    auto globalSlotInt = [](int value) {
        qDebug() << "** global slot: value = " << value;

    };

    auto globalSlotPerson = [](std::string name, int age) {
        qDebug() << "** global slot: name: " << QString::fromStdString(name) << ", age: " << age;

    };


    // Exemple 1: Le receiver est forcement de type Recepteur
    {

        Signal<int> monSignal;
        Signal<int, double, std::string> monSignal2;

        Receiver recepteur;

        // Connexion du signal à la méthode de la classe Recepteur
        monSignal.connect(&recepteur, &Receiver::monSlot);
        monSignal2.connect(&recepteur, &Receiver::monSlot);


        // Emission signal
        monSignal.Emit(42);
        monSignal2.Emit(27, 35.5, "Karim");

    }


    // Exemple 2: Recepteur generic...
    {
        struct AnotherReceiver
        {
            void anotherSlot(int value) {
                qDebug() << __FUNCTION__ << "Another Value: " << value;
            }

            void anotherSlot(int age, double money, std::string name) {
                qDebug() << __FUNCTION__ << "Another Name: " << QString::fromStdString(name) << ", Age: " << age << ", Money: " << money;
            }
        };

        EventLoop eventLoop;

        struct ReceiverChild : public Receiver {
            void foo() {}
        };

        GenericSignal<Receiver, int> signal1;
        GenericSignal<AnotherReceiver, int> signal2;
        GenericSignal<AnotherReceiver, int, double, std::string> signal3;


        AnotherReceiver anotherReceiver;
        ReceiverChild receiverChild;
        Receiver receiver;

        signal1.connect(&receiver, &Receiver::monSlot);
        signal1.connect(&receiverChild, &ReceiverChild::monSlot, eventLoop);
        signal2.connect(&anotherReceiver, &AnotherReceiver::anotherSlot);
        signal3.connect(&anotherReceiver, &AnotherReceiver::anotherSlot);


        signal1.Emit(25);
        signal2.Emit(29);
        signal3.Emit(29, 365, "Karim");
    }



    /*
    w = new MyDialog{};
    w->show();
    a.exec();
    delete w;
    */
    return 0;
}


