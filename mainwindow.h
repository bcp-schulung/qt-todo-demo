#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "todomodel.h"
#include "todoproxymodel.h"
#include <QMainWindow>
#include <QSqlDatabase>
#include <QTableWidgetItem>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void addTask();
    void removeTask();
    void onActionGenerateTriggered();
    void updateDateFilters();
    void updatePieChart();

private:
    Ui::MainWindow *ui;
    TodoModel *model;
    TodoProxyModel *proxy;
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
};
#endif // MAINWINDOW_H
