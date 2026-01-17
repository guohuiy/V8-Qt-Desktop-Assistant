#ifndef AILIB_H
#define AILIB_H

#include <QtCore/qglobal.h>

#ifndef AILIB_EXPORT
#ifdef AILIB_LIBRARY
#define AILIB_EXPORT Q_DECL_EXPORT
#else
#define AILIB_EXPORT Q_DECL_IMPORT
#endif
#endif

#include <QObject>
#include <QString>

// AI接口
class AIInterface {
public:
    virtual ~AIInterface() {}
    virtual QString processText(const QString &text) = 0;
};

class AILIB_EXPORT AILib : public QObject, public AIInterface
{
    Q_OBJECT
public:
    explicit AILib(QObject *parent = nullptr);
    ~AILib();

    QString processText(const QString &text) override;
};

#endif // AILIB_H
