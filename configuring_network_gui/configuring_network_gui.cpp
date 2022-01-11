#include "configuring_network_gui.h"
#include <QDebug>
#include "../sdk/configuring_network.h"
#include "QProgressIndicator.h" 
#include <QMessageBox>

configuring_network_gui::configuring_network_gui(QWidget *parent)
    : QDialog(parent)
{
    ui.setupUi(this);
 
    ui.tableWidget_network_message->setColumnCount(2);
    QStringList header;
    header << "MAC" << "IP";
    ui.tableWidget_network_message->setHorizontalHeaderLabels(header);
    ui.tableWidget_network_message->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    ui.tableWidget_network_message->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    ui.tableWidget_network_message->setFrameShape(QFrame::Box);
    ui.tableWidget_network_message->horizontalHeader()->setLineWidth(1);
    //ui.tableWidget_network_message->setEditTriggers(QAbstractItemView::NoEditTriggers); //设置不可编辑
    ui.tableWidget_network_message->setSelectionBehavior(QAbstractItemView::SelectRows); //整行选中的方式
    ui.tableWidget_network_message->setSelectionMode(QAbstractItemView::SingleSelection); //设置为可以选中多个目标
 

    initializeFunction();



}

bool configuring_network_gui::initializeFunction()
{

    connect(ui.pushButton_find, SIGNAL(clicked()), this, SLOT(do_pushButton_find()));

    connect(ui.pushButton_auto, SIGNAL(clicked()), this, SLOT(do_pushButton_auto())); 

    connect(ui.tableWidget_network_message, SIGNAL(itemClicked(QTableWidgetItem* )), this, SLOT(do_tableWidget_clicked(QTableWidgetItem*)));

    connect(ui.tableWidget_network_message, SIGNAL(itemChanged(QTableWidgetItem*)), this, SLOT(do_tableWidget_DoubleClicked(QTableWidgetItem*)));

     



    return true;
}


bool configuring_network_gui::setCameraIp(QString mac, QString ip)
{
    if (ui.tableWidget_network_message->rowCount() == 0)
        return false;


    disconnect(ui.tableWidget_network_message, SIGNAL(itemChanged(QTableWidgetItem*)), this, SLOT(do_tableWidget_DoubleClicked(QTableWidgetItem*)));

    for (int i = 0; i < ui.tableWidget_network_message->rowCount(); i++)
    {
        if (mac == ui.tableWidget_network_message->item(i, 0)->text())
        {
            ui.tableWidget_network_message->item(i, 1)->setText(ip);
 
        }
    }


    connect(ui.tableWidget_network_message, SIGNAL(itemChanged(QTableWidgetItem*)), this, SLOT(do_tableWidget_DoubleClicked(QTableWidgetItem*)));

    return true;
}
 

void configuring_network_gui::do_tableWidget_DoubleClicked(QTableWidgetItem* item)
{
     
    
    std::string mac = ui.tableWidget_network_message->item(item->row(), 0)->text().toStdString();
    std::string ip = ui.tableWidget_network_message->item(item->row(), 1)->text().toStdString();

 

 

    int ret = SetCameraIp(mac, ip);

    setCameraIp(QString::fromStdString(mac), QString::fromStdString(ip));

    std::cout << "set ip: " << ip<<std::endl;

    if (!ret)
    {
        QMessageBox::information(this, QString::fromLocal8Bit("提示"), QString::fromLocal8Bit("设置IP成功"),
            QMessageBox::Yes, QMessageBox::Yes);
    }
    else
    {
        QMessageBox::information(this, QString::fromLocal8Bit("提示"), QString::fromLocal8Bit("设置IP出错"),
            QMessageBox::Yes, QMessageBox::Yes);
    }




}


void configuring_network_gui::do_tableWidget_clicked(QTableWidgetItem* item)
{ 

    select_mac_ = ui.tableWidget_network_message->item(item->row(), 0)->text();
    select_ip_ = ui.tableWidget_network_message->item(item->row(), 1)->text();
     

}

void configuring_network_gui::do_pushButton_auto()
{  
    qDebug() << "select_mac_: " << select_mac_; 
    std::string ip; 
    int ret = SetCameraIpAuto(select_mac_.toStdString(), ip); 
    std::cout << "auto set ip: " << ip;
    setCameraIp(select_mac_, QString::fromStdString(ip));

    if (!ret)
    {
        QMessageBox::information(this, QString::fromLocal8Bit("提示"), QString::fromLocal8Bit("设置IP成功"),
            QMessageBox::Yes, QMessageBox::Yes);
    }
    else
    {
        QMessageBox::information(this, QString::fromLocal8Bit("提示"), QString::fromLocal8Bit("设置IP出错"),
            QMessageBox::Yes, QMessageBox::Yes);
    }

}

void configuring_network_gui::do_pushButton_find()
{
 
    std::vector<std::string> mac_list;
    std::vector<std::string> ip_list;

     

    int ret = GetCameraList( mac_list,  ip_list);



    ui.tableWidget_network_message->setRowCount(mac_list.size());


    disconnect(ui.tableWidget_network_message, SIGNAL(itemChanged(QTableWidgetItem*)), this, SLOT(do_tableWidget_DoubleClicked(QTableWidgetItem*)));

    for (int i = 0; i < mac_list.size(); i++)
    {
        std::cout << mac_list[i] << std::endl;
        std::cout << ip_list[i] << std::endl;

        ui.tableWidget_network_message->setItem(i, 0, new QTableWidgetItem(QString::fromStdString(mac_list[i]))); 
        ui.tableWidget_network_message->item(i, 0)->setTextAlignment(Qt::AlignHCenter | Qt::AlignCenter); 
        QTableWidgetItem* item_mac = new QTableWidgetItem(QString::fromStdString(mac_list[i]));
        item_mac->setFlags(item_mac->flags() & (~Qt::ItemIsEditable));
        item_mac->setTextAlignment(Qt::AlignHCenter | Qt::AlignCenter);
        ui.tableWidget_network_message->setItem(i, 0, item_mac);



        QTableWidgetItem* item_ip = new QTableWidgetItem(QString::fromStdString(ip_list[i]));
        item_ip->setTextAlignment(Qt::AlignHCenter | Qt::AlignCenter);
        ui.tableWidget_network_message->setItem(i, 1, item_ip);
 


    }

    ui.tableWidget_network_message->sortItems(0); 
    connect(ui.tableWidget_network_message, SIGNAL(itemChanged(QTableWidgetItem*)), this, SLOT(do_tableWidget_DoubleClicked(QTableWidgetItem*)));

}
