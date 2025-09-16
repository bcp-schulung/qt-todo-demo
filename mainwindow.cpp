#include "mainwindow.h"
#include "todo.h"
#include "todomodel.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    auto *model = new TodoModel();

    ui->table->setModel(model);
    ui->table->setEditTriggers(QAbstractItemView::DoubleClicked | QAbstractItemView::SelectedClicked);
    ui->table->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->table->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->table->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    ui->table->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    ui->table->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    ui->table->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeToContents);

    connect(ui->createButton, &QPushButton::clicked, this, &MainWindow::addTask);
    connect(ui->deleteButton, &QPushButton::clicked, this, &MainWindow::removeTask);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::addTask() {
    QString taskText = ui->lineEdit->text().trimmed();

    if (taskText.isEmpty())
        return;

    auto *model = qobject_cast<TodoModel*>(ui->table->model());
    if (!model) return;

    Todo *todo = new Todo(taskText);
    model->addTodo(todo);

    ui->lineEdit->clear();
}

void MainWindow::removeTask() {
    auto *model = qobject_cast<TodoModel*>(ui->table->model());
    if (!model) return;

    QModelIndexList selection = ui->table->selectionModel()->selectedRows();
    if (selection.isEmpty())
        return;

    int row = selection.first().row();
    model->removeTodoAt(row);
}

