#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow){

    ui->setupUi(this);
    saltsize = 256;
    year = 0;
    month = 0;
    db = new DbManager();
    mp = new MonthPlan();
    connect(mp, &MonthPlan::sendTimestamp, this, &MainWindow::recvTimestamp);
    connect(mp, &MonthPlan::sendMonthYear, this, &MainWindow::recvMonthYear);
    connect(this, &MainWindow::sendDatesToCalender, mp, &MonthPlan::recvDatesToCalender);
    connect(mp, &MonthPlan::sendCloseForm, this, &MainWindow::recvCloseForm);
    connect(this, &MainWindow::updateCalender, mp, &MonthPlan::updateCalender);

    dp = new DayPlan();
    connect(this, &MainWindow::sendDumpToDay, dp, &DayPlan::recvDumpToDay);
    connect(dp, &DayPlan::sendThrowError, this, &MainWindow::recvThrowError);
    connect(dp, &DayPlan::sendDaysWorthBackToDb, this, &MainWindow::recvDaysWorthBackToDb);
}

MainWindow::~MainWindow(){
    delete ui;
}

void MainWindow::wipePassword(){
    for(int i = 0; i < 4096; i++)
        pw[i] = 0;
    pw = QByteArray();
}

void MainWindow::wipeSalt(){
    for(int i = 0; i < saltsize; i++){
        salt1[i] = 0;
        salt2[i] = 0;
    }
    salt1 = QByteArray();
    salt2 = QByteArray();
}

void MainWindow::throwError(QString err){
    db->newTable();
    wipePassword();
    wipeSalt();
    year = 0;
    month = 0;
    ui->lineEditFilename->setText(tr(""));
    mp->hide();
    dp->hide();
    if(!err.isEmpty())
        QMessageBox::critical(this, windowTitle(), err);
}

void MainWindow::parseFile(QByteArray filecontents){
    QStringList qlqba = QString(filecontents).split("\n");
    int n = qlqba.length();
    for(int i = 0; i < n; i++){
        QString line = qlqba[i];
        if(line.isEmpty())
            continue;
        if(QRegExp("^[0-9a-f]+,[0-9a-zA-Z+/=]+$").indexIn(line, QRegExp::CaretAtZero) == -1){
            throwError("Error: database invalid.");
            return;
        }
        bool ok = false;
        quint64 ts = line.split(",")[0].toULongLong(&ok, 16);
        if(!ok){
            throwError("Error: database invalid. (bad timestamp)");
            return;
        }
        if(!isBase64(line.split(",")[1])){
            throwError("Error: database invalid. (bad base64)");
            return;
        }
        QByteArray value = QByteArray::fromBase64(line.split(",")[1].toLatin1());
        if(!db->insert(ts, QString(value))){
            throwError("Error: insert into database failed.");
            return;
        }
    }
}

bool MainWindow::isBase64(QString i){
    if(i.length() % 4 != 0 || QRegExp("^[0-9a-zA-Z+/]+=?=?$").indexIn(i, QRegExp::CaretAtZero) == -1) return false;
    return true;
}

bool MainWindow::saveDb(){
    if(pw.isEmpty() || salt1.isEmpty() || salt2.isEmpty() || saltsize == 0)
        return false;
    QFile file(ui->lineEditFilename->text());
    QByteArray iv;
    QByteArray key;
    OpenSSL::sha256(pw, key, salt1);
    OpenSSL::sha256(pw, iv, salt2);
    iv = iv.mid(0, 16);
    QByteArray ciphertext;
    QList<QPair<quint64,QString>> list = db->dump();
    QByteArray dump;
    QListIterator<QPair<quint64,QString>> it(list);
    while (it.hasNext()){
        QPair<quint64,QString> tmp = it.next();
        QByteArray line = QByteArray::number(tmp.first, 16)+QByteArray(",")+QByteArray().append(QByteArray().append(tmp.second).toBase64())+QByteArray("\n");
        dump += line;
    }
    if(dump.isEmpty())
        dump = QByteArray("\n\n\n\n", 4);
    quint64 ciphertext_len = OpenSSL::encrypt(dump, key, iv, ciphertext);
    if(file.open(QFile::WriteOnly | QFile::Truncate)){
        file.write(salt1, saltsize);
        file.write(salt2, saltsize);
        file.write(ciphertext, ciphertext_len);
        file.close();
    }else{
        throwError("Error: Cannot open file for writing.");
    }
    return true;
}

