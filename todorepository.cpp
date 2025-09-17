#include "todorepository.h"

#include <qsqlquery.h>

TodoRepository::TodoRepository(QSqlDatabase db) : m_db(db) {}

bool TodoRepository::create(const Todo &todo)
{
    QSqlQuery query(m_db);
    query.prepare("INSERT INTO todos (text, done, created, updated) "
                  "VALUES (:text, :done, :created, :updated)");
    query.bindValue(":text", todo.getText());
    query.bindValue(":done", todo.getDone() ? 1 : 0);
    query.bindValue(":created", todo.getCreated());
    query.bindValue(":updated", todo.getUpdated());

    if (!query.exec()) {
        qDebug() << "Failed to insert todo";
        return false;
    }
    return true;
}

bool TodoRepository::update(const Todo &todo) {
    if (todo.getId().isEmpty()) {
        qDebug() << "Cannot update Todo without ID!";
        return false;
    }

    QSqlQuery query(m_db);
    query.prepare("UPDATE todos SET text = :text, done = :done, updated = :updated WHERE id = :id");
    query.bindValue(":text", todo.getText());
    query.bindValue(":done", todo.getDone() ? 1 : 0);
    query.bindValue(":updated", todo.getUpdated());
    query.bindValue(":id", todo.getId());

    if (!query.exec()) {
        qDebug() << "Failed to update todo";
        return false;
    }

    if (query.numRowsAffected() == 0) {
        qDebug() << "No rows updated for ID:" << todo.getId();
    }

    return true;
}

bool TodoRepository::remove(const Todo &todo)
{
    QSqlQuery query(m_db);
    query.prepare("DELETE FROM todos WHERE id = :id");
    query.bindValue(":id", todo.getId());

    if (!query.exec()) {
        qDebug() << "Failed to delete todo";
        return false;
    }
    return true;
}

QList<Todo *> TodoRepository::loadAll()
{
    QList<Todo*> todos;
    QSqlQuery query(m_db);

    if (!query.exec("SELECT id, text, done, created, updated FROM todos")) {
        qDebug() << "Failed to load todos";
        return todos;
    }

    while (query.next()) {
        Todo *todo = new Todo(query.value("text").toString());
        todo->setDone(query.value("done").toInt() == 1);
        todo->setCreated(query.value("created").toDateTime());
        todo->setUpdated(query.value("updated").toDateTime());
        todo->setId(query.value("id").toString());

        todos.append(todo);
    }

    return todos;
}

// NEW: Streaming batch loader
QList<Todo *> TodoRepository::loadBatch(int offset, int batchSize)
{
    QList<Todo*> todos;
    QSqlQuery query(m_db);
    query.prepare("SELECT id, text, done, created, updated FROM todos LIMIT :limit OFFSET :offset");
    query.bindValue(":limit", batchSize);
    query.bindValue(":offset", offset);

    if (!query.exec()) {
        qDebug() << "Failed to load todos batch";
        return todos;
    }

    while (query.next()) {
        Todo *todo = new Todo(query.value("text").toString());
        todo->setDone(query.value("done").toInt() == 1);
        todo->setCreated(query.value("created").toDateTime());
        todo->setUpdated(query.value("updated").toDateTime());
        todo->setId(query.value("id").toString());
        todos.append(todo);
    }
    return todos;
}
