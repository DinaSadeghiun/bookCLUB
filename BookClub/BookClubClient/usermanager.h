#ifndef USERMANAGER_H
#define USERMANAGER_H

#include <QObject>

class UserManager : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString userName READ userName NOTIFY userNameChanged)

public:
    explicit UserManager(QObject *parent = nullptr) : QObject(parent), m_userName("Guest User") {}
    QString userName() const { return m_userName; }

signals:
    void userNameChanged();

private:
    QString m_userName;
};

#endif // USERMANAGER_H
