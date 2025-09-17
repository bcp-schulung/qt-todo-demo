#include "todomodel.h"

TodoModel::TodoModel(QSqlDatabase *db, QObject *parent)
    : QAbstractTableModel(parent)
{
    repo = new TodoRepository(*db);

    reload();
}

int TodoModel::rowCount(const QModelIndex & /*parent*/) const
{
    return m_todos.size();
}

int TodoModel::columnCount(const QModelIndex & /*parent*/) const
{
    return 5;
}

QVariant TodoModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= m_todos.size())
        return QVariant();

    const Todo *todo = m_todos.at(index.row());

    if (role == Qt::DisplayRole || role == Qt::EditRole) {
        switch (index.column()) {
        case 0: return todo->getId();
        case 1: return todo->getText();
        case 2: return todo->getCreated().toString("yyyy-MM-dd hh:mm");
        case 3: return todo->getUpdated().toString("yyyy-MM-dd hh:mm");
        case 4: return todo->getDone() ? "✓" : "";
        }
    }

    if (role == Qt::CheckStateRole && index.column() == 4) {
        return todo->getDone() ? Qt::Checked : Qt::Unchecked;
    }

    return QVariant();
}

QVariant TodoModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        switch (section) {
        case 0: return "ID";
        case 1: return "Text";
        case 2: return "Created";
        case 3: return "Updated";
        case 4: return "Done";
        }
    }
    return QVariant();
}

Qt::ItemFlags TodoModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;

    if (index.column() == 1)  // Text column editable
        return Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsSelectable;

    if (index.column() == 4)  // Done column as checkbox
        return Qt::ItemIsUserCheckable | Qt::ItemIsEnabled | Qt::ItemIsSelectable;

    // Created & Updated: read-only
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

bool TodoModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid() || index.row() >= m_todos.size())
        return false;

    Todo *todo = m_todos.at(index.row());

    if (index.column() == 1 && role == Qt::EditRole) {  // Text edit
        todo->setText(value.toString());
        todo->setUpdated(QDateTime::currentDateTime());

        repo->update(*todo);
        reload();

        emit dataChanged(index, index, {Qt::DisplayRole, Qt::EditRole});
        QModelIndex updatedIdx = this->index(index.row(), 3);
        emit dataChanged(updatedIdx, updatedIdx, {Qt::DisplayRole});
        return true;
    }

    if (index.column() == 4 && role == Qt::CheckStateRole) {  // Checkbox toggle
        todo->setDone(value.toInt() == Qt::Checked);
        todo->setUpdated(QDateTime::currentDateTime());
        repo->update(*todo);
        reload();

        emit dataChanged(index, index, {Qt::CheckStateRole, Qt::DisplayRole});
        QModelIndex updatedIdx = this->index(index.row(), 3);
        emit dataChanged(updatedIdx, updatedIdx, {Qt::DisplayRole});
        return true;
    }

    return false;
}

void TodoModel::addTodo(Todo *todo)
{
    beginInsertRows(QModelIndex(), m_todos.size(), m_todos.size());
    m_todos.append(todo);
    repo->create(*todo);
    endInsertRows();
    reload();
}

void TodoModel::addTodos(const QList<Todo*> &list)
{
    if (list.isEmpty())
        return;

    int startRow = m_todos.size();
    int endRow = startRow + list.size() - 1;

    beginInsertRows(QModelIndex(), startRow, endRow);
    for (Todo *t : list) {
        m_todos.append(t);
        repo->create(*t);
    }
    endInsertRows();
    reload();
}


void TodoModel::removeTodoAt(int row)
{
    if (row < 0 || row >= m_todos.size())
        return;

    // Grab the item first
    Todo* todo = m_todos.at(row);

    repo->remove(*todo);

    // Now update the model structure between begin/end
    beginRemoveRows(QModelIndex(), row, row);
    Todo* removed = m_todos.takeAt(row); // removes and returns pointer
    endRemoveRows();

    delete removed;
}

void TodoModel::toggleDoneTodo(int row)
{
    if (row < 0 || row >= m_todos.size())
        return;

    Todo *todo = m_todos.at(row);
    todo->setDone(!todo->getDone());
    todo->setUpdated(QDateTime::currentDateTime());

    QModelIndex doneIndex = index(row, 4);   // Done column
    QModelIndex updatedIndex = index(row, 3); // Updated column
    emit dataChanged(doneIndex, doneIndex, {Qt::CheckStateRole, Qt::DisplayRole});
    emit dataChanged(updatedIndex, updatedIndex, {Qt::DisplayRole});
}

Todo* TodoModel::getTodoAt(int row)
{
    return (row >= 0 && row < m_todos.size()) ? m_todos.at(row) : nullptr;
}

QDateTime TodoModel::minCreated() const {
    if (m_todos.isEmpty()) return QDateTime();
    QDateTime min = m_todos.first()->getCreated();
    for (auto *t : m_todos)
        if (t->getCreated() < min)
            min = t->getCreated();
    return min;
}

QDateTime TodoModel::maxCreated() const {
    if (m_todos.isEmpty()) return QDateTime();
    QDateTime max = m_todos.first()->getCreated();
    for (auto *t : m_todos)
        if (t->getCreated() > max)
            max = t->getCreated();
    return max;
}

QDateTime TodoModel::minUpdated() const {
    if (m_todos.isEmpty()) return QDateTime();
    QDateTime min = m_todos.first()->getUpdated();
    for (auto *t : m_todos)
        if (t->getUpdated() < min)
            min = t->getUpdated();
    return min;
}

QDateTime TodoModel::maxUpdated() const {
    if (m_todos.isEmpty()) return QDateTime();
    QDateTime max = m_todos.first()->getUpdated();
    for (auto *t : m_todos)
        if (t->getUpdated() > max)
            max = t->getUpdated();
    return max;
}


void TodoModel::reload() {
    beginResetModel();
    m_todos = repo->loadAll();
    endResetModel();
}

QPair<int, int> TodoModel::doneCounters()
{
    int done = 0, notDone = 0;
    for (const Todo* t : std::as_const(m_todos)) {
        if (t->getDone())
            done++;
        else
            notDone++;
    }
    return {done, notDone};
}

void TodoModel::addTodosBatch(const QList<Todo *> &batch)
{
    if(batch.isEmpty())
        return;

    int startRow = m_todos.size();
    int endRow = startRow + batch.size() - 1;

    beginInsertRows(QModelIndex(), startRow, endRow);
    for(Todo *t : batch){
        m_todos.append(t);
        repo->create(*t);
    }
    endInsertRows();
}
