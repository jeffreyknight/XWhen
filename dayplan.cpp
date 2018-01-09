#include "dayplan.h"
#include "ui_dayplan.h"

DayPlan::DayPlan(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DayPlan)
{
    ui->setupUi(this);
}

DayPlan::~DayPlan()
{
    delete ui;
}

void DayPlan::recvDumpToDay(QList<QPair<quint64,QString>> dump, QDate d){
    clearBuffers();
    day = d;
    ents = dump;
    updateItinerary();
    show();
}

void DayPlan::clearBuffers(){
    ents = QList<QPair <quint64, QString> >();
    day = QDate(0,0,0);
    currentIndex = 0;
}

void DayPlan::closeEvent(QCloseEvent *event){
    clearBuffers();
    event->accept();
}

void DayPlan::updateItinerary(){
    ui->spinBoxCurrent->setValue(currentIndex);
    if((quint32)ents.length() > 0 + currentIndex){
        QDateTime qdt = QDateTime::fromTime_t(ents.at(0 + currentIndex).first);
        ui->timeEdit1->setTime(qdt.time());
        ui->plainTextEdit1->setPlainText(ents.at(0 + currentIndex).second);
    }else{
        ui->timeEdit1->setTime(QTime(0, 0, 0));
        ui->plainTextEdit1->setPlainText("");
    }
    if((quint32)ents.length() > 1 + currentIndex){
        QDateTime qdt = QDateTime::fromTime_t(ents.at(1 + currentIndex).first);
        ui->timeEdit2->setTime(qdt.time());
        ui->plainTextEdit2->setPlainText(ents.at(1 + currentIndex).second);
    }else{
        ui->timeEdit2->setTime(QTime(0, 0, 0));
        ui->plainTextEdit2->setPlainText("");
    }
    if((quint32)ents.length() > 2 + currentIndex){
        QDateTime qdt = QDateTime::fromTime_t(ents.at(2 + currentIndex).first);
        ui->timeEdit3->setTime(qdt.time());
        ui->plainTextEdit3->setPlainText(ents.at(2 + currentIndex).second);
    }else{
        ui->timeEdit3->setTime(QTime(0, 0, 0));
        ui->plainTextEdit3->setPlainText("");
    }
    if((quint32)ents.length() > 3 + currentIndex){
        QDateTime qdt = QDateTime::fromTime_t(ents.at(3 + currentIndex).first);
        ui->timeEdit4->setTime(qdt.time());
        ui->plainTextEdit4->setPlainText(ents.at(3 + currentIndex).second);
    }else{
        ui->timeEdit4->setTime(QTime(0, 0, 0));
        ui->plainTextEdit4->setPlainText("");
    }
    if((quint32)ents.length() > 4 + currentIndex){
        QDateTime qdt = QDateTime::fromTime_t(ents.at(4 + currentIndex).first);
        ui->timeEdit5->setTime(qdt.time());
        ui->plainTextEdit5->setPlainText(ents.at(4 + currentIndex).second);
    }else{
        ui->timeEdit5->setTime(QTime(0, 0, 0));
        ui->plainTextEdit5->setPlainText("");
    }
}

void DayPlan::on_applyButton_clicked(){
    sendDaysWorthBackToDb(QDateTime(day).toTime_t(), ents);
}

void DayPlan::on_okButton_clicked(){
    on_applyButton_clicked();
    clearBuffers();
    hide();
}

void DayPlan::on_cancelButton_clicked(){
    clearBuffers();
    hide();
}

void DayPlan::on_upButton_clicked(){
    if(currentIndex > 0){
        currentIndex--;
        updateItinerary();
    }
}

void DayPlan::on_downButton_clicked(){
    if(currentIndex < std::numeric_limits<quint32>::max()){
        currentIndex++;
        updateItinerary();
    }
}

void DayPlan::on_delButton1_clicked(){
    if((quint32)ents.length() > 0 + currentIndex){
        ents.removeAt(0 + currentIndex);
        updateItinerary();
    }
}

void DayPlan::on_delButton2_clicked(){
    if((quint32)ents.length() > 1 + currentIndex){
        ents.removeAt(1 + currentIndex);
        updateItinerary();
    }
}

void DayPlan::on_delButton3_clicked(){
    if((quint32)ents.length() > 2 + currentIndex){
        ents.removeAt(2 + currentIndex);
        updateItinerary();
    }
}

void DayPlan::on_delButton4_clicked(){
    if((quint32)ents.length() > 3 + currentIndex){
        ents.removeAt(3 + currentIndex);
        updateItinerary();
    }
}

void DayPlan::on_delButton5_clicked(){
    if((quint32)ents.length() > 4 + currentIndex){
        ents.removeAt(4 + currentIndex);
        updateItinerary();
    }
}

void DayPlan::on_plainTextEdit1_textChanged(){
    if(!ui->plainTextEdit1->toPlainText().isEmpty()){
        while((quint32)ents.length() <= 0 + currentIndex){
            QPair<quint64, QString> empty;
            empty.first = QDateTime(day, QTime(0,0,0)).toTime_t();
            empty.second = QString();
            ents.append(empty);
        }
        QPair<quint64, QString> s = ents.at(0 + currentIndex);
        s.second = ui->plainTextEdit1->toPlainText();
        ents.replace(0 + currentIndex, s);
    }
}

