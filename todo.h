#ifndef TODO_H
#define TODO_H

#include <QObject>
#include <QString>
#include <QDateTime>

class Todo : public QObject
{
    Q_OBJECT
public:
    Todo(const QString &text)
        : text(text),
        done(false),
        created(QDateTime::currentDateTime()),
        updated(QDateTime::currentDateTime()) {}

    QString getText() const;
    void setText(const QString &newText);

    QDateTime getCreated() const;
    void setCreated(const QDateTime &newCreated);

    QDateTime getUpdated() const;
    void setUpdated(const QDateTime &newUpdated);

    bool getDone() const;
    void setDone(bool newDone);

signals:

private:
    QString text;
    bool    done;
    QDateTime created;
    QDateTime updated;
};

#endif // TODO_H