void MainWindow::on_actionOpen_triggered(){
    on_pushButtonFile_clicked();
}

void MainWindow::on_actionClose_triggered(){
    throwError();
}

void MainWindow::on_actionExit_triggered(){
    qApp->quit();
}

void MainWindow::on_actionAdd_Holidays_triggered(){
    if(year == 0)
        return;
    if(db->isOpen()){
        QList<QPair<QDate,QString>> holidays = {
            QPair<QDate,QString>{QDate(1,1,1),tr("New Year's Day")},
            QPair<QDate,QString>{QDate(1,1,15),tr("Martin Luther King Jr. Day")},
            QPair<QDate,QString>{QDate(1,2,2),tr("Groundhog Day")},
            QPair<QDate,QString>{QDate(1,2,14),tr("Valentine's Day")},
            QPair<QDate,QString>{QDate(1,2,15),tr("Washington's Birthday")},
            QPair<QDate,QString>{QDate(1,3,17),tr("Saint Patrick's Day")},
            QPair<QDate,QString>{QDate(1,4,1),tr("April Fool's Day")},
            QPair<QDate,QString>{QDate(1,4,22),tr("Earth Day")},
            QPair<QDate,QString>{QDate(1,4,24),tr("Arbor Day")},
            QPair<QDate,QString>{QDate(1,5,5),tr("Cinco De Mayo")},
            QPair<QDate,QString>{QDate(1,5,8),tr("Mother's Day")},
            QPair<QDate,QString>{QDate(1,5,15),tr("Armed Forces Day")},
            QPair<QDate,QString>{QDate(1,5,25),tr("Memorial Day")},
            QPair<QDate,QString>{QDate(1,6,14),tr("Flag Day")},
            QPair<QDate,QString>{QDate(1,6,15),tr("Father's Day")},
            QPair<QDate,QString>{QDate(1,7,4),tr("Independence Day")},
            QPair<QDate,QString>{QDate(1,9,1),tr("Labor Day")},
            QPair<QDate,QString>{QDate(1,10,8),tr("Columbus Day")},
            QPair<QDate,QString>{QDate(1,10,31),tr("Halloween")},
            QPair<QDate,QString>{QDate(1,11,11),tr("Veterans Day")},
            QPair<QDate,QString>{QDate(1,11,22),tr("Thanksgiving Day")},
            QPair<QDate,QString>{QDate(1,12,7),tr("National Pearl Harbor Remembrance Day")},
            QPair<QDate,QString>{QDate(1,12,25),tr("Christmas Day")},
            QPair<QDate,QString>{QDate(1,12,31),tr("New Year's Eve")},
        };
        //You try to calculate the lunisolar calendar...
        QList<QDate> easter_dates = {QDate(2000,4,23),QDate(2001,4,15),QDate(2002,3,31),QDate(2003,4,20),QDate(2004,4,11),QDate(2005,3,27),QDate(2006,4,16),QDate(2007,4,8),QDate(2008,3,23),QDate(2009,4,12),QDate(2010,4,4),QDate(2011,4,24),QDate(2012,4,8),QDate(2013,3,31),QDate(2014,4,20),QDate(2015,4,5),QDate(2016,3,27),QDate(2017,4,16),QDate(2018,4,1),QDate(2019,4,21),QDate(2020,4,12),QDate(2021,4,4),QDate(2022,4,17),QDate(2023,4,9),QDate(2024,3,31),QDate(2025,4,20),QDate(2026,4,5),QDate(2027,3,28),QDate(2028,4,16),QDate(2029,4,1),QDate(2030,4,21),QDate(2031,4,13),QDate(2032,3,28),QDate(2033,4,17),QDate(2034,4,9),QDate(2035,3,25),QDate(2036,4,13),QDate(2037,4,5),QDate(2038,4,25),QDate(2039,4,10),QDate(2040,4,1),QDate(2041,4,21),QDate(2042,4,6),QDate(2043,3,29),QDate(2044,4,17),QDate(2045,4,9),QDate(2046,3,25),QDate(2047,4,14),QDate(2048,4,5),QDate(2049,4,18),QDate(2050,4,10),QDate(2051,4,2),QDate(2052,4,21),QDate(2053,4,6),QDate(2054,3,29),QDate(2055,4,18),QDate(2056,4,2),QDate(2057,4,22),QDate(2058,4,14),QDate(2059,3,30),QDate(2060,4,18),QDate(2061,4,10),QDate(2062,3,26),QDate(2063,4,15),QDate(2064,4,6),QDate(2065,3,29),QDate(2066,4,11),QDate(2067,4,3),QDate(2068,4,22),QDate(2069,4,14),QDate(2070,3,30),QDate(2071,4,19),QDate(2072,4,10),QDate(2073,3,26),QDate(2074,4,15),QDate(2075,4,7),QDate(2076,4,19),QDate(2077,4,11),QDate(2078,4,3),QDate(2079,4,23),QDate(2080,4,7),QDate(2081,3,30),QDate(2082,4,19),QDate(2083,4,4),QDate(2084,3,26),QDate(2085,4,15),QDate(2086,3,31),QDate(2087,4,20),QDate(2088,4,11),QDate(2089,4,3),QDate(2090,4,16),QDate(2091,4,8),QDate(2092,3,30),QDate(2093,4,12),QDate(2094,4,4),QDate(2095,4,24),QDate(2096,4,15),QDate(2097,3,31),QDate(2098,4,20),QDate(2099,4,12),QDate(2100,3,28)};
        QStringList floatinghdays_mon = {
            tr("Martin Luther King Jr. Day"),
            tr("Washington's Birthday"),
            tr("Memorial Day"),
            tr("Labor Day"),
            tr("Columbus Day"),
        };
        QStringList floatinghdays_thur = {
            tr("Thanksgiving Day"),
        };
        QStringList floatinghdays_fri = {
            tr("Arbor Day"),
        };
        QStringList floatinghdays_sat = {
            tr("Armed Forces Day"),
        };
        QStringList floatinghdays_sun = {
            tr("Mother's Day"),
            tr("Father's Day"),
        };
        for(int i = 0; i < holidays.length(); i++){
            QPair<QDate,QString> tmp = holidays.at(i);
            QDate datetmp(year, tmp.first.month(), tmp.first.day());
            if(floatinghdays_mon.contains(tmp.second)){
                while(datetmp.dayOfWeek() != Qt::Monday)
                    datetmp = datetmp.addDays(1);
            }
            if(floatinghdays_thur.contains(tmp.second)){
                while(datetmp.dayOfWeek() != Qt::Thursday)
                    datetmp = datetmp.addDays(1);
            }
            if(floatinghdays_fri.contains(tmp.second)){
                while(datetmp.dayOfWeek() != Qt::Friday)
                    datetmp = datetmp.addDays(1);
            }
            if(floatinghdays_sat.contains(tmp.second)){
                while(datetmp.dayOfWeek() != Qt::Saturday)
                    datetmp = datetmp.addDays(1);
            }
            if(floatinghdays_sun.contains(tmp.second)){
                while(datetmp.dayOfWeek() != Qt::Sunday)
                    datetmp = datetmp.addDays(1);
            }
            if(!db->doesEntryExists(QDateTime(datetmp, QTime()).toTime_t(), tmp.second)){
                db->insert(QDateTime(datetmp, QTime()).toTime_t(), tmp.second);
            }
        }
        for(int i = 0; i < easter_dates.length(); i++){
            if(easter_dates.at(i).year() == year){
                if(!db->doesEntryExists(QDateTime(easter_dates.at(i).addDays(-7), QTime()).toTime_t(), tr("Palm Sunday"))){
                    db->insert(QDateTime(easter_dates.at(i).addDays(-7), QTime()).toTime_t(), tr("Palm Sunday"));
                }
                if(!db->doesEntryExists(QDateTime(easter_dates.at(i), QTime()).toTime_t(), tr("Easter"))){
                    db->insert(QDateTime(easter_dates.at(i), QTime()).toTime_t(), tr("Easter"));
                }
                break;
            }
        }
        if(!saveDb())
            throwError("Failed to save database. Closing down.");
        updateCalender();
    }
}

