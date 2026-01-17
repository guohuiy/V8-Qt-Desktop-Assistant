#include "mainwindow.h"
#include <QApplication>
#include <QScreen>
#include <QVBoxLayout>
#include <QLabel>
#include <algorithm>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    // 设置窗口属性：全屏、透明、无边框
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    setAttribute(Qt::WA_TranslucentBackground);
    showFullScreen();

    // 创建中央widget
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    // 主布局
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    // 上方空白区域 (进一步增加权重让工具栏更靠下)
    mainLayout->addStretch(5);

    // 创建独立的控件容器（无背景工具栏）
    QWidget *controls = new QWidget();
    QHBoxLayout *controlsLayout = new QHBoxLayout(controls);
    controlsLayout->setContentsMargins(5, 5, 5, 5);
    controlsLayout->setSpacing(10); // 增加间距使组件更独立

    // 关闭按钮
    closeButton = new QPushButton();
    closeButton->setFixedSize(50, 50);
    closeButton->setStyleSheet("QPushButton { border-radius: 25px; background-color: rgba(255,0,0,0.8); color: white; font-size: 24px; font-weight: bold; border: none; }"
                                "QPushButton:hover { background-color: rgba(255,0,0,1.0); }");
    closeButton->setText("✕");
    connect(closeButton, &QPushButton::clicked, this, &QWidget::close);
    controlsLayout->addWidget(closeButton);

    // 输入框容器
    inputContainer = new QWidget();
    inputContainer->setStyleSheet("background-color: transparent; border-radius: 5px;");
    inputLayout = new QHBoxLayout(inputContainer);
    inputLayout->setContentsMargins(0, 0, 0, 0);
    inputLayout->setSpacing(5);

    // 关闭麦克风按钮
    closeMicButton = new QPushButton("✕");
    closeMicButton->setFixedSize(30, 30);
    closeMicButton->setStyleSheet("QPushButton { border-radius: 15px; background-color: rgba(128,128,128,0.8); color: white; font-size: 16px; border: none; }"
                                  "QPushButton:hover { background-color: rgba(128,128,128,1.0); }");
    closeMicButton->hide();
    connect(closeMicButton, &QPushButton::clicked, this, &MainWindow::onCloseMicButtonClicked);
    inputLayout->addWidget(closeMicButton);

    // 输入框
    inputEdit = new QTextEdit();
    inputEdit->setPlaceholderText("输入文字...");
    inputEdit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    inputEdit->setMinimumWidth(200); // 长度缩小一半
    inputEdit->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff); // 隐藏滚动条
    inputEdit->setStyleSheet("font-size: 16px; padding: 5px; border: 2px solid rgba(0,120,212,0.5); border-radius: 5px; color: white; background-color: rgba(255,255,255,0.1);");
    connect(inputEdit, &QTextEdit::textChanged, this, &MainWindow::onInputTextChanged);
    inputLayout->addWidget(inputEdit);

    // 麦克风按钮
    micButton = new QPushButton("🎤");
    micButton->setFixedSize(30, 30);
    micButton->setStyleSheet("QPushButton { border-radius: 15px; background-color: rgba(0,255,0,0.8); color: white; font-size: 16px; border: none; }"
                             "QPushButton:hover { background-color: rgba(0,255,0,1.0); }");
    connect(micButton, &QPushButton::clicked, this, &MainWindow::onMicButtonClicked);
    inputLayout->addWidget(micButton);

    // 发送按钮
    sendButton = new QPushButton("发送");
    sendButton->setFixedSize(50, 30);
    sendButton->setStyleSheet("QPushButton { border: none; background-color: rgba(0,120,212,0.8); color: white; font-size: 14px; border-radius: 5px; }"
                              "QPushButton:hover { background-color: rgba(0,120,212,1.0); }");
    sendButton->hide();
    connect(sendButton, &QPushButton::clicked, this, &MainWindow::onSendButtonClicked);
    inputLayout->addWidget(sendButton);

    controlsLayout->addWidget(inputContainer);

    // 相机按钮
    cameraButton = new QPushButton();
    cameraButton->setFixedSize(50, 50);
    cameraButton->setStyleSheet("QPushButton { border-radius: 25px; background-color: rgba(0,120,212,0.8); color: white; font-size: 24px; font-weight: bold; border: none; }"
                                "QPushButton:hover { background-color: rgba(0,120,212,1.0); }");
    cameraButton->setText("📷");
    connect(cameraButton, &QPushButton::clicked, this, &MainWindow::onCameraButtonClicked);
    controlsLayout->addWidget(cameraButton);

    mainLayout->addWidget(controls, 0, Qt::AlignHCenter);

    // 下方空白区域 (减少权重)
    mainLayout->addStretch(1);

    // 初始化相机相关
    camera = nullptr;
    captureSession = nullptr;
    videoWidget = nullptr;

    // 初始化模块
    micLib = new SpeechModule(this);
    connect(micLib, &SpeechModule::textRecognized, this, &MainWindow::onTextRecognized);

    // 初始化悬浮显示库
    overlayLib = new OverlayModule(this);

    // 初始化AI库
    aiLib = new AIModule(this);

    // 初始化屏幕扫描库
    screenScanLib = new OCRModule(this);
    connect(screenScanLib, &OCRModule::textRecognized, overlayLib, &OverlayModule::showText);

    // 初始化动画库
    animationLib = new AnimationLib(this);
    animationLib->applyCloseButtonAnimation(closeButton);
    animationLib->applyInputEditAnimation(inputEdit);
    animationLib->applyCameraButtonAnimation(cameraButton);
    animationLib->applyMicButtonAnimation(micButton);
    animationLib->applySendButtonAnimation(sendButton);
    animationLib->applyCloseMicButtonAnimation(closeMicButton);
}

