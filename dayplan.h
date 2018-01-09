#ifndef DAYPLAN_H
#define DAYPLAN_H

#include <QWidget>
#include <QDateTime>

#include <limits>

namespace Ui {
class DayPlan;
}

class DayPlan : public QWidget
{
    Q_OBJECT
private slots:
    void on_applyButton_clicked();
    void on_okButton_clicked();
    void on_cancelButton_clicked();
    void on_upButton_clicked();
    void on_downButton_clicked();
    void on_delButton1_clicked();
    void on_delButton2_clicked();
    void on_delButton3_clicked();
    void on_delButton4_clicked();
    void on_delButton5_clicked();
    void on_plainTextEdit1_textChanged();
    void on_plainTextEdit2_textChanged();
    void on_plainTextEdit3_textChanged();
    void on_plainTextEdit4_textChanged();
    void on_plainTextEdit5_textChanged();
    void on_timeEdit1_timeChanged(const QTime &time);
    void on_timeEdit2_timeChanged(const QTime &time);
    void on_timeEdit3_timeChanged(const QTime &time);
    void on_timeEdit4_timeChanged(const QTime &time);
    void on_timeEdit5_timeChanged(const QTime &time);

public slots:
    void recvDumpToDay(QList<QPair<quint64,QString>> dump, QDate d);

signals:
    void sendThrowError(QString err);
    void sendDaysWorthBackToDb(const quint64 &timestamp, QList<QPair<quint64,QString>> dump);

public:
    explicit DayPlan(QWidget *parent = 0);
    ~DayPlan();

private:
    Ui::DayPlan *ui;
    QList<QPair <quint64, QString> > ents;
    quint32 currentIndex;
    QDate day;
    void updateItinerary();
    void clearBuffers();

protected:
    void closeEvent(QCloseEvent *event) override;

};

#endif // DAYPLAN_H
