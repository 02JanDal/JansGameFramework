#include "Logging.h"

#include <QDateTime>

Q_LOGGING_CATEGORY(LOG_GENERAL, "icecream.server")
Q_LOGGING_CATEGORY(LOG_AI, "icecream.server.ai")
Q_LOGGING_CATEGORY(LOG_CACHE, "icecream.server.cache")
Q_LOGGING_CATEGORY(LOG_ENTITY, "icecream.server.entity")
Q_LOGGING_CATEGORY(LOG_NETWORK, "icecream.server.network")
Q_LOGGING_CATEGORY(LOG_PERMISSION, "icecream.server.permission")
Q_LOGGING_CATEGORY(LOG_PHYSICS, "icecream.server.physics")
Q_LOGGING_CATEGORY(LOG_PLUGIN, "icecream.server.plugin")
Q_LOGGING_CATEGORY(LOG_SAVE, "icecream.server.save")

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
	switch (type)
	{
	case QtTraceMsg:
		fprintf(stderr, "[%s][TRACE] %s\n", qPrintable(QDateTime::currentDateTime().toString(Qt::RFC2822Date)), qPrintable(msg));
		break;
	case QtDebugMsg:
		fprintf(stderr, "[%s][%s][DEBUG] %s\n", qPrintable(QDateTime::currentDateTime().toString(Qt::RFC2822Date)), context.category, qPrintable(msg));
		break;
	case QtWarningMsg:
		fprintf(stderr, "[%s][%s][WARNING] %s\n", qPrintable(QDateTime::currentDateTime().toString(Qt::RFC2822Date)), context.category, qPrintable(msg));
		break;
	case QtCriticalMsg:
		fprintf(stderr, "[%s][%s][CRITICAL] %s\n", qPrintable(QDateTime::currentDateTime().toString(Qt::RFC2822Date)), context.category, qPrintable(msg));
		break;
	case QtFatalMsg:
		fprintf(stderr, "[%s][%s][FATAL] %s\n", qPrintable(QDateTime::currentDateTime().toString(Qt::RFC2822Date)), context.category, qPrintable(msg));
#ifdef DEBUG_BUILD
		abort();
#endif
		break;
	}
}
