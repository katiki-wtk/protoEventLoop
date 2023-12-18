#include "signalslot.h"


namespace test
{

Connection::Connection(BasicConnection conn)
    : data(conn.data())
{
    data->m_used = true;
}

Connection::~Connection() {
    disconnect();
}

Connection::Connection(Connection&& other) noexcept
    : data(other.data) {
    other.data = nullptr;
}

Connection& Connection::operator=(Connection&& other) noexcept
{
    disconnect();
    data = other.data;
    other.data = nullptr;
    return *this;
}



void Connection::disconnect()
{
    delete data;
    data = nullptr;
}


SignalBase::SignalBase(SignalBase&& other) noexcept
    : m_connections(std::move(other.m_connections))
    , m_dirty(other.m_dirty)
{
    for (auto& connection : m_connections) {
        if (connection.conn) {
            connection.conn->m_signal = this;
        }
    }
}

SignalBase& SignalBase::operator= (SignalBase&& other) noexcept
{
    m_connections = std::move(other.m_connections);
    m_dirty = other.m_dirty;

    for (auto& connection : m_connections) {
        if (connection.conn) {
            connection.conn->m_signal = this;
        }
    }

    return *this;
}

SignalBase::~SignalBase()
{
    for (auto& connection : m_connections) {
        auto * conn = connection.conn;
        if (conn) {
            if (conn->m_used) {
                conn->m_signal = nullptr;
            }
            else {
                delete conn;
            }
        }
    }
}

}
