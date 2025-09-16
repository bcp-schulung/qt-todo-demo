#include "todomodel.h"

TodoModel::TodoModel(QObject *parent)
    : QAbstractTableModel(parent)
{}

int TodoModel::rowCount(const QModelIndex & /*parent*/) const
{
    return todos.size();
}

int TodoModel::columnCount(const QModelIndex & /*parent*/) const
{
    return 4;
}

QVariant TodoModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= todos.size())
        return QVariant();

    const Todo *todo = todos.at(index.row());

    if (role == Qt::DisplayRole || role == Qt::EditRole) {
        switch (index.column()) {
        case 0: return todo->getText();
        case 1: return todo->getCreated().toString("yyyy-MM-dd hh:mm");
        case 2: return todo->getUpdated().toString("yyyy-MM-dd hh:mm");
        case 3: return todo->getDone() ? "✓" : "";
        }
    }

    if (role == Qt::CheckStateRole && index.column() == 3) {
        return todo->getDone() ? Qt::Checked : Qt::Unchecked;
    }

    return QVariant();
}

QVariant TodoModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        switch (section) {
        case 0: return "Text";
        case 1: return "Created";
        case 2: return "Updated";
        case 3: return "Done";
        }
    }
    return QVariant();
}

Qt::ItemFlags TodoModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;

    if (index.column() == 0)  // Text column editable
        return Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsSelectable;

    if (index.column() == 3)  // Done column as checkbox
        return Qt::ItemIsUserCheckable | Qt::ItemIsEnabled | Qt::ItemIsSelectable;

    // Created & Updated: read-only
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

bool TodoModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid() || index.row() >= todos.size())
        return false;

    Todo *todo = todos[index.row()];

    if (index.column() == 0 && role == Qt::EditRole) {  // Text edit
        todo->setText(value.toString());
        todo->setUpdated(QDateTime::currentDateTime());
        emit dataChanged(index, this->index(index.row(), 2)); // text + updated
        return true;
    }

    if (index.column() == 3 && role == Qt::CheckStateRole) {  // Checkbox toggle
        todo->setDone(value.toInt() == Qt::Checked);
        todo->setUpdated(QDateTime::currentDateTime());
        emit dataChanged(index, this->index(index.row(), 2)); // checkbox + updated
        return true;
    }

    return false;
}

void TodoModel::addTodo(Todo *todo)
{
    beginInsertRows(QModelIndex(), todos.size(), todos.size());
    todos.append(todo);
    endInsertRows();
}

void TodoModel::removeTodoAt(int row)
{
    if (row < 0 || row >= todos.size()) {
        return;
    }

    beginRemoveRows(QModelIndex(), row, row);
    delete todos.takeAt(row);
    endRemoveRows();
}

void TodoModel::toggleDoneTodo(int row)
{
    if (row < 0 || row >= todos.size())
        return;

    Todo *todo = todos[row];
    todo->setDone(!todo->getDone());
    todo->setUpdated(QDateTime::currentDateTime());

    // notify view: done column and updated column
    QModelIndex doneIndex = index(row, 3);
    QModelIndex updatedIndex = index(row, 2);
    emit dataChanged(doneIndex, doneIndex, {Qt::CheckStateRole, Qt::DisplayRole});
    emit dataChanged(updatedIndex, updatedIndex, {Qt::DisplayRole});
}

Todo* TodoModel::getTodoAt(int row)
{
    return (row >= 0 && row < todos.size()) ? todos[row] : nullptr;
}
