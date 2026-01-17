#include "speech.h"
#include <QDebug>

SpeechModule::SpeechModule(QObject *parent)
    : QObject(parent), micLib(new MicLib(this))
{
    connect(micLib, &MicLib::textRecognized, this, &SpeechModule::textRecognized);
}

SpeechModule::~SpeechModule()
{
    stopListening();
}

void SpeechModule::startListening()
{
    qDebug() << "SpeechModule::startListening called";
    micLib->startListening();
}

void SpeechModule::stopListening()
{
    qDebug() << "SpeechModule::stopListening called";
    micLib->stopListening();
}
