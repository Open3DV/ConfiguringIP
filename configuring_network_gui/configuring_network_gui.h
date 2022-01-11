#pragma once

#include <QtWidgets/QDialog>
#include "ui_configuring_network_gui.h"

class configuring_network_gui : public QDialog
{
    Q_OBJECT

public:
    configuring_network_gui(QWidget *parent = Q_NULLPTR);


    bool initializeFunction();

    bool setCameraIp(QString mac, QString ip);

private slots: 
    void do_pushButton_find();

    void do_pushButton_auto(); 

    void do_tableWidget_clicked(QTableWidgetItem* item); 

    void do_tableWidget_DoubleClicked(QTableWidgetItem* item);
      

private:
    Ui::configuring_network_guiClass ui;

    QString select_mac_;
    QString select_ip_;

};