void MainWindow::on_actionAbout_triggered(){
    QMessageBox::information(this, windowTitle(), tr("A calendar program"));
}

void MainWindow::openGetPasswordDialog(){
    QDialog *qd = new QDialog(this);
    qd->setModal(true);
    QHBoxLayout *qhbl = new QHBoxLayout();
    qd->setLayout(qhbl);
    QLabel *qlel = new QLabel("Password");
    qhbl->addWidget(qlel);
    QLineEdit *qle = new QLineEdit();
    qle->setFixedWidth(200);
    qle->setEchoMode(QLineEdit::Password);
    qhbl->addWidget(qle);
    QPushButton *ok = new QPushButton("Ok");
    qhbl->addWidget(ok);
    QPushButton *cancel = new QPushButton("Cancel");
    qhbl->addWidget(cancel);
    qd->show();
    connect(ok, &QPushButton::clicked, qd, &QDialog::accept);
    connect(cancel, &QPushButton::clicked, qd, &QDialog::reject);
    if(qd->exec()){
        pw = qle->text().toLatin1();
    }
}

void MainWindow::on_pushButtonFile_clicked(){
    throwError();
    ui->lineEditFilename->setText(QFileDialog::getOpenFileName(this, windowTitle(),
        QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation), Q_NULLPTR));
    QFile file(ui->lineEditFilename->text());
    if(file.exists()){
        if(file.open(QFile::ReadWrite)){
            QByteArray filebytes = Q_NULLPTR;
            filebytes = file.readAll();

            if(filebytes.size()<=2*saltsize){
                QMessageBox::critical(this, windowTitle(), tr("File size is too small to be a database."));
                wipePassword();
                ui->lineEditFilename->setText(tr(""));
                file.close();
                return;
            }
            openGetPasswordDialog();

            if(pw.isEmpty()){
                throwError();
                return;
            }
            QByteArray ciphertext = Q_NULLPTR;
            QByteArray decryptedtext = Q_NULLPTR;
            int decryptedtext_len = 0;

            salt1 = filebytes.mid(0,saltsize);
            salt2 = filebytes.mid(saltsize,saltsize);
            ciphertext = filebytes.mid(2*saltsize);

            QByteArray iv;
            QByteArray key;
            OpenSSL::sha256(pw, key, salt1);
            OpenSSL::sha256(pw, iv, salt2);
            iv = iv.mid(0, 16);

            decryptedtext_len = OpenSSL::decrypt(ciphertext, key, iv, decryptedtext);
            if(decryptedtext_len <= 0){
                file.close();
                throwError("unable to decrypt database with the given password.");
                return;
            }
            parseFile(decryptedtext);
            file.close();
            mp->show();
            QDate current = QDateTime::currentDateTime().date();
            recvMonthYear(current.year(), current.month());
        }else{
            throwError("Could not open file.");
            return;
        }
    }else{
        throwError();
    }
}

