#ifndef CAMERA_H
#define CAMERA_H

#include <QtCore/qglobal.h>

#ifndef CAMERA_EXPORT
#ifdef CAMERA_LIBRARY
#define CAMERA_EXPORT Q_DECL_EXPORT
#else
#define CAMERA_EXPORT Q_DECL_IMPORT
#endif
#endif

#include <QObject>

// 相机接口
class CameraInterface {
public:
    virtual ~CameraInterface() {}
    virtual void startPreview() = 0;
    virtual void stopPreview() = 0;
    virtual void captureImage() = 0;
};

class CAMERA_EXPORT CameraModule : public QObject, public CameraInterface
{
    Q_OBJECT
public:
    explicit CameraModule(QObject *parent = nullptr);
    ~CameraModule();

    void startPreview() override;
    void stopPreview() override;
    void captureImage() override;

signals:
    void imageCaptured(const QImage &image);

private:
    QWidget *cameraWidget;
};

#endif // CAMERA_H
