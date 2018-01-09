#include "dbmanager.h"
static void regexp(sqlite3_context *ctx, int argc, sqlite3_value **argv);

void regexp(sqlite3_context *ctx, int argc, sqlite3_value **argv){
    QRegExp regex;
    QString str1((const char*)sqlite3_value_text(argv[0]));
    QString str2((const char*)sqlite3_value_text(argv[1]));
    if(argc >= 0)
        regex.setPattern(str1);
    if (str2.contains(regex)){
        sqlite3_result_int(ctx, 1);
    }else{
        sqlite3_result_int(ctx, 0);
    }
}

DbManager::DbManager(){
    m_QSqlD = QSqlDatabase::addDatabase("QSQLITE");
    m_QSqlD.setDatabaseName(":memory:");
    m_QSqlD.open();

    QVariant v = m_QSqlD.driver()->handle();
    if (v.isValid() && qstrcmp(v.typeName(), "sqlite3*")==0) {
        sqlite3 *db_handle = *static_cast<sqlite3 **>(v.data());
        if (db_handle != 0) { // check that it is not NULL
            sqlite3_initialize();
            sqlite3_create_function(db_handle, "REGEXP", 2, SQLITE_UTF8 | SQLITE_DETERMINISTIC, NULL, regexp, NULL, NULL);
        }
    }
    createTable();
}

DbManager::~DbManager(){
    if (m_QSqlD.isOpen()){
        m_QSqlD.close();
    }
}

bool DbManager::isOpen(){
    return m_QSqlD.isOpen();
}

bool DbManager::newTable(){
    QSqlQuery query(m_QSqlD);
    query.prepare("DROP TABLE IF EXISTS entries;");
    query.exec();
    return createTable();
}

bool DbManager::doesEntryExists(quint64 timestamp, QString value){
    QSqlQuery query(m_QSqlD);
    query.prepare("SELECT * FROM entries WHERE timestamp = :timestamp AND value = :value;");
    query.bindValue(":timestamp", timestamp);
    query.bindValue(":value", value);
    if(!query.exec())
        return false;
    int timestampIndex = query.record().indexOf("timestamp");
    int valueIndex = query.record().indexOf("value");
    while (query.next()) {
        quint64 ts = query.value(timestampIndex).toULongLong();
        QString vlu = query.value(valueIndex).toString();
        if(ts == timestamp && vlu == value)
            return true;
    }
    return false;
}


bool DbManager::insert(quint64 timestamp, QString value){
    QSqlQuery q(m_QSqlD);
    q.prepare("INSERT OR IGNORE INTO entries (timestamp, value) VALUES (:timestamp, :value);");
    q.bindValue(":timestamp", timestamp);
    q.bindValue(":value", value, QSql::In|QSql::Binary);
    if(q.exec() == false){
        qDebug() << "DbManager::insert " << q.lastError().text();
        return false;
    }
    return true;
}

QList<QPair<quint64,QString>> DbManager::dump(quint64 from, quint64 to){
    QList<QPair<quint64,QString>> ret;
    QSqlQuery query(m_QSqlD);
    if(from == 0 && to == 0){
        query.prepare("SELECT * FROM entries;");
    }else if(from == 0){
        query.prepare("SELECT * FROM entries WHERE timestamp < :ts_to;");
        query.bindValue(":ts_to", to);
    }else if(to == 0){
        query.prepare("SELECT * FROM entries WHERE timestamp >= :ts_from;");
        query.bindValue(":ts_from", from);
    }else{
        query.prepare("SELECT * FROM entries WHERE timestamp < :ts_to AND timestamp >= :ts_from;");
        query.bindValue(":ts_to", to);
        query.bindValue(":ts_from", from);
    }
    if(!query.exec())
        return QList<QPair<quint64,QString>>();
    int timestampIndex = query.record().indexOf("timestamp");
    int valueIndex = query.record().indexOf("value");
    while (query.next()) {
        quint64 ts = query.value(timestampIndex).toULongLong();
        QString vlu = query.value(valueIndex).toString();
        QPair<quint64,QString> tmp;
        tmp. first = ts;
        tmp.second = vlu;
        ret.append(tmp);
    }
    return ret;
}

bool DbManager::deleteFrom(quint64 from, quint64 to){
    QSqlQuery query(m_QSqlD);
    if(from == 0 && to == 0){
        query.prepare("DELETE FROM entries;");
    }else if(from == 0){
        query.prepare("DELETE FROM entries WHERE timestamp < :ts_to;");
        query.bindValue(":ts_to", to);
    }else if(to == 0){
        query.prepare("DELETE FROM entries WHERE timestamp >= :ts_from;");
        query.bindValue(":ts_from", from);
    }else{
        query.prepare("DELETE FROM entries WHERE timestamp < :ts_to AND timestamp >= :ts_from;");
        query.bindValue(":ts_to", to);
        query.bindValue(":ts_from", from);
    }
    return query.exec();
}


bool DbManager::createTable(){
    QSqlQuery query(m_QSqlD);
    query.prepare("CREATE TABLE IF NOT EXISTS entries(timestamp INTEGER NOT NULL, value TEXT NOT NULL);");
    if (!query.exec()){
        return false;
    }
    return true;
}

bool DbManager::beginTrans(){
    return m_QSqlD.transaction();
}

bool DbManager::commitTrans(){
    return m_QSqlD.commit();
}
