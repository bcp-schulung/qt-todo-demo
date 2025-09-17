#ifndef PIECHARTWIDGET_H
#define PIECHARTWIDGET_H

#include <QChartView>
#include <QPieSeries>
#include <QWidget>

class PieChartWidget : public QWidget
{
    Q_OBJECT
public:
    explicit PieChartWidget(QWidget *parent = nullptr);

    void setData(const QList<QPair<QString, qreal>>& data);

private:
    QChartView* chartView;
    QPieSeries* pieSeries;

signals:
};

#endif // PIECHARTWIDGET_H
