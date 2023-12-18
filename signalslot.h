#ifndef SIGNALSLOT_H
#define SIGNALSLOT_H

#include <QDebug>

#include <iostream>
#include <functional>
#include <iostream>
#include <type_traits>
#include <tuple>
#include <vector>

namespace test
{

struct ConnectionInfo;

/*!
 *
 *  NOTE: Un Signal peut etre utilise dans plusieurs Connection !
 *
 */

struct SignalBase
{
    struct Slot
    {
        void *obj {nullptr};
        void *func {nullptr};
    };

    struct ConnectionSlot {
        ConnectionInfo * conn;
        Slot slot;
    };

    SignalBase() = default;
    ~SignalBase();
    SignalBase(SignalBase&& other) noexcept;
    SignalBase& operator= (SignalBase&& other) noexcept;

    SignalBase(const SignalBase&) = delete;
    SignalBase& operator= (const SignalBase&) = delete;

    // Declare mutable members to "constify" child class methods
    mutable std::vector<ConnectionSlot> m_connections;
    mutable bool m_dirty {false};
    mutable bool m_calling {false};
};


/*!
 * \brief The ConnectionInfo class
 */
struct ConnectionInfo
{
    const SignalBase * m_signal {nullptr};
    int m_index{0};
    bool m_used {false};

    ConnectionInfo(const SignalBase *sig, int idx)
        : m_signal(sig)
        , m_index(idx)
    {}

    virtual ~ConnectionInfo()
    {
        // Destruction de la connection:
        // Il faut signaler au signal que cette connection n'existe plus
        // TODO: On pourrait passer des shared_ptr/weak_ptr...a voir ?
        if (m_signal) {
            m_signal->m_connections[m_index].conn = nullptr;
            m_signal->m_connections[m_index].slot.obj = nullptr;
            m_signal->m_connections[m_index].slot.func = nullptr;
            m_signal->m_dirty = true;
        }
    }
};


/*!
 * \brief The NonTrivialBasicConnection class is used for connection
 *  that involves a functor non-trivially destructibles and built with placement "new"
 */
template <typename T>
struct NonTrivialConnectionInfo : ConnectionInfo
{
    using ConnectionInfo::ConnectionInfo;

    virtual ~NonTrivialConnectionInfo() override {
        if (m_signal) {
            // Destroy explicitly the resource
            reinterpret_cast<T*>(&m_signal->m_connections[m_index].slot.obj)->~T();
        }

    }

};

/*!
 * \brief The BasicConnection class
 */
class BasicConnection
{
public:
    BasicConnection(ConnectionInfo *data) noexcept
        : m_data(data) {}

    virtual ~BasicConnection() = default;
    BasicConnection(const BasicConnection&) = default;
    BasicConnection(BasicConnection&&) = default;

    ConnectionInfo * data() const {
        return m_data;
    }

private:
    ConnectionInfo * m_data {nullptr};
};



/*!
 * \brief Connection est juste une enveloppe de ConnectionBase
 * On peut rajouter d'autres methodes
 */
class Connection
{
public:
    Connection(BasicConnection conn);
    ~Connection();
    Connection(Connection&& other) noexcept;

    Connection(const Connection&) = delete;
    Connection& operator = (const Connection&) = delete;
    Connection& operator = (Connection&& other) noexcept;

