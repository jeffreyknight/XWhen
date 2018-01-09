#ifndef MONTHPLAN_H
#define MONTHPLAN_H

#include <QWidget>
#include <QCalendarWidget>
#include <QPen>
#include <QBrush>
#include <QPainter>
#include <QCloseEvent>
#include <QIcon>

class MonthPlan : public QCalendarWidget
{
    Q_OBJECT
private slots:
    void on_calendarWidget_clicked(const QDate &date);
    void on_calendarWidget_currentPageChanged(int year, int month);

public slots:
    void recvDatesToCalender(QList<QDate> days);
    void updateCalender();

signals:
    void sendTimestamp(const quint64 &timestamp);
    void sendMonthYear(int year, int month);
    void sendCloseForm();

public:
    explicit MonthPlan(QWidget *parent = 0);
    ~MonthPlan();

protected:
    virtual void paintCell(QPainter *painter, const QRect &rect, const QDate &date) const;
    void closeEvent(QCloseEvent *event) override;

private:
    QList<QDate> dates;
};

#endif // MONTHPLAN_H
