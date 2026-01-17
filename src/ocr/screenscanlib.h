#ifndef SCREENS_CANLIB_H
#define SCREENS_CANLIB_H

#include <QtCore/qglobal.h>

#ifndef SCREENS_CANLIB_EXPORT
#ifdef SCREENS_CANLIB_LIBRARY
#define SCREENS_CANLIB_EXPORT Q_DECL_EXPORT
#else
#define SCREENS_CANLIB_EXPORT Q_DECL_IMPORT
#endif
#endif

#include <QObject>
#include <QString>
#include <QTimer>

// ScreenScan接口
class ScreenScanInterface {
public:
    virtual ~ScreenScanInterface() {}
    virtual void startScanning() = 0;
    virtual void stopScanning() = 0;
};

class SCREENS_CANLIB_EXPORT ScreenScanLib : public QObject, public ScreenScanInterface
{
    Q_OBJECT
public:
    explicit ScreenScanLib(QObject *parent = nullptr);
    ~ScreenScanLib();

    void startScanning() override;
    void stopScanning() override;

signals:
    void textRecognized(const QString &text);

private slots:
    void onTimeout();

private:
    QTimer *timer;
    bool scanning;
};

#endif // SCREENS_CANLIB_H
