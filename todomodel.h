#ifndef TODOMODEL_H
#define TODOMODEL_H

#include "todorepository.h"

#include <QAbstractTableModel>
#include <todo.h>


class TodoModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit TodoModel(QSqlDatabase *db, QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role) override;

    void addTodo(Todo *todo);
    void addTodos(const QList<Todo*> &todos);
    void removeTodoAt(int row);
    void toggleDoneTodo(int row);
    Todo* getTodoAt(int row);

    QDateTime minCreated() const;
    QDateTime maxCreated() const;
    QDateTime minUpdated() const;
    QDateTime maxUpdated() const;
    void reload();

private:
    TodoRepository *repo;
    QList<Todo*> m_todos;

signals:
};

#endif // TODOMODEL_H
