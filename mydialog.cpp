#include <QDebug>
#include <QKeyEvent>
#include <QThread>

#include "mydialog.h"
#include "./ui_mydialog.h"


#include "eventloop.h"

/*!
 * \brief MyDialog::MyDialog
 * \param parent
 */
MyDialog::MyDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::MyDialog)
    , m_evLoop(std::make_unique<EventLoop>())
{
    ui->setupUi(this);
}


/*!
 * \brief MyDialog::~MyDialog
 */
MyDialog::~MyDialog()
{
    delete ui;

    if (m_timerId!=-1) {
        killTimer(m_timerId);
    }
}


/*!
 * \brief MyDialog::on_chbox_timer_events_toggled
 * \param value
 */
void
MyDialog::on_chbox_timer_events_toggled(bool value)
{
    if (value) {
        m_timerId = startTimer(ui->spin_timer->value());
    }
    else {
        killTimer(m_timerId);
        m_timerId = -1;
    }
}


/*!
 * \brief MyDialog::on_btn_send_clicked
 */
void
MyDialog::on_btn_send_clicked()
{
    auto str = ui->edit_simple_msg->text().toStdString();

    if (str.size() != 0) {
        
        m_evLoop->post([str]{
            qDebug() << "MESSAGE EVENT: msg=" << QString::fromStdString(str);

        }, ui->chbox_pri_msg->isChecked());
    }
}

void
MyDialog::keyPressEvent(QKeyEvent *ev)
{
    if (ui->chbox_kb_events->isChecked()) {

        //qDebug() << "key pressed = " << ev->key();
        auto key = ev->key();
        m_evLoop->post([key]{
            qDebug() << "KEYBOARD EVENT: key=" << key;
            QThread::sleep(2);

        }, ui->chbox_pri_kb->isChecked());
        ev->accept();
    }
    else {
        ev->ignore();
    }

}

void
MyDialog::timerEvent(QTimerEvent *ev)
{
    if (ev->timerId() == m_timerId) {
        m_evLoop->post([this]{
            qDebug() << "TIMER EVENT: id=" << m_timerId;

        }, ui->chbox_pri_timer->isChecked());
    }
    ev->accept();
}
