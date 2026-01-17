#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QWidget>
#include <QHBoxLayout>
#include <QTextEdit>
#include <QPushButton>
#include <QCamera>
#include <QMediaCaptureSession>
#include <QVideoWidget>
#include <QMouseEvent>
#include <QPoint>
#include <QSizePolicy>
#include "speech.h"
#include "overlay.h"
#include "ai.h"
#include "ocr.h"
#include "styles/animationlib.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onCameraButtonClicked();
    void onVideoWidgetDestroyed();
    void onInputTextChanged();
    void onMicButtonClicked();
    void onSendButtonClicked();
    void onCloseMicButtonClicked();
    void onTextRecognized(const QString &text);

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

private:
    QWidget *inputContainer;
    QHBoxLayout *inputLayout;
    QTextEdit *inputEdit;
    QPushButton *closeMicButton;
    QPushButton *micButton;
    QPushButton *sendButton;
    QPushButton *closeButton;
    QPushButton *cameraButton;
    QCamera *camera;
    QMediaCaptureSession *captureSession;
    QVideoWidget *videoWidget;
    QPoint dragPosition;
    SpeechModule *micLib;
    OverlayModule *overlayLib;
    AIModule *aiLib;
    OCRModule *screenScanLib;
    AnimationLib *animationLib;
};

#endif // MAINWINDOW_H
