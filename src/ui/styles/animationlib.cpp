#include "animationlib.h"
#include <QPropertyAnimation>
#include <QGraphicsDropShadowEffect>
#include <QEvent>
#include <QFocusEvent>
#include <QParallelAnimationGroup>

AnimationLib::AnimationLib(QObject *parent)
    : QObject{parent}
{
}

AnimationLib::~AnimationLib()
{
}

void AnimationLib::applyCloseButtonAnimation(QWidget *button)
{
    if (!button) return;

    // 创建阴影效果
    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect(button);
    shadow->setBlurRadius(0);
    shadow->setColor(Qt::red);
    shadow->setOffset(0, 0);
    button->setGraphicsEffect(shadow);

    // 创建边框动画助手
    BorderAnimationHelper *helper = new BorderAnimationHelper(button, button->styleSheet(), this);
    borderHelpers[button] = helper;

    // 安装事件过滤器
    button->installEventFilter(this);
}

void AnimationLib::applyInputEditAnimation(QWidget *edit)
{
    if (!edit) return;

    // 创建阴影效果用于边框发光
    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect(edit);
    shadow->setBlurRadius(0);
    shadow->setColor(QColor(0, 120, 212));
    shadow->setOffset(0, 0);
    edit->setGraphicsEffect(shadow);

    // 安装事件过滤器
    edit->installEventFilter(this);
}

void AnimationLib::applyCameraButtonAnimation(QWidget *button)
{
    if (!button) return;

    // 创建阴影效果
    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect(button);
    shadow->setBlurRadius(0);
    shadow->setColor(QColor(0, 120, 212));
    shadow->setOffset(0, 0);
    button->setGraphicsEffect(shadow);

    // 创建边框动画助手
    BorderAnimationHelper *helper = new BorderAnimationHelper(button, button->styleSheet(), this);
    borderHelpers[button] = helper;

    // 安装事件过滤器
    button->installEventFilter(this);
}

void AnimationLib::applyMicButtonAnimation(QWidget *button)
{
    if (!button) return;

    // 创建阴影效果
    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect(button);
    shadow->setBlurRadius(0);
    shadow->setColor(Qt::green);
    shadow->setOffset(0, 0);
    button->setGraphicsEffect(shadow);

    // 创建边框动画助手
    BorderAnimationHelper *helper = new BorderAnimationHelper(button, button->styleSheet(), this);
    borderHelpers[button] = helper;

    // 安装事件过滤器
    button->installEventFilter(this);
}

void AnimationLib::applySendButtonAnimation(QWidget *button)
{
    if (!button) return;

    // 创建阴影效果
    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect(button);
    shadow->setBlurRadius(0);
    shadow->setColor(Qt::blue);
    shadow->setOffset(0, 0);
    button->setGraphicsEffect(shadow);

    // 创建边框动画助手
    BorderAnimationHelper *helper = new BorderAnimationHelper(button, button->styleSheet(), this);
    borderHelpers[button] = helper;

    // 安装事件过滤器
    button->installEventFilter(this);
}

void AnimationLib::applyCloseMicButtonAnimation(QWidget *button)
{
    if (!button) return;

    // 创建阴影效果
    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect(button);
    shadow->setBlurRadius(0);
    shadow->setColor(Qt::gray);
    shadow->setOffset(0, 0);
    button->setGraphicsEffect(shadow);

    // 创建边框动画助手
    BorderAnimationHelper *helper = new BorderAnimationHelper(button, button->styleSheet(), this);
    borderHelpers[button] = helper;

    // 安装事件过滤器
    button->installEventFilter(this);
}

