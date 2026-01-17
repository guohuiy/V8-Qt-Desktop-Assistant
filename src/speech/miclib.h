#ifndef MICLIB_H
#define MICLIB_H

#include <QObject>
#include <QAudioSource>
#include <QIODevice>
#include <QBuffer>
#include <QTimer>
#include <QLibrary>

// Vosk types
typedef void* VoskModel;
typedef void* VoskRecognizer;

class MicLib : public QObject
{
    Q_OBJECT
public:
    explicit MicLib(QObject *parent = nullptr);
    ~MicLib();

    void startListening();
    void stopListening();

signals:
    void textRecognized(const QString &text);

private slots:
    void onReadyRead();

private:
    QAudioSource *audioSource;
    QBuffer *audioBuffer;
    QTimer *timer;
    QByteArray accumulatedAudioData;
    VoskModel *model;
    VoskRecognizer *recognizer;
    void processAudio();
};

#endif // MICLIB_H
