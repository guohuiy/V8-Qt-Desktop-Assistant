#ifndef SPEECH_H
#define SPEECH_H

#include <QtCore/qglobal.h>

#ifndef SPEECH_EXPORT
#ifdef SPEECH_LIBRARY
#define SPEECH_EXPORT Q_DECL_EXPORT
#else
#define SPEECH_EXPORT Q_DECL_IMPORT
#endif
#endif

#include <QObject>
#include <QTimer>
#include "miclib.h"

class SPEECH_EXPORT SpeechModule : public QObject
{
    Q_OBJECT
public:
    explicit SpeechModule(QObject *parent = nullptr);
    ~SpeechModule();

    void startListening();
    void stopListening();

signals:
    void textRecognized(const QString &text);

private:
    MicLib *micLib;
};

#endif // SPEECH_H
