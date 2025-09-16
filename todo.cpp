#include "todo.h"

QString Todo::getText() const
{
    return text;
}

void Todo::setText(const QString &newText)
{
    text = newText;
}

QDateTime Todo::getCreated() const
{
    return created;
}

void Todo::setCreated(const QDateTime &newCreated)
{
    created = newCreated;
}

QDateTime Todo::getUpdated() const
{
    return updated;
}

void Todo::setUpdated(const QDateTime &newUpdated)
{
    updated = newUpdated;
}

bool Todo::getDone() const
{
    return done;
}

void Todo::setDone(bool newDone)
{
    done = newDone;
}
