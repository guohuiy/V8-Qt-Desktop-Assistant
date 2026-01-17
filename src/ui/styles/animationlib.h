#ifndef ANIMATIONLIB_H
#define ANIMATIONLIB_H

#include <QtCore/qglobal.h>

#ifndef ANIMATIONLIB_EXPORT
#ifdef ANIMATIONLIB_LIBRARY
#define ANIMATIONLIB_EXPORT Q_DECL_EXPORT
#else
#define ANIMATIONLIB_EXPORT Q_DECL_IMPORT
#endif
#endif

#include <QObject>
#include <QWidget>
#include <QPushButton>
#include <QTextEdit>
#include <QPropertyAnimation>
#include <QMap>
#include <QColor>

// 边框动画助手类
class BorderAnimationHelper : public QObject {
    Q_OBJECT
    Q_PROPERTY(QColor borderColor READ borderColor WRITE setBorderColor)

public:
    BorderAnimationHelper(QWidget *widget, const QString &originalStyle, QObject *parent = nullptr)
        : QObject(parent), m_widget(widget), m_originalStyle(originalStyle) {}

    QColor borderColor() const { return m_borderColor; }
    void setBorderColor(const QColor &color) {
        m_borderColor = color;
        QString newStyle = m_originalStyle + QString("border: 2px solid rgba(%1,%2,%3,%4);").arg(color.red()).arg(color.green()).arg(color.blue()).arg(color.alpha());
        m_widget->setStyleSheet(newStyle);
    }

private:
    QWidget *m_widget;
    QString m_originalStyle;
    QColor m_borderColor;
};

// 动画接口
class AnimationInterface {
public:
    virtual ~AnimationInterface() {}
    virtual void applyCloseButtonAnimation(QWidget *button) = 0;
    virtual void applyInputEditAnimation(QWidget *edit) = 0;
    virtual void applyCameraButtonAnimation(QWidget *button) = 0;
    virtual void applyMicButtonAnimation(QWidget *button) = 0;
    virtual void applySendButtonAnimation(QWidget *button) = 0;
    virtual void applyCloseMicButtonAnimation(QWidget *button) = 0;
};

class ANIMATIONLIB_EXPORT AnimationLib : public QObject, public AnimationInterface
{
    Q_OBJECT
public:
    explicit AnimationLib(QObject *parent = nullptr);
    ~AnimationLib();

    void applyCloseButtonAnimation(QWidget *button) override;
    void applyInputEditAnimation(QWidget *edit) override;
    void applyCameraButtonAnimation(QWidget *button) override;
    void applyMicButtonAnimation(QWidget *button) override;
    void applySendButtonAnimation(QWidget *button) override;
    void applyCloseMicButtonAnimation(QWidget *button) override;

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;

private:
    QMap<QWidget*, QObject*> borderHelpers;
};

#endif // ANIMATIONLIB_H
