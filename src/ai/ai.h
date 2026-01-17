#ifndef AI_H
#define AI_H

#include <QtCore/qglobal.h>

#ifndef AI_EXPORT
#ifdef AI_LIBRARY
#define AI_EXPORT Q_DECL_EXPORT
#else
#define AI_EXPORT Q_DECL_IMPORT
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

class AI_EXPORT AIModule : public QObject, public AIInterface
{
    Q_OBJECT
public:
    explicit AIModule(QObject *parent = nullptr);
    ~AIModule();

    QString processText(const QString &text) override;
};

#endif // AI_H