void MainWindow::on_actionNew_triggered(){
    throwError();
    ui->lineEditFilename->setText(QFileDialog::getSaveFileName(this, windowTitle(), Q_NULLPTR, Q_NULLPTR, Q_NULLPTR, QFileDialog::DontConfirmOverwrite));
    if(ui->lineEditFilename->text().isEmpty())
        return;
    QFile file(ui->lineEditFilename->text());
    if(!file.exists()){
        QTime time = QTime::currentTime();
        qsrand((uint)time.msec());

        for(int i = 0; i < saltsize; i++){
            salt1[i] = (qrand() % saltsize);
            salt2[i] = (qrand() % saltsize);
        }

        if(salt1.size() != saltsize || salt2.size() != saltsize){
            throwError("Internal Error: salt lengths incorrect. try again.");
            return;
        }

        QDialog *qd = new QDialog(this);
        qd->setModal(true);
        QVBoxLayout *qvbl = new QVBoxLayout();
        qd->setLayout(qvbl);
        QHBoxLayout *h1 = new QHBoxLayout();
        QLabel *qlel = new QLabel("Password:");
        QLineEdit *qle = new QLineEdit();
        qle->setFixedWidth(200);
        qle->setEchoMode(QLineEdit::Password);
        h1->addWidget(qlel);
        h1->addWidget(qle);
        qvbl->addLayout(h1);
        QHBoxLayout *h2 = new QHBoxLayout();
        QLabel *qlel_repeat = new QLabel("Repeat:");
        QLineEdit *qle_repeat = new QLineEdit();
        qle_repeat->setFixedWidth(200);
        qle_repeat->setEchoMode(QLineEdit::Password);
        h2->addWidget(qlel_repeat);
        h2->addWidget(qle_repeat);
        qvbl->addLayout(h2);
        QHBoxLayout *qhbl = new QHBoxLayout();
        QPushButton *ok = new QPushButton("Ok");
        qhbl->addWidget(ok);
        QPushButton *cancel = new QPushButton("Cancel");
        qhbl->addWidget(cancel);
        qvbl->addLayout(qhbl);
        qd->show();
        connect(ok, &QPushButton::clicked, qd, &QDialog::accept);
        connect(cancel, &QPushButton::clicked, qd, &QDialog::reject);
        if(qd->exec()){
            if(qle->text().toLatin1() != qle_repeat->text().toLatin1()){
                throwError("Passwords do not match.");
                return;
            }
        }else{
            throwError();
            return;
        }
        pw = qle->text().toLatin1();
        if(pw.isEmpty())
            throwError("Password can't be empty.");
        if(saveDb())
            mp->show();
        updateCalender();

    }else{
        throwError("File already exists. Delete the file and try again.");
        return;
    }
}

