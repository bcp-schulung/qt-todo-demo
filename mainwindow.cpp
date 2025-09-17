#include "loadworker.h"
#include "mainwindow.h"
#include "todo.h"
#include "todomodel.h"
#include "todoproxymodel.h"
#include "todoutil.h"
#include "ui_mainwindow.h"
#include <QRandomGenerator>
#include <QMessageBox>
#include <QFutureWatcher>
#include <QtConcurrent/QtConcurrent>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //DB
    db.setDatabaseName("todos.db");
    if(!db.open()){
        qDebug() << "Error: unable to open database";
    } else{
        qDebug() << "Database opened successfully!";
    }

    TodoUtil::initDatabase(db);

    model = new TodoModel(&db, this);
    proxy = new TodoProxyModel(this);
    proxy->setSourceModel(model);

    updateDateFilters();

    ui->table->setModel(proxy);
    ui->table->setSortingEnabled(true);
    ui->table->setEditTriggers(QAbstractItemView::DoubleClicked | QAbstractItemView::SelectedClicked);
    ui->table->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->table->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->table->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    ui->table->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    ui->table->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    ui->table->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeToContents);
    ui->table->horizontalHeader()->setSectionResizeMode(4, QHeaderView::ResizeToContents);


    streamLoadTodos(1000);

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
    db.close();
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
    QModelIndexList selection = ui->table->selectionModel()->selectedRows();
    if (selection.isEmpty())
        return;

    QModelIndex proxyIndex = selection.first();
    // Map from proxy index to source model index
    QModelIndex sourceIndex = proxy->mapToSource(proxyIndex);
    int sourceRow = sourceIndex.row();
    model->removeTodoAt(sourceRow);

    updateDateFilters();
}

void MainWindow::onActionGenerateTriggered()
{
    int n = QRandomGenerator::global()->bounded(1000, 10001);

    model->clearTodos();
    QThread* thread = new QThread();
    LoaderWorker* worker = new LoaderWorker(model->repo, 500);
    worker->moveToThread(thread);

    connect(thread, &QThread::started, [=]() { worker->generate(n); });
    connect(worker, &LoaderWorker::batchReady, model, &TodoModel::addTodosBatch, Qt::QueuedConnection);
    connect(worker, &LoaderWorker::finished, thread, &QThread::quit);
    connect(worker, &LoaderWorker::finished, worker, &LoaderWorker::deleteLater);
    connect(thread, &QThread::finished, thread, &QThread::deleteLater);
    connect(worker, &LoaderWorker::finished, this, [this, n]() {
        updateDateFilters();
        QMessageBox::information(this, "Done", QString("Generated %1 todos").arg(n));
    });

    thread->start();
}

void MainWindow::streamLoadTodos(int count)
{
    model->clearTodos();
    QThread* thread = new QThread();
    LoaderWorker* worker = new LoaderWorker(model->repo, 500); // batch size
    worker->moveToThread(thread);

    connect(thread, &QThread::started, [=]() { worker->load(count); });
    connect(worker, &LoaderWorker::batchReady, model, &TodoModel::addTodosBatch, Qt::QueuedConnection);
    connect(worker, &LoaderWorker::finished, thread, &QThread::quit);
    connect(worker, &LoaderWorker::finished, worker, &LoaderWorker::deleteLater);
    connect(thread, &QThread::finished, thread, &QThread::deleteLater);
    connect(worker, &LoaderWorker::finished, this, [this]() {
        updateDateFilters();
        QMessageBox::information(this, "Done", "Loaded todos.");
    });

    thread->start();
}



