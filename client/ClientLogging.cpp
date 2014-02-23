#include "ClientLogging.h"

#include <QDateTime>

Q_LOGGING_CATEGORY(LOG_GENERAL, "icecream.client")
Q_LOGGING_CATEGORY(LOG_GUI, "icecream.client.gui")
Q_LOGGING_CATEGORY(LOG_NETWORK, "icecream.client.network")
Q_LOGGING_CATEGORY(LOG_PLUGIN, "icecream.client.plugin")

void loggingFilter(QLoggingCategory *category)
{
#ifdef DEBUG_BUILD
	category->setEnabled(QtTraceMsg, true);
	category->setEnabled(QtDebugMsg, true);
#endif
	category->setEnabled(QtWarningMsg, true);
	category->setEnabled(QtCriticalMsg, true);
}
void messageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
#ifdef Q_OS_LINUX
	if (msg.startsWith("QXcbWindow"))
	{
		return;
	}
#endif
	switch (type)
	{
	case QtTraceMsg:
		fprintf(stderr, "[%s][TRACE] %s\n", qPrintable(QDateTime::currentDateTime().toString(Qt::RFC2822Date)), qPrintable(msg));
		break;
	case QtDebugMsg:
		fprintf(stderr, "[%s][DEBUG] %s\n", qPrintable(QDateTime::currentDateTime().toString(Qt::RFC2822Date)), qPrintable(msg));
		break;
	case QtWarningMsg:
		fprintf(stderr, "[%s][WARNING] %s\n", qPrintable(QDateTime::currentDateTime().toString(Qt::RFC2822Date)), qPrintable(msg));
		break;
	case QtCriticalMsg:
		fprintf(stderr, "[%s][CRITICAL] %s\n", qPrintable(QDateTime::currentDateTime().toString(Qt::RFC2822Date)), qPrintable(msg));
		break;
	case QtFatalMsg:
		fprintf(stderr, "[%s][FATAL] %s\n", qPrintable(QDateTime::currentDateTime().toString(Qt::RFC2822Date)), qPrintable(msg));
#ifdef DEBUG_BUILD
		abort();
#endif
		break;
	}
}