MainWindow::~MainWindow()
{
    if (animationLib) {
        delete animationLib;
        animationLib = nullptr;
    }
}

void MainWindow::onCameraButtonClicked()
{
    if (camera == nullptr) {
        // 启动相机
        camera = new QCamera(this);
        captureSession = new QMediaCaptureSession(this);
        videoWidget = new QVideoWidget();
        videoWidget->setAttribute(Qt::WA_DeleteOnClose);
        videoWidget->setWindowTitle("Camera");
        videoWidget->resize(640, 480);

        captureSession->setCamera(camera);
        captureSession->setVideoOutput(videoWidget);

        connect(videoWidget, &QObject::destroyed, this, &MainWindow::onVideoWidgetDestroyed);

        videoWidget->show();
        camera->start();
        cameraButton->setEnabled(false);
    } else {
        // 停止相机
        camera->stop();
        videoWidget->close();
    }
}

void MainWindow::onVideoWidgetDestroyed()
{
    if (camera) {
        camera->stop();
        delete camera;
        camera = nullptr;
    }
    if (captureSession) {
        delete captureSession;
        captureSession = nullptr;
    }
    videoWidget = nullptr;
    cameraButton->setEnabled(true);
}

void MainWindow::onInputTextChanged()
{
    QString text = inputEdit->toPlainText().trimmed();
    if (text.isEmpty()) {
        if (!closeMicButton->isVisible()) { // not in mic mode
            micButton->show();
            sendButton->hide();
        }
    } else {
        micButton->hide();
        sendButton->show();
    }
}

void MainWindow::onMicButtonClicked()
{
    closeMicButton->show();
    micButton->hide();
    sendButton->show();
    // inputEdit->setPlainText("语音输入中...");  // 移除，保持原来文本
    micLib->startListening();
}

void MainWindow::onSendButtonClicked()
{
    QString text = inputEdit->toPlainText().trimmed();
    if (!text.isEmpty()) {
        if (text == "扫描屏幕") {
            screenScanLib->startScanning();
            overlayLib->showText("开始扫描屏幕...");
        } else {
            QString processedText = aiLib->processText(text);
            overlayLib->showText(processedText);
        }
        inputEdit->clear();
    }
}

void MainWindow::onCloseMicButtonClicked()
{
    closeMicButton->hide();
    micButton->show();
    sendButton->hide();
    inputEdit->clear();
    micLib->stopListening();
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        dragPosition = event->globalPosition().toPoint() - frameGeometry().topLeft();
        event->accept();
    }
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton) {
        move(event->globalPosition().toPoint() - dragPosition);
        event->accept();
    }
}

void MainWindow::onTextRecognized(const QString &text)
{
    QString current = inputEdit->toPlainText();
    inputEdit->setPlainText(current + text);
}
