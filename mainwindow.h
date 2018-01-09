#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QCloseEvent>
#include <QMainWindow>
#include <QMessageBox>
#include <QFileDialog>
#include <QTime>
#include <QLabel>
#include <QStandardPaths>

#include "dbmanager.h"
#include "monthplan.h"
#include "dayplan.h"

#include "openssl.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
private slots:
    void on_actionNew_triggered();
    void on_actionOpen_triggered();
    void on_actionClose_triggered();
    void on_actionExit_triggered();
    void on_actionAdd_Holidays_triggered();
    void on_actionAbout_triggered();
    void on_pushButtonFile_clicked();

public slots:
    void recvTimestamp(const quint64 &begin);
    void recvMonthYear(int year, int month);
    void recvCloseForm();
    void recvThrowError(QString err);
    void recvDaysWorthBackToDb(const quint64 &timestamp, QList<QPair<quint64,QString>> dump);

signals:
    void sendDatesToCalender(QList<QDate> days);
    void sendDumpToDay(QList<QPair<quint64,QString>> dump, QDate d);
    void updateCalender();

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    DayPlan *dp;
    MonthPlan *mp;
    DbManager *db;
    QByteArray pw;
    QByteArray salt1;
    QByteArray salt2;
    int saltsize;
    int year;
    int month;

    bool saveDb();
    void openGetPasswordDialog();
    void parseFile(QByteArray filecontents);
    bool isBase64(QString input);
    void wipePassword();
    void wipeSalt();
    void throwError(QString err = QString());

protected:
    void closeEvent(QCloseEvent *event) override;
};

#endif // MAINWINDOW_H
