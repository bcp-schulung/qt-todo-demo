#ifndef TODOREPOSITORY_H
#define TODOREPOSITORY_H

#include "todo.h"

#include <QSqlDatabase>


class TodoRepository
{
public:
    TodoRepository(QSqlDatabase db);

    bool create(const Todo &todo);
    bool update(const Todo &todo);
    bool remove(const Todo &todo);
    QList<Todo*> loadAll();
    QList<Todo *> loadBatch(int offset, int batchSize);

private:
    QSqlDatabase m_db;
};

#endif // TODOREPOSITORY_H
