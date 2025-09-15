#include "mainwindow.h"
#include "todo.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->table->setColumnCount(3);
    QStringList headers;
    headers << "Task" << "Created" << "Updated";
    ui->table->setHorizontalHeaderLabels(headers);
    ui->table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    connect(ui->createButton, &QPushButton::clicked, this, &MainWindow::addTask);
    connect(ui->deleteButton, &QPushButton::clicked, this, &MainWindow::removeTask);
    connect(ui->table, &QTableWidget::itemChanged, this, &MainWindow::handleItemChanged);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::addTask() {
    QString taskText = ui->lineEdit->text();
    Todo todo(taskText);

    int row = ui->table->rowCount();
    ui->table->insertRow(row);

    ui->table->blockSignals(true);

    ui->table->setItem(row, 0, new QTableWidgetItem(todo.getText()));
    ui->table->setItem(row, 1, new QTableWidgetItem(todo.getCreated().toString("yyyy-MM-dd hh:mm:ss")));
    ui->table->setItem(row, 2, new QTableWidgetItem(todo.getUpdated().toString("yyyy-MM-dd hh:mm:ss")));

    ui->table->blockSignals(false);

    qDebug() << "Task created:" << todo.getText();
    ui->lineEdit->clear();
}

void MainWindow::removeTask() {
    int currentRow = ui->table->currentRow();
    if (currentRow >= 0) {
        ui->table->removeRow(currentRow);
        qDebug() << "Task removed at row" << currentRow;
    } else {
        qDebug() << "No task selected to remove!";
    }
}

void MainWindow::handleItemChanged(QTableWidgetItem *item)
{
    int row = item->row();
    int col = item->column();

    if(col == 0){
        QString now = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
        ui->table->setItem(row, 2, new QTableWidgetItem(now));
        qDebug() << "Row: " << row << "now updated at" << now;
    }
}
