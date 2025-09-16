#include "mainwindow.h"
#include "todo.h"
#include "todomodel.h"
#include "todoproxymodel.h"
#include "todoutil.h"
#include "ui_mainwindow.h"
#include <QRandomGenerator>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    model = new TodoModel(this);
    proxy = new TodoProxyModel(this);
    proxy->setSourceModel(model);

    updateDateFilters();

    ui->table->setModel(proxy);
    ui->table->setSortingEnabled(true);
    ui->table->setEditTriggers(QAbstractItemView::DoubleClicked | QAbstractItemView::SelectedClicked);
    ui->table->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->table->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->table->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    ui->table->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    ui->table->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    ui->table->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeToContents);

    connect(ui->createButton, &QPushButton::clicked, this, &MainWindow::addTask);
    connect(ui->deleteButton, &QPushButton::clicked, this, &MainWindow::removeTask);

    connect(ui->actionGenerate, &QAction::triggered, this, &MainWindow::onActionGenerateTriggered);

    connect(ui->lineEditSearch, &QLineEdit::textChanged,
            proxy, &TodoProxyModel::setSearchText);

    connect(ui->dateFrom, &QDateTimeEdit::dateTimeChanged,
            this, [this](const QDateTime &dt){ proxy->setDateRange(dt, ui->dateTo->dateTime()); });

    connect(ui->dateTo, &QDateTimeEdit::dateTimeChanged,
            this, [this](const QDateTime &dt){ proxy->setDateRange(ui->dateFrom->dateTime(), dt); });

    connect(ui->comboDoneFilter, QOverload<int>::of(&QComboBox::currentIndexChanged),
            proxy, &TodoProxyModel::setDoneFilter);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::updateDateFilters()
{
    if (!model || model->rowCount() == 0)
        return;

    QDateTime minCreated = model->minCreated();
    QDateTime maxCreated = model->maxCreated();
    QDateTime minUpdated = model->minUpdated();
    QDateTime maxUpdated = model->maxUpdated();

    // Take earliest of created/updated and latest of both
    QDateTime min = (minCreated < minUpdated) ? minCreated : minUpdated;
    QDateTime max = (maxCreated > maxUpdated) ? maxCreated : maxUpdated;

    // 🔹 Adjust: make "from" 1 minute before the earliest created
    if (minCreated.isValid())
        min = minCreated.addSecs(-60);

    ui->dateFrom->setDateTime(min);
    ui->dateTo->setDateTime(max);

    proxy->setDateRange(min, max);
}

void MainWindow::addTask() {
    QString taskText = ui->lineEdit->text().trimmed();

    if (taskText.isEmpty())
        return;

    Todo *todo = new Todo(taskText);
    model->addTodo(todo);

    updateDateFilters();

    ui->lineEdit->clear();
}

void MainWindow::removeTask() {
    auto *model = qobject_cast<TodoModel*>(ui->table->model());
    if (!model) return;

    QModelIndexList selection = ui->table->selectionModel()->selectedRows();
    if (selection.isEmpty())
        return;

    updateDateFilters();

    int row = selection.first().row();
    model->removeTodoAt(row);
}

void MainWindow::onActionGenerateTriggered()
{
    int n = QRandomGenerator::global()->bounded(1000, 10001);
    QList<Todo*> todos = TodoUtil::generateTodos(n);

    model->addTodos(todos);

    QMessageBox::information(this, "Done", QString("Generated %1 todos").arg(n));
}