    void disconnect();

private:
    ConnectionInfo * data {nullptr};
};

/*!
 * \brief Signal accepte des parametres variadics definis par ...Args
 */
template <typename ... Args>
struct Signal: public SignalBase
{
    /*!
     * \brief notify execute les slots
     * On peut passer d'autres arguments definis par CallArgs, le perfect forwarding devrait
     * permettre de caster les arguments correctement
     */
    template <typename ...CallArgs>
    void notify(CallArgs&& ...args) const
    {
        bool recursive = m_calling;

        // Mark the no
        if (!m_calling)
            m_calling = 1;

        // Pour chaque connection, on invoque la methode rengistree (functor ou PMF)
        // Les arguments sont castes par le perfect forwarding
        for (size_t i=0 ; i<m_connections.size() ; i++)
        {
            auto& slot = m_connections[i].slot;

            if (slot.func) {
                if (slot.func == slot.obj) {
                    reinterpret_cast<void(*)(Args...)>(slot.func)(std::forward<CallArgs>(args)...);
                } else {
                    reinterpret_cast<void(*)(void*, Args...)>(slot.func)(&slot.obj, std::forward<CallArgs>(args)...);
                }
            }
        }

        // Si une connecion a pris fin, on rearrange le vecteur pour que
        // les enregistrements restent contigus
        // Axe d'amelioration ?
        if (!recursive) {
            m_calling = false;

            if (m_dirty) {
                m_dirty = false;
                size_t idx {0};

                for (size_t i=0 ; i<m_connections.size() ; i++) {
                    if ( m_connections[i].conn) {
                        m_connections[i].conn->m_index = idx;
                        m_connections[idx] = m_connections[i];
                        idx++;
                    }
                }

                m_connections.resize(idx);
            }
        }

    }

    /*!
     * \brief Connecte
     *
     * \return BasicConnection
     */
    template<auto FuncPtr, class C>
    BasicConnection connect(C* object) const
    {
        // Prochain index dans les connections
        size_t idx = m_connections.size(); //m_connections.size();
        auto& connection = m_connections.emplace_back();
        connection.slot.obj = object;
        connection.slot.func = reinterpret_cast<void*>(+[](void* obj, Args ... args) {((*reinterpret_cast<C**>(obj))->*FuncPtr)(args...); });
        connection.conn = new ConnectionInfo(this, idx);
        return {connection.conn};
    }

    /*!
     * \brief connect a pointer to function with expected arguments
     */
    BasicConnection connect(void(*func)(Args...)) const
    {
        // Cast simple de la fonction en void *
        size_t idx = m_connections.size();
        auto& connection = m_connections.emplace_back();

        connection.slot.func = reinterpret_cast<void*>(func);
        connection.slot.obj = connection.slot.func;
        connection.conn = new ConnectionInfo(this, idx);
        return {connection.conn};
    }

    /*!
     * \brief connect a pointer to function with expected arguments
     */
    template<typename F>
    BasicConnection connect(F&& functor) const
    {
        using f_type = std::remove_pointer_t<std::remove_reference_t<F>>;
        if  constexpr(std::is_convertible_v<f_type, void(*)(Args...)>)
        {
            return connect(+functor);
        }
        else if constexpr (std::is_lvalue_reference_v<F>)
        {
            // Prochain index dans les connections
            size_t idx = m_connections.size();
            auto& connection = m_connections.emplace_back();
            connection.slot.obj = &functor;
            connection.slot.func = reinterpret_cast<void*>(+[](void* obj, Args ... args) {(*reinterpret_cast<f_type**>(obj))->operator()(args...); });
            connection.conn = new ConnectionInfo(this, idx);
            return {connection.conn};
        }
        else if constexpr (sizeof(std::remove_pointer_t<f_type>) <= sizeof(void*))
        {
            std::cout << "STOP: " << sizeof(std::remove_pointer_t<f_type>) << " <= " << sizeof(void*) << ", trivially: " << std::is_trivially_destructible_v<F> << std::endl;
            //copy the functor.
            size_t idx = m_connections.size();
            auto& connection = m_connections.emplace_back();
            connection.slot.func = reinterpret_cast<void*>(+[](void* obj, Args ... args) { reinterpret_cast<f_type*>(obj)->operator()(args...); });
            new (&connection.slot.obj) f_type(std::move(functor));

            using connection_t = std::conditional_t<std::is_trivially_destructible_v<F>, ConnectionInfo, NonTrivialConnectionInfo<F>>;
            connection.conn = new connection_t(this, idx);
            return {connection.conn};

        }
        return {nullptr};
    }
};

}



#endif // SIGNALSLOT_H
