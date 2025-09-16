#ifndef TODOPROXYMODEL_H
#define TODOPROXYMODEL_H

#include <QDateTime>
#include <QSortFilterProxyModel>


class TodoProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    explicit TodoProxyModel(QObject *parent = nullptr);

    void setSearchText(const QString &text);
    void setCreatedFilter(const QDateTime &from, const QDateTime &to);
    void setUpdatedFilter(const QDateTime &from, const QDateTime &to);
    void setDoneFilter(int mode);

protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const override;

private:
    QString searchText;
    QDateTime createdFrom, createdTo;
    QDateTime updatedFrom, updatedTo;
    int doneMode = 0;
};

#endif // TODOPROXYMODEL_H
