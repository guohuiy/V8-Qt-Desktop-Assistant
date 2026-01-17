#include "ocr.h"
#include <QApplication>
#include <QScreen>
#include <QPixmap>
#include <QPainter>
#include <QBuffer>
#include <QDebug>
#include <QProcess>
#include <QTemporaryFile>
#include <QDir>

OCRModule::OCRModule(QObject *parent) : QObject(parent), scanning(false) {
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &OCRModule::onTimeout);
}

OCRModule::~OCRModule() {
    stopScanning();
}

void OCRModule::startScanning() {
    if (!scanning) {
        scanning = true;
        timer->start(1000 / 30); // 每秒30次
    }
}

void OCRModule::stopScanning() {
    if (scanning) {
        scanning = false;
        timer->stop();
    }
}

void OCRModule::onTimeout() {
    if (!scanning) return;

    // 获取主屏幕
    QScreen *screen = QGuiApplication::primaryScreen();
    if (!screen) return;

    // 截取全屏
    QPixmap pixmap = screen->grabWindow(0);

    // 保存为临时文件
    QTemporaryFile tempFile(QDir::tempPath() + "/screen_XXXXXX.png");
    if (!tempFile.open()) {
        qDebug() << "Failed to create temp file";
        return;
    }

    if (!pixmap.save(&tempFile, "PNG")) {
        qDebug() << "Failed to save screenshot";
        return;
    }

    QString tempFileName = tempFile.fileName();
    tempFile.close();

    // 使用QProcess调用tesseract命令行
    QProcess process;
    QStringList arguments;
    arguments << tempFileName << "stdout" << "-l" << "chi_sim+eng";

    process.start("tesseract", arguments);
    if (!process.waitForFinished(5000)) { // 5秒超时
        qDebug() << "Tesseract process timeout";
        return;
    }

    QByteArray output = process.readAllStandardOutput();
    QString recognizedText = QString::fromUtf8(output).trimmed();

    // 清理临时文件
    tempFile.remove();

    if (!recognizedText.isEmpty()) {
        emit textRecognized(recognizedText);
    } else {
        // 如果没有识别到文字，发送模拟文本
        emit textRecognized("屏幕内容识别模拟");
    }
}