void DayPlan::on_plainTextEdit2_textChanged(){
    if(!ui->plainTextEdit2->toPlainText().isEmpty()){
        while((quint32)ents.length() <= 1 + currentIndex){
            QPair<quint64, QString> empty;
            empty.first = QDateTime(day, QTime(0,0,0)).toTime_t();
            empty.second = QString();
            ents.append(empty);
        }
        QPair<quint64, QString> s = ents.at(1 + currentIndex);
        s.second = ui->plainTextEdit2->toPlainText();
        ents.replace(1 + currentIndex, s);
    }
}

void DayPlan::on_plainTextEdit3_textChanged(){
    if(!ui->plainTextEdit3->toPlainText().isEmpty()){
        while((quint32)ents.length() <= 2 + currentIndex){
            QPair<quint64, QString> empty;
            empty.first = QDateTime(day, QTime(0,0,0)).toTime_t();
            empty.second = QString();
            ents.append(empty);
        }
        QPair<quint64, QString> s = ents.at(2 + currentIndex);
        s.second = ui->plainTextEdit3->toPlainText();
        ents.replace(2 + currentIndex, s);
    }
}

void DayPlan::on_plainTextEdit4_textChanged(){
    if(!ui->plainTextEdit4->toPlainText().isEmpty()){
        while((quint32)ents.length() <= 3 + currentIndex){
            QPair<quint64, QString> empty;
            empty.first = QDateTime(day, QTime(0,0,0)).toTime_t();
            empty.second = QString();
            ents.append(empty);
        }
        QPair<quint64, QString> s = ents.at(3 + currentIndex);
        s.second = ui->plainTextEdit4->toPlainText();
        ents.replace(3 + currentIndex, s);
    }
}

void DayPlan::on_plainTextEdit5_textChanged(){
    if(!ui->plainTextEdit5->toPlainText().isEmpty()){
        while((quint32)ents.length() <= 4 + currentIndex){
            QPair<quint64, QString> empty;
            empty.first = QDateTime(day, QTime(0,0,0)).toTime_t();
            empty.second = QString();
            ents.append(empty);
        }
        QPair<quint64, QString> s = ents.at(4 + currentIndex);
        s.second = ui->plainTextEdit5->toPlainText();
        ents.replace(4 + currentIndex, s);
    }
}

void DayPlan::on_timeEdit1_timeChanged(const QTime &time){
    if(time.msecsSinceStartOfDay() != 0){
        while((quint32)ents.length() <= 0 + currentIndex){
            QPair<quint64, QString> empty;
            empty.first = QDateTime(day, QTime(0,0,0)).toTime_t();
            empty.second = QString();
            ents.append(empty);
        }
        QPair<quint64, QString> s = ents.at(0 + currentIndex);
        s.first = QDateTime(day, time).toTime_t();
        ents.replace(0 + currentIndex, s);
    }
}

void DayPlan::on_timeEdit2_timeChanged(const QTime &time){
    if(time.msecsSinceStartOfDay() != 0){
        while((quint32)ents.length() <= 1 + currentIndex){
            QPair<quint64, QString> empty;
            empty.first = QDateTime(day, QTime(0,0,0)).toTime_t();
            empty.second = QString();
            ents.append(empty);
        }
        QPair<quint64, QString> s = ents.at(1 + currentIndex);
        s.first = QDateTime(day, time).toTime_t();
        ents.replace(1 + currentIndex, s);
    }
}

void DayPlan::on_timeEdit3_timeChanged(const QTime &time){
    if(time.msecsSinceStartOfDay() != 0){
        while((quint32)ents.length() <= 2 + currentIndex){
            QPair<quint64, QString> empty;
            empty.first = QDateTime(day, QTime(0,0,0)).toTime_t();
            empty.second = QString();
            ents.append(empty);
        }
        QPair<quint64, QString> s = ents.at(2 + currentIndex);
        s.first = QDateTime(day, time).toTime_t();
        ents.replace(2 + currentIndex, s);
    }
}

void DayPlan::on_timeEdit4_timeChanged(const QTime &time){
    if(time.msecsSinceStartOfDay() != 0){
        while((quint32)ents.length() <= 3 + currentIndex){
            QPair<quint64, QString> empty;
            empty.first = QDateTime(day, QTime(0,0,0)).toTime_t();
            empty.second = QString();
            ents.append(empty);
        }
        QPair<quint64, QString> s = ents.at(3 + currentIndex);
        s.first = QDateTime(day, time).toTime_t();
        ents.replace(3 + currentIndex, s);
    }
}

void DayPlan::on_timeEdit5_timeChanged(const QTime &time){
    if(time.msecsSinceStartOfDay() != 0){
        while((quint32)ents.length() <= 4 + currentIndex){
            QPair<quint64, QString> empty;
            empty.first = QDateTime(day, QTime(0,0,0)).toTime_t();
            empty.second = QString();
            ents.append(empty);
        }
        QPair<quint64, QString> s = ents.at(4 + currentIndex);
        s.first = QDateTime(day, time).toTime_t();
        ents.replace(4 + currentIndex, s);
    }
}
