#ifndef OCR_H
#define OCR_H

#include <QtCore/qglobal.h>

#ifndef OCR_EXPORT
#ifdef OCR_LIBRARY
#define OCR_EXPORT Q_DECL_EXPORT
#else
#define OCR_EXPORT Q_DECL_IMPORT
#endif
#endif

#include <QObject>
#include <QString>
#include <QTimer>

// OCR接口
class OCRInterface {
public:
    virtual ~OCRInterface() {}
    virtual void startScanning() = 0;
    virtual void stopScanning() = 0;
};

class OCR_EXPORT OCRModule : public QObject, public OCRInterface
{
    Q_OBJECT
public:
    explicit OCRModule(QObject *parent = nullptr);
    ~OCRModule();

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

#endif // OCR_H