// 实现事件过滤器
bool AnimationLib::eventFilter(QObject *obj, QEvent *event)
{
    QWidget *widget = qobject_cast<QWidget*>(obj);
    if (!widget) return false;

    QGraphicsDropShadowEffect *effect = qobject_cast<QGraphicsDropShadowEffect*>(widget->graphicsEffect());
    if (!effect) return false;

    if (event->type() == QEvent::Enter) {
        // 鼠标进入：放大和阴影动画
        QPropertyAnimation *shadowAnim = new QPropertyAnimation(effect, "blurRadius");
        shadowAnim->setStartValue(0);
        shadowAnim->setEndValue(20);
        shadowAnim->setDuration(200);

        QParallelAnimationGroup *group = new QParallelAnimationGroup;
        group->addAnimation(shadowAnim);

        // 对于按钮，添加大小动画；对于输入框，不改变几何
        if (qobject_cast<QPushButton*>(widget)) {
            QPropertyAnimation *scaleAnim = new QPropertyAnimation(widget, "size");
            QSize startSize = widget->size();
            QSize endSize = startSize + QSize(10, 10);
            scaleAnim->setStartValue(startSize);
            scaleAnim->setEndValue(endSize);
            scaleAnim->setDuration(200);
            scaleAnim->setEasingCurve(QEasingCurve::OutCubic);
            group->addAnimation(scaleAnim);
        }

        if (borderHelpers.contains(widget)) {
            BorderAnimationHelper *helper = static_cast<BorderAnimationHelper*>(borderHelpers[widget]);
            QPropertyAnimation *borderAnim = new QPropertyAnimation(helper, "borderColor");
            borderAnim->setStartValue(QColor(255, 255, 255, 0)); // 透明
            borderAnim->setEndValue(QColor(255, 255, 255, 255)); // 不透明白色边框
            borderAnim->setDuration(200);
            group->addAnimation(borderAnim);
        }

        group->start(QAbstractAnimation::DeleteWhenStopped);

    } else if (event->type() == QEvent::Leave) {
        // 鼠标离开：恢复
        QPropertyAnimation *shadowAnim = new QPropertyAnimation(effect, "blurRadius");
        shadowAnim->setStartValue(20);
        shadowAnim->setEndValue(0);
        shadowAnim->setDuration(200);

        QParallelAnimationGroup *group = new QParallelAnimationGroup;
        group->addAnimation(shadowAnim);

        // 对于按钮，恢复大小；对于输入框，不改变几何
        if (qobject_cast<QPushButton*>(widget)) {
            QPropertyAnimation *scaleAnim = new QPropertyAnimation(widget, "size");
            QSize startSize = widget->size();
            QSize endSize = startSize - QSize(10, 10);
            scaleAnim->setStartValue(startSize);
            scaleAnim->setEndValue(endSize);
            scaleAnim->setDuration(200);
            scaleAnim->setEasingCurve(QEasingCurve::OutCubic);
            group->addAnimation(scaleAnim);
        }

        if (borderHelpers.contains(widget)) {
            BorderAnimationHelper *helper = static_cast<BorderAnimationHelper*>(borderHelpers[widget]);
            QPropertyAnimation *borderAnim = new QPropertyAnimation(helper, "borderColor");
            borderAnim->setStartValue(QColor(255, 255, 255, 255)); // 不透明
            borderAnim->setEndValue(QColor(255, 255, 255, 0)); // 透明
            borderAnim->setDuration(200);
            group->addAnimation(borderAnim);
        }

        group->start(QAbstractAnimation::DeleteWhenStopped);

    } else if (event->type() == QEvent::FocusIn && qobject_cast<QTextEdit*>(widget)) {
        // 输入框获得焦点：边框发光
        QPropertyAnimation *glowAnim = new QPropertyAnimation(effect, "blurRadius");
        glowAnim->setStartValue(0);
        glowAnim->setEndValue(15);
        glowAnim->setDuration(300);
        glowAnim->start(QAbstractAnimation::DeleteWhenStopped);

    } else if (event->type() == QEvent::FocusOut && qobject_cast<QTextEdit*>(widget)) {
        // 输入框失去焦点：恢复
        QPropertyAnimation *glowAnim = new QPropertyAnimation(effect, "blurRadius");
        glowAnim->setStartValue(15);
        glowAnim->setEndValue(0);
        glowAnim->setDuration(300);
        glowAnim->start(QAbstractAnimation::DeleteWhenStopped);
    }

    return false;
}
