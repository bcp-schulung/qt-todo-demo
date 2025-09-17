#ifndef LOADWORKER_H
#define LOADWORKER_H

#include "todo.h"
#include "todorepository.h"
#include "todoutil.h"

#include <QObject>
#include <qthread.h>

class LoaderWorker : public QObject {
    Q_OBJECT
public:
    LoaderWorker(TodoRepository *repo, int batchSize, QObject *parent = nullptr)
        : QObject(parent), m_repo(repo), m_batchSize(batchSize) {}

signals:
    void batchReady(QList<Todo*> batch, bool persist);
    void finished();

public slots:
    // Stream loading from database
    void load(int totalCount) {
        int offset = 0;
        int remaining = totalCount;
        while (remaining > 0) {
            QList<Todo*> batch = m_repo->loadBatch(offset, qMin(m_batchSize, remaining));
            if (batch.isEmpty())
                break;
            emit batchReady(batch, false); // loaded, don't save again
            offset += batch.size();
            remaining -= batch.size();
            QThread::msleep(10); // let UI process events
        }
        emit finished();
    }

    // Stream generating new todos
    void generate(int totalCount) {
        int generated = 0;
        while (generated < totalCount) {
            int thisBatch = qMin(m_batchSize, totalCount - generated);
            QList<Todo*> batch = TodoUtil::generateTodos(thisBatch);
            emit batchReady(batch, true); // generated, save to DB
            generated += thisBatch;
            QThread::msleep(10);
        }
        emit finished();
    }

private:
    TodoRepository *m_repo;
    int m_batchSize;
};

#endif // LOADWORKER_H
