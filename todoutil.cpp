#include "todoutil.h"
#include <QRandomGenerator>

QString TodoUtil::randomLorem(int words)
{
    static const QStringList loremWords = {
        "lorem", "ipsum", "dolor", "sit", "amet", "consectetur",
        "adipiscing", "elit", "sed", "do", "eiusmod", "tempor",
        "incididunt", "ut", "labore", "et", "dolore", "magna", "aliqua"
    };

    QStringList chosen;
    for (int i = 0; i < words; ++i) {
        int idx = QRandomGenerator::global()->bounded(loremWords.size());
        chosen << loremWords[idx];
    }
    return chosen.join(' ');
}

QDateTime TodoUtil::randomDateTime()
{
    QDate start(1901, 1, 1);
    QDate end = QDate::currentDate();

    qint64 startSecs = QDateTime(start, QTime(0,0)).toSecsSinceEpoch();
    qint64 endSecs   = QDateTime(end,   QTime(23,59)).toSecsSinceEpoch();

    qint64 randSecs = QRandomGenerator::global()->bounded(startSecs, endSecs);
    return QDateTime::fromSecsSinceEpoch(randSecs);
}

QList<Todo*> TodoUtil::generateTodos(int count)
{
    QList<Todo*> todos;
    todos.reserve(count);

    for (int i = 0; i < count; ++i) {

        Todo* todo = new Todo(randomLorem(QRandomGenerator::global()->bounded(4, 12)));
        todo->setDone(QRandomGenerator::global()->bounded(2) == 1);
        todo->setCreated(randomDateTime());
        todo->setUpdated(randomDateTime());


        // Ensure updated >= created
        if (todo->getUpdated() < todo->getCreated()) {
            todo->getUpdated() = todo->getCreated().addSecs(QRandomGenerator::global()->bounded(3600*24*365));
        }

        todos << todo;
    }
    return todos;
}
