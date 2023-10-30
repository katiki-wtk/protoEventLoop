#ifndef MYDIALOG_H
#define MYDIALOG_H

#include <QDialog>

#include <QTextEdit>
#include <QMutex>

class EventLoop;

QT_BEGIN_NAMESPACE
namespace Ui { class MyDialog; }
QT_END_NAMESPACE



/*!
 * \brief The MyDialog class
 */
class MyDialog : public QDialog
{
    Q_OBJECT

public:
    MyDialog(QWidget *parent = nullptr);
    ~MyDialog();


protected slots:
    void on_btn_send_clicked();
    void on_chbox_timer_events_toggled(bool);

protected:
    void timerEvent(QTimerEvent *);
    void keyPressEvent(QKeyEvent *);

private:
    Ui::MyDialog *ui;
    std::unique_ptr<EventLoop> m_evLoop;

    int m_timerId{-1};
};
#endif // MYDIALOG_H
