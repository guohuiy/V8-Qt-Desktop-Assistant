#include "miclib.h"
#include <QAudioFormat>
#include <QAudioDevice>
#include <QMediaDevices>
#include <QTimer>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QCoreApplication>

// Vosk function pointers
typedef void* VoskModel;
typedef void* VoskRecognizer;

typedef VoskModel* (*vosk_model_new_func)(const char *model_path);
typedef VoskRecognizer* (*vosk_recognizer_new_func)(VoskModel *model, float sample_rate);
typedef void (*vosk_recognizer_free_func)(VoskRecognizer *recognizer);
typedef void (*vosk_model_free_func)(VoskModel *model);
typedef int (*vosk_recognizer_accept_waveform_func)(VoskRecognizer *recognizer, const char *data, int length);
typedef const char* (*vosk_recognizer_result_func)(VoskRecognizer *recognizer);
typedef const char* (*vosk_recognizer_partial_result_func)(VoskRecognizer *recognizer);

static QLibrary *voskLib = nullptr;
static vosk_model_new_func vosk_model_new_ptr = nullptr;
static vosk_recognizer_new_func vosk_recognizer_new_ptr = nullptr;
static vosk_recognizer_free_func vosk_recognizer_free_ptr = nullptr;
static vosk_model_free_func vosk_model_free_ptr = nullptr;
static vosk_recognizer_accept_waveform_func vosk_recognizer_accept_waveform_ptr = nullptr;
static vosk_recognizer_result_func vosk_recognizer_result_ptr = nullptr;
static vosk_recognizer_partial_result_func vosk_recognizer_partial_result_ptr = nullptr;

MicLib::MicLib(QObject *parent)
    : QObject(parent), audioSource(nullptr), audioBuffer(nullptr), timer(nullptr), model(nullptr), recognizer(nullptr)
{
    if (!voskLib) {
        QString appDir = QCoreApplication::applicationDirPath();
        QString voskLibPath = appDir + "/../libs/vosk/lib/libvosk.dll";
        voskLib = new QLibrary(voskLibPath);
        if (!voskLib->load()) {
            qDebug() << "Failed to load libvosk.dll:" << voskLib->errorString();
            return;
        }
        vosk_model_new_ptr = (vosk_model_new_func)voskLib->resolve("vosk_model_new");
        vosk_recognizer_new_ptr = (vosk_recognizer_new_func)voskLib->resolve("vosk_recognizer_new");
        vosk_recognizer_free_ptr = (vosk_recognizer_free_func)voskLib->resolve("vosk_recognizer_free");
        vosk_model_free_ptr = (vosk_model_free_func)voskLib->resolve("vosk_model_free");
        vosk_recognizer_accept_waveform_ptr = (vosk_recognizer_accept_waveform_func)voskLib->resolve("vosk_recognizer_accept_waveform");
        vosk_recognizer_result_ptr = (vosk_recognizer_result_func)voskLib->resolve("vosk_recognizer_result");
        vosk_recognizer_partial_result_ptr = (vosk_recognizer_partial_result_func)voskLib->resolve("vosk_recognizer_partial_result");
        if (!vosk_model_new_ptr || !vosk_recognizer_new_ptr || !vosk_recognizer_free_ptr || !vosk_model_free_ptr ||
            !vosk_recognizer_accept_waveform_ptr || !vosk_recognizer_result_ptr || !vosk_recognizer_partial_result_ptr) {
            qDebug() << "Failed to resolve Vosk functions";
        }
    }
}

MicLib::~MicLib()
{
    stopListening();
    if (recognizer && vosk_recognizer_free_ptr) {
        vosk_recognizer_free_ptr(recognizer);
        recognizer = nullptr;
    }
    if (model && vosk_model_free_ptr) {
        vosk_model_free_ptr(model);
        model = nullptr;
    }
}

