#ifndef OVERLAY_H
#define OVERLAY_H

#include <QtCore/qglobal.h>

#ifndef OVERLAY_EXPORT
#ifdef OVERLAY_LIBRARY
#define OVERLAY_EXPORT Q_DECL_EXPORT
#else
#define OVERLAY_EXPORT Q_DECL_IMPORT
#endif
#endif

#include <QObject>
#include <QString>

// 悬浮显示接口
class OverlayInterface {
public:
    virtual ~OverlayInterface() {}
    virtual void showText(const QString &text) = 0;
};

class OVERLAY_EXPORT OverlayModule : public QObject, public OverlayInterface
{
    Q_OBJECT
public:
    explicit OverlayModule(QObject *parent = nullptr);
    ~OverlayModule();

    void showText(const QString &text) override;

private:
    QWidget *overlayWindow;
};

#endif // OVERLAY_H
