#include "todoproxymodel.h"

TodoProxyModel::TodoProxyModel(QObject *parent)
    : QSortFilterProxyModel(parent)
{
    setDynamicSortFilter(true);
}

void TodoProxyModel::setSearchText(const QString &text)
{
    searchText = text;
    invalidateFilter();
}

void TodoProxyModel::setCreatedFilter(const QDateTime &from, const QDateTime &to)
{
    createdFrom = from;
    createdTo = to;
    invalidateFilter();
}

void TodoProxyModel::setUpdatedFilter(const QDateTime &from, const QDateTime &to)
{
    updatedFrom = from;
    updatedTo = to;
    invalidateFilter();
}

void TodoProxyModel::setDoneFilter(int mode)
{
    doneMode = mode;
    invalidateFilter();
}

bool TodoProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    QModelIndex textIndex    = sourceModel()->index(sourceRow, 0, sourceParent);
    QModelIndex createdIndex = sourceModel()->index(sourceRow, 1, sourceParent);
    QModelIndex updatedIndex = sourceModel()->index(sourceRow, 2, sourceParent);
    QModelIndex doneIndex    = sourceModel()->index(sourceRow, 3, sourceParent);

    QString todoText = sourceModel()->data(textIndex).toString();
    QDateTime created = QDateTime::fromString(sourceModel()->data(createdIndex).toString(), "yyyy-MM-dd hh:mm");
    QDateTime updated = QDateTime::fromString(sourceModel()->data(updatedIndex).toString(), "yyyy-MM-dd hh:mm");
    bool todoDone = sourceModel()->data(doneIndex, Qt::CheckStateRole) == Qt::Checked;

    // Text filter
    if (!searchText.isEmpty() && !todoText.contains(searchText, Qt::CaseInsensitive))
        return false;

    // Created filter
    if (createdFrom.isValid() && created < createdFrom)
        return false;
    if (createdTo.isValid() && created > createdTo)
        return false;

    // Updated filter
    if (updatedFrom.isValid() && updated < updatedFrom)
        return false;
    if (updatedTo.isValid() && updated > updatedTo)
        return false;

    // Done filter
    if (doneMode == 1 && !todoDone) return false;   // only done
    if (doneMode == 2 && todoDone) return false;    // only not done

    return true;
}
