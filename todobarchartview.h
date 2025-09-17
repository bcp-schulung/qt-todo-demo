#ifndef TODOBARCHARTVIEW_H
#define TODOBARCHARTVIEW_H

#include <Todo.h>
#include <qgraphicsview.h>


class TodoBarChartView : public QGraphicsView
{
    Q_OBJECT
public:
    explicit TodoBarChartView(QWidget* parent = nullptr);

    void setTodos(const QVector<Todo*>& todos);

protected:
    void drawBackground(QPainter* painter, const QRectF& rect) override;

private:
    QVector<Todo*> m_todos;
    QMap<int, QPair<int, int>> m_yearCounts; // year -> (done, not done)

    void processTodos();
    void drawBarChart(QPainter* painter, const QRectF& rect);
};
#endif // TODOBARCHARTVIEW_H
