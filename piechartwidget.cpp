#include "piechartwidget.h"

#include <QPainter>
#include <qboxlayout.h>

PieChartWidget::PieChartWidget(QWidget *parent)
    : QWidget{parent},
    chartView(new QChartView(this)),
    pieSeries(new QPieSeries())
{
    QChart *chart = new QChart();
    chart->addSeries(pieSeries);
    chart->setTitle("Done Todo Chart:");
    chart->legend()->setVisible(true);

    chartView->setChart(chart);
    chartView->setRenderHint(QPainter::Antialiasing);

    auto layout = new QVBoxLayout(this);
    layout->addWidget(chartView);
    setLayout(layout);
}

void PieChartWidget::setData(const QList<QPair<QString, qreal>>& data){
    pieSeries->clear();
    for(const auto& pair : data){
        pieSeries->append(pair.first, pair.second);
    }
}
