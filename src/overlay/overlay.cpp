#include "overlay.h"
#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QApplication>
#include <QScreen>
#include <QTimer>

OverlayModule::OverlayModule(QObject *parent)
    : QObject(parent), overlayWindow(nullptr)
{
}

OverlayModule::~OverlayModule()
{
    if (overlayWindow) {
        overlayWindow->close();
        delete overlayWindow;
    }
}

void OverlayModule::showText(const QString &text)
{
    if (overlayWindow) {
        overlayWindow->close();
        delete overlayWindow;
    }

    overlayWindow = new QWidget();
    overlayWindow->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::Tool);
    overlayWindow->setAttribute(Qt::WA_TranslucentBackground);
    overlayWindow->setAttribute(Qt::WA_ShowWithoutActivating);

    // 创建标签，先设置文字以计算大小
    QLabel *label = new QLabel(text);
    label->setStyleSheet("color: white; font-size: 24px; background-color: rgba(0, 0, 0, 150); "
                         "border-radius: 10px; padding: 10px;");
    label->setAlignment(Qt::AlignCenter);
    label->setWordWrap(true);
    // 设置最大宽度，避免过宽
    label->setMaximumWidth(600);

    // 计算标签的合适大小
    QSize labelSize = label->sizeHint();
    // 添加一些边距
    int margin = 10;
    QSize windowSize(labelSize.width() + 2 * margin, labelSize.height() + 2 * margin);

    // 创建布局
    QVBoxLayout *layout = new QVBoxLayout(overlayWindow);
    layout->setContentsMargins(margin, margin, margin, margin);
    layout->addWidget(label);

    overlayWindow->setFixedSize(windowSize);

    // 居中显示在屏幕上
    QScreen *screen = QApplication::primaryScreen();
    QRect screenGeometry = screen->geometry();
    QPoint center = screenGeometry.center();
    overlayWindow->move(center.x() - windowSize.width() / 2, center.y() - windowSize.height() / 2);

    overlayWindow->show();

    // 5秒后自动隐藏
    QTimer::singleShot(5000, [this]() {
        if (overlayWindow) {
            overlayWindow->close();
            overlayWindow = nullptr;
        }
    });
}
