#ifndef DBMANAGER_H
#define DBMANAGER_H

#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>
#include <QDebug>
#include <QSqlDatabase>
#include <QSqlQueryModel>
#include <QSqlDriver>


#include "sqlite3.h"

class DbManager: public QObject
{
    Q_OBJECT

public:
    DbManager();
    ~DbManager();
    bool beginTrans();
    bool commitTrans();
    bool newTable();
    bool isOpen();
    bool doesEntryExists(quint64 timestamp, QString value);
    bool insert(quint64 timestamp, QString value);
    QList<QPair<quint64,QString>> dump(quint64 from = 0, quint64 to = 0);
    bool deleteFrom(quint64 from = 0, quint64 to = 0);

private:
    QSqlDatabase m_QSqlD;
    bool createTable();
};

#endif // DBMANAGER_H
