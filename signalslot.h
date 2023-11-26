#ifndef SIGNALSLOT_H
#define SIGNALSLOT_H

#include <QDebug>

#include <iostream>
#include <functional>
#include <iostream>
#include <tuple>
#include <vector>

#include "eventloop.h"

class Receiver;


/*!
 * \brief La classe GenericSignal gere des signaux n'importe quel type de classe
 */
template <typename Class, typename... Args>
class GenericSignal
{
public:
    // Declaration de pointeur de methode membre de Receiver
    using SlotFunction = void (Class::*)(Args...);

    using QueuedSlots = std::tuple<EventLoop *, std::pair<Class*, SlotFunction>>;

    void connect(Class* recepteur, SlotFunction slot) {
        m_slots.push_back(std::make_pair(recepteur, slot));
    }

    void connect(Class *recepteur, SlotFunction slot, EventLoop& eventLoop) {
        m_queuedSlots.push_back(std::make_tuple(&eventLoop, std::make_pair(recepteur, slot)));
    }

    // Notification du signal avec un nombre variable d'arguments
    void Emit(Args... args) {
        for (const auto& qslot : m_queuedSlots) {
            std::get<0>(qslot)->post([qslot, args...]() {
                qDebug() << __FUNCTION__ << "EXECUTION FROM EVENTLOOP:";
                auto& slot = std::get<1>(qslot);
                (slot.first->*slot.second)(args...);

            });
        }

        for (const auto& slot : m_slots) {
            (slot.first->*slot.second)(args...);
        }
    }

private:
    std::vector<std::pair<Class*, SlotFunction>> m_slots;
    std::vector<QueuedSlots> m_queuedSlots;
};



template<typename...Args>
class Signal : public GenericSignal<Receiver, Args...>
{

};


/*!
 * \brief The Recepteur class
 */
class Receiver
{
public:
    Receiver() = default;

    // Méthode de slot générique
    void monSlot(int value) {
        qDebug() << __FUNCTION__ << "Value: " << value;
    }

    void monSlot(int age, double money, std::string name) {
        qDebug() << __FUNCTION__ << "Name: " << QString::fromStdString(name) << ", Age: " << age << ", Money: " << money;
    }

    void monSlot3(std::string ) {

    }

};




#endif // SIGNALSLOT_H
