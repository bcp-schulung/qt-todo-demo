#include "todobarchartview.h"

TodoBarChartView::TodoBarChartView(QWidget* parent)
    : QGraphicsView(parent)
{
    setScene(new QGraphicsScene(this));
    setRenderHint(QPainter::Antialiasing);
}

void TodoBarChartView::setTodos(const QVector<Todo*>& todos)
{
    qDebug() << "Todos: " << todos.size();
    m_todos = todos;
    processTodos();
    viewport()->update();
}

void TodoBarChartView::processTodos()
{
    m_yearCounts.clear();
    for (const Todo* todo : m_todos) {
        int year = todo->getCreated().date().year();
        if (!m_yearCounts.contains(year)) {
            m_yearCounts[year] = qMakePair(0, 0);
        }
        if (todo->getDone())
            m_yearCounts[year].first++;    // done
        else
            m_yearCounts[year].second++;   // not done
    }
}

void TodoBarChartView::drawBackground(QPainter* painter, const QRectF& rect)
{
    drawBarChart(painter, rect);
}

void TodoBarChartView::drawBarChart(QPainter* painter, const QRectF& rect)
{
    if (m_yearCounts.isEmpty())
        return;

    // Settings
    int barWidth = 40;
    int gap = 30;
    int leftMargin = 60;
    int bottomMargin = 40;
    int topMargin = 40;

    // Find range
    QList<int> years = m_yearCounts.keys();
    std::sort(years.begin(), years.end());
    int maxCount = 0;
    for (auto pair : m_yearCounts) {
        int total = pair.first + pair.second;
        if (total > maxCount) maxCount = total;
    }

    // Bar chart area
    int chartHeight = rect.height() - bottomMargin - topMargin;

    // Draw bars
    int x = leftMargin;
    for (int year : years) {
        int done = m_yearCounts[year].first;
        int notDone = m_yearCounts[year].second;
        int total = done + notDone;

        int barMaxHeight = (maxCount > 0) ? chartHeight * total / maxCount : 0;
        int doneHeight = (total > 0) ? barMaxHeight * done / total : 0;
        int notDoneHeight = barMaxHeight - doneHeight;

        int yBase = rect.height() - bottomMargin;

        // not done (bottom)
        painter->setBrush(QColor(220, 68, 68));
        painter->drawRect(x, yBase - notDoneHeight, barWidth, notDoneHeight);

        // done (top)
        painter->setBrush(QColor(68, 180, 68));
        painter->drawRect(x, yBase - barMaxHeight, barWidth, doneHeight);

        // Year label
        painter->setPen(Qt::black);
        painter->drawText(x, yBase + 15, barWidth, 20, Qt::AlignCenter, QString::number(year));

        // Count label
        painter->drawText(x, yBase - barMaxHeight - 20, barWidth, 20, Qt::AlignCenter, QString("T:%1").arg(total));

        x += barWidth + gap;
    }

    // Draw Y axis
    painter->setPen(Qt::black);
    painter->drawLine(leftMargin - 20, rect.height() - bottomMargin, x, rect.height() - bottomMargin);

    // Draw X axis
    painter->drawLine(leftMargin - 20, rect.height() - bottomMargin, leftMargin - 20, topMargin);
    painter->drawText(leftMargin - 50, topMargin, 30, 20, Qt::AlignRight, "Count");
}
