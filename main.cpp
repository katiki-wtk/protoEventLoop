#include <unordered_map>
#include "mydialog.h"


#include <QApplication>
#include <QDateTime>
#include <QDebug>
#include <QMutex>
#include <QThread>

static MyDialog * w {nullptr};


void myMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{

    static std::unordered_map<QtMsgType, QString> typeMap = {
        {QtMsgType::QtCriticalMsg, "Critical"},
        {QtMsgType::QtDebugMsg, "Debug"},
        {QtMsgType::QtFatalMsg, "Fatal"},
        {QtMsgType::QtInfoMsg, "Info"},
        {QtMsgType::QtWarningMsg, "Warning"}
    };

    if (!w)
        return;

    auto dt = QDateTime::currentDateTime();
    auto str = QString("%1: -- %2 -- %3 -- %4").arg(typeMap[type]).arg(dt.toString("hh:mm:ss.zzz")).arg(reinterpret_cast<uint64_t>(QThread::currentThreadId())).arg(msg);


    /*QByteArray localMsg = msg.toLocal8Bit();
    const char *file = context.file ? context.file : "";
    const char *function = context.function ? context.function : "";
    switch (type) {
    case QtDebugMsg:
        fprintf(stderr, "Debug: %s (%s:%u, %s)\n", localMsg.constData(), file, context.line, function);
        break;
    case QtInfoMsg:
        fprintf(stderr, "Info: %s (%s:%u, %s)\n", localMsg.constData(), file, context.line, function);
        break;
    case QtWarningMsg:
        fprintf(stderr, "Warning: %s (%s:%u, %s)\n", localMsg.constData(), file, context.line, function);
        break;
    case QtCriticalMsg:
        fprintf(stderr, "Critical: %s (%s:%u, %s)\n", localMsg.constData(), file, context.line, function);
        break;
    case QtFatalMsg:
        fprintf(stderr, "Fatal: %s (%s:%u, %s)\n", localMsg.constData(), file, context.line, function);
        break;
    }
*/
}


int main(int argc, char *argv[])
{
    qSetMessagePattern("%{time hh:mm:ss.z} -- %{qthreadptr} -- %{category} -- %{message}");
//    qInstallMessageHandler(myMessageOutput);

    QApplication a(argc, argv);

    w = new MyDialog{};
    w->show();
    a.exec();
    delete w;
    return 0;
}