void MainWindow::recvTimestamp(const quint64 &begin){
    quint64 end = QDateTime::fromTime_t(begin).addDays(1).toTime_t() - 1;
    sendDumpToDay(db->dump(begin, end), QDateTime::fromTime_t(begin).date());
}

void MainWindow::recvMonthYear(int year, int month){
    this->year = year;
    this->month = month;
    quint64 begin = QDateTime(QDate(year, month, 1)).toTime_t();
    quint64 end = QDateTime(QDate(year, month, 1).addMonths(1)).toTime_t() - 1;
    QList<QPair<quint64,QString>> rd = db->dump(begin, end);
    QListIterator<QPair<quint64,QString>> it(rd);
    QList<QDate> d;
    while (it.hasNext()){
        d.append(QDateTime::fromTime_t(it.next().first).date());
    }
    sendDatesToCalender(d);
}

void MainWindow::recvDaysWorthBackToDb(const quint64 &timestamp, QList<QPair<quint64,QString>> dump){
    quint64 end = QDateTime::fromTime_t(timestamp).addDays(1).toTime_t() - 1;
    db->deleteFrom(timestamp, end);
    QListIterator<QPair<quint64,QString>> it(dump);
    while (it.hasNext()){
        QPair<quint64,QString> tmp = it.next();
        db->insert(tmp.first, tmp.second);
    }
    if(!saveDb())
        throwError("Failed to save database. Closing down.");
    QDate qd = QDateTime::fromTime_t(timestamp).date();
    recvMonthYear(qd.year(), qd.month());
}

void MainWindow::recvCloseForm(){
    throwError();
}

void MainWindow::recvThrowError(QString err){
    throwError(err);
}

void MainWindow::closeEvent(QCloseEvent *event){
    qApp->quit();
    event->accept();
}