void MicLib::startListening()
{
    qDebug() << "MicLib::startListening called";
    if (audioSource) {
        qDebug() << "Already listening";
        return;
    }

    // 初始化Vosk模型和识别器
    if (!model) {
        qDebug() << "Loading Vosk model...";
        QString appDir = QCoreApplication::applicationDirPath();
        QString modelPath = appDir + "/../resources/models/vosk-model-cn-0.22";
        if (vosk_model_new_ptr) {
            model = vosk_model_new_ptr(modelPath.toUtf8().constData()); // 使用相对路径
        }
        if (!model) {
            qDebug() << "Failed to load Vosk model from" << modelPath;
            return;
        }
        qDebug() << "Model loaded successfully";
    }
    if (!recognizer) {
        qDebug() << "Creating Vosk recognizer...";
        if (vosk_recognizer_new_ptr) {
            recognizer = vosk_recognizer_new_ptr(model, 16000.0f);
        }
        if (!recognizer) {
            qDebug() << "Failed to create Vosk recognizer";
            return;
        }
        qDebug() << "Recognizer created successfully";
    }

    qDebug() << "Setting up audio format...";
    QAudioFormat format;
    format.setSampleRate(16000);
    format.setChannelCount(1);
    format.setSampleFormat(QAudioFormat::Int16);

    qDebug() << "Checking audio device...";
    QAudioDevice defaultDevice = QMediaDevices::defaultAudioInput();
    if (defaultDevice.isNull()) {
        qDebug() << "No audio input device available";
        return;
    }
    qDebug() << "Audio device found:" << defaultDevice.description();

    qDebug() << "Creating audio source...";
    audioSource = new QAudioSource(defaultDevice, format, this);
    audioBuffer = new QBuffer(this);
    audioBuffer->open(QIODevice::ReadWrite);

    qDebug() << "Starting audio recording...";
    audioSource->start(audioBuffer);

    qDebug() << "Connecting signals...";
    connect(audioBuffer, &QBuffer::readyRead, this, &MicLib::onReadyRead);

    qDebug() << "Starting timer...";
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MicLib::processAudio);
    timer->start(1000); // 每1秒处理一次，实时更新

    qDebug() << "Start listening completed";
}

void MicLib::stopListening()
{
    if (audioSource) {
        audioSource->stop();
        delete audioSource;
        audioSource = nullptr;
    }
    if (audioBuffer) {
        audioBuffer->close();
        delete audioBuffer;
        audioBuffer = nullptr;
    }
    if (timer) {
        timer->stop();
        delete timer;
        timer = nullptr;
    }
}

void MicLib::onReadyRead()
{
    // 累积音频数据
    audioBuffer->seek(0);
    QByteArray newData = audioBuffer->readAll();
    accumulatedAudioData.append(newData);
    // 清空buffer以准备下次写入
    audioBuffer->buffer().clear();
    audioBuffer->seek(0);
}

void MicLib::processAudio()
{
    if (accumulatedAudioData.isEmpty() || !recognizer) return;

    // 发送累积的音频数据到Vosk
    try {
        if (vosk_recognizer_accept_waveform_ptr) {
            int result = vosk_recognizer_accept_waveform_ptr(recognizer, accumulatedAudioData.data(), accumulatedAudioData.size());
            if (result) {
                if (vosk_recognizer_result_ptr) {
                    const char *final_result = vosk_recognizer_result_ptr(recognizer);
                    if (final_result) {
                        QJsonDocument doc = QJsonDocument::fromJson(final_result);
                        QString text = doc.object().value("text").toString();
                        if (!text.isEmpty()) {
                            emit textRecognized(text);
                            qDebug() << "Recognized text:" << text;
                        }
                    }
                }
            } else {
                // 获取partial result
                if (vosk_recognizer_partial_result_ptr) {
                    const char *partial_result = vosk_recognizer_partial_result_ptr(recognizer);
                    if (partial_result) {
                        QJsonDocument doc = QJsonDocument::fromJson(partial_result);
                        QString text = doc.object().value("partial").toString();
                        if (!text.isEmpty()) {
                            emit textRecognized(text);
                            qDebug() << "Partial text:" << text;
                        }
                    }
                }
            }
        }
    } catch (...) {
        qDebug() << "Exception in Vosk processing";
    }

    // 清空累积数据
    accumulatedAudioData.clear();
}
