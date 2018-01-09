#include "monthplan.h"

MonthPlan::MonthPlan(QWidget *parent) :
QCalendarWidget(parent){
    setWindowIcon(QIcon(":/xwhen.ico"));
    connect(this, &MonthPlan::clicked, this, &MonthPlan::on_calendarWidget_clicked);
    connect(this, &MonthPlan::currentPageChanged, this, &MonthPlan::on_calendarWidget_currentPageChanged);
}

MonthPlan::~MonthPlan(){

}

void MonthPlan::on_calendarWidget_clicked(const QDate &date){
    QDateTime qdt(date);
    sendTimestamp((quint64)qdt.toTime_t());
}

void MonthPlan::on_calendarWidget_currentPageChanged(int year, int month){
    sendMonthYear(year, month);
}

void MonthPlan::closeEvent(QCloseEvent *event){
    sendCloseForm();
    event->ignore();
}

void MonthPlan::paintCell(QPainter *painter, const QRect &rect, const QDate &date) const {
    QCalendarWidget::paintCell(painter, rect, date);
    if(dates.contains(date)){
        painter->setPen(QPen(Qt::red, 3, Qt::DashDotLine, Qt::RoundCap, Qt::RoundJoin));
        painter->drawRect(rect.adjusted(0, 0, -1, -1));
    }
}

void MonthPlan::recvDatesToCalender(QList<QDate> days){
    dates = days;
    update();
}

void MonthPlan::updateCalender(){
    sendMonthYear(yearShown(), monthShown());
}
