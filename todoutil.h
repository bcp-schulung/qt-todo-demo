#ifndef TODOUTIL_H
#define TODOUTIL_H

#include "todo.h"


class TodoUtil
{
public:
    TodoUtil() = default;

    static QList<Todo*> generateTodos(int count);

private:
    static QString randomLorem(int words = 6);
    static QDateTime randomDateTime();
};

#endif // TODOUTIL_H
