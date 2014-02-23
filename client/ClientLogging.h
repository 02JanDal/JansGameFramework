#pragma once

#include <QLoggingCategory>

#define LOG_GENERAL LogGeneral
#define LOG_GUI LogGui
#define LOG_NETWORK LogNetwork
#define LOG_PLUGIN LogPlugin

Q_DECLARE_LOGGING_CATEGORY(LOG_GENERAL)
Q_DECLARE_LOGGING_CATEGORY(LOG_GUI)
Q_DECLARE_LOGGING_CATEGORY(LOG_NETWORK)
Q_DECLARE_LOGGING_CATEGORY(LOG_PLUGIN)

void loggingFilter(QLoggingCategory *category);
void messageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg);
