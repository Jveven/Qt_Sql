#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QPixmap>


void MainWindow::openTable()
{
    tabModel = new QSqlTableModel(this,DB);
    tabModel->setTable("employee"); //设置数据表名称
    tabModel->setSort(tabModel->fieldIndex("empNo"),Qt::AscendingOrder);
    tabModel->setEditStrategy(QSqlTableModel::OnManualSubmit);
    if(!tabModel->select())
    {
        QMessageBox::critical(this,"错误","打开数据表错误，错误信息\n"+tabModel->lastError().text());
        return;
    }

    //修改表头
    tabModel->setHeaderData(tabModel->fieldIndex("empNo"),Qt::Horizontal,"工号");
    tabModel->setHeaderData(tabModel->fieldIndex("Name"),Qt::Horizontal,"姓名");
    tabModel->setHeaderData(tabModel->fieldIndex("Gender"),Qt::Horizontal,"性别");
    tabModel->setHeaderData(tabModel->fieldIndex("Height"),Qt::Horizontal,"身高");
    tabModel->setHeaderData(tabModel->fieldIndex("Birthday"),Qt::Horizontal,"出生日期");
    tabModel->setHeaderData(tabModel->fieldIndex("Mobile"),Qt::Horizontal,"手机号");
    tabModel->setHeaderData(tabModel->fieldIndex("Province"),Qt::Horizontal,"省份");
    tabModel->setHeaderData(tabModel->fieldIndex("City"),Qt::Horizontal,"城市");
    tabModel->setHeaderData(tabModel->fieldIndex("Department"),Qt::Horizontal,"部门");
    tabModel->setHeaderData(tabModel->fieldIndex("Education"),Qt::Horizontal,"学历");
    tabModel->setHeaderData(tabModel->fieldIndex("Salary"),Qt::Horizontal,"薪资");
    tabModel->setHeaderData(tabModel->fieldIndex("Photo"),Qt::Horizontal,"照片");
    tabModel->setHeaderData(tabModel->fieldIndex("Memo"),Qt::Horizontal,"备注");

    theSelection = new QItemSelectionModel(tabModel);
    ui->tableView->setModel(tabModel);
    ui->tableView->setSelectionModel(theSelection);

    connect(theSelection,SIGNAL(currentChanged(QModelIndex,QModelIndex)),
            this,SLOT(on_currentChanged(QModelIndex,QModelIndex)));
    connect(theSelection,SIGNAL(currentChanged(QModelIndex,QModelIndex)),
            this,SLOT(on_currentRowChanged(QModelIndex,QModelIndex)));

    //隐藏列
    ui->tableView->setColumnHidden(tabModel->fieldIndex("Photo"),true);
    ui->tableView->setColumnHidden(tabModel->fieldIndex("Memo"),true);

    dataMapper = new QDataWidgetMapper;
    dataMapper->setModel(tabModel);
    dataMapper->setSubmitPolicy(QDataWidgetMapper::AutoSubmit);

    dataMapper->addMapping(ui->dbSpinEmpNo,tabModel->fieldIndex("empNo"));
    dataMapper->addMapping(ui->dbEditName,tabModel->fieldIndex("Name"));
    dataMapper->addMapping(ui->dbComboSex,tabModel->fieldIndex("Gender"));
    dataMapper->addMapping(ui->dbSpinHeight,tabModel->fieldIndex("Height"));
    dataMapper->addMapping(ui->dbEditBirth,tabModel->fieldIndex("Birthday"));
    dataMapper->addMapping(ui->dbEditMobile,tabModel->fieldIndex("Mobile"));
    dataMapper->addMapping(ui->dbComboProvince,tabModel->fieldIndex("Province"));
    dataMapper->addMapping(ui->dbEditCity,tabModel->fieldIndex("City"));
    dataMapper->addMapping(ui->dbComboDep,tabModel->fieldIndex("Department"));
    dataMapper->addMapping(ui->dbComboEdu,tabModel->fieldIndex("Education"));
    dataMapper->addMapping(ui->dbSpinSalary,tabModel->fieldIndex("Salary"));
    dataMapper->addMapping(ui->dbEditMemo,tabModel->fieldIndex("Memo"));

    getFiledNames();

    ui->actOpenDB->setEnabled(false);
    ui->actRecAppend->setEnabled(true);
    ui->actRecDelete->setEnabled(true);
    ui->actRecInsert->setEnabled(true);
    ui->actScan->setEnabled(true);
    ui->groupBoxSort->setEnabled(true);
    ui->groupBoxFilter->setEnabled(true);
    ui->actPhoto->setEnabled(true);
    ui->actPhotoClear->setEnabled(true);

    QStringList strList;
    strList<<"男"<<"女";
    bool isEditable = false;
    delegateSex.setItems(strList,isEditable);
    ui->tableView->setItemDelegateForColumn(tabModel->fieldIndex("Gender"),&delegateSex);

    strList.clear();
    strList<<"销售部"<<"技术部"<<"生产部"<<"行政部";
    delegateDepart.setItems(strList,isEditable);
    ui->tableView->setItemDelegateForColumn(tabModel->fieldIndex("Department"),&delegateDepart);

    strList.clear();
    strList<<"博士研究生"<<"硕士研究生"<<"大学本科"<<"大学专科"<<"高中"<<"初中"<<"小学";
    delegateEdu.setItems(strList,isEditable);
    ui->tableView->setItemDelegateForColumn(tabModel->fieldIndex("Education"),&delegateEdu);
    //省份 城市


}

void MainWindow::getFiledNames()
{
    // QSqlRecord emptyRec = tabModel->record(); //空记录
    // for(int i=0;i<emptyRec.count();i++)
    // {
    //     ui->comboFields->addItem(emptyRec.fieldName(i));
    // }
    ui->comboFields->addItem("工号");
    ui->comboFields->addItem("姓名");
    ui->comboFields->addItem("性别");
    ui->comboFields->addItem("身高");
    ui->comboFields->addItem("出身日期");
    ui->comboFields->addItem("手机号");
    ui->comboFields->addItem("省份");
    ui->comboFields->addItem("城市");
    ui->comboFields->addItem("部门");
    ui->comboFields->addItem("学历");
    ui->comboFields->addItem("工资");
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    this->setCentralWidget(ui->splitter);

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_currentChanged(const QModelIndex &current, const QModelIndex &preivous)
{
    Q_UNUSED(current);
    Q_UNUSED(preivous);
    ui->actSubmit->setEnabled(tabModel->isDirty());
    ui->actRevert->setEnabled(tabModel->isDirty());
}

void MainWindow::on_currentRowChanged(const QModelIndex &current, const QModelIndex &preivous)
{
    ui->actRecDelete->setEnabled(current.isValid());
    ui->actRecAppend->setEnabled(current.isValid());
    ui->actRecInsert->setEnabled(current.isValid());

    if(!current.isValid())
    {
        ui->dbLabPhoto->clear();
        return;
    }
    dataMapper->setCurrentIndex(current.row());
    QSqlRecord curRec = tabModel->record(current.row());
    if(curRec.isNull("Photo"))
    {
        ui->dbLabPhoto->clear();
    }
    else
    {
        QByteArray data = curRec.value("Photo").toByteArray();
        QPixmap pic;
        pic.loadFromData(data);
        ui->dbLabPhoto->setPixmap(pic.scaledToWidth(ui->dbLabPhoto->size().width()));
    }
}


void MainWindow::on_actOpenDB_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this,"选择数据库文件","","Sqlite(*.db *.db3)");
    if(fileName.isEmpty())
    {
        return;
    }
    DB = QSqlDatabase::addDatabase("QSQLITE");
    DB.setDatabaseName(fileName);
    if(!DB.open())
    {
        QMessageBox::warning(this,"错误","打开数据库失败");
        return;
    }
    openTable();
}


void MainWindow::on_actRecAppend_triggered()
{
    tabModel->insertRow(tabModel->rowCount(),QModelIndex());
    QModelIndex curIndex = tabModel->index(tabModel->rowCount()-1,1);
    theSelection->clearSelection();
    theSelection->setCurrentIndex(curIndex,QItemSelectionModel::Select);
    tabModel->setData(tabModel->index(curIndex.row(),0),2000+tabModel->rowCount());
    tabModel->setData(tabModel->index(curIndex.row(),2),"男");
    ui->actSubmit->setEnabled(true);
    ui->actRevert->setEnabled(true);
}


void MainWindow::on_actRecInsert_triggered()
{
    QModelIndex curIndex = theSelection->currentIndex();
    tabModel->insertRow(curIndex.row(),QModelIndex());
    theSelection->clearSelection();
    theSelection->setCurrentIndex(curIndex,QItemSelectionModel::Select);
    tabModel->setData(tabModel->index(curIndex.row(),0),2000+tabModel->rowCount());
    tabModel->setData(tabModel->index(curIndex.row(),2),"男");
    ui->actSubmit->setEnabled(true);
    ui->actRevert->setEnabled(true);
}


void MainWindow::on_actRecDelete_triggered()
{
    QModelIndex curIndex = theSelection->currentIndex();
    tabModel->removeRow(curIndex.row());
    ui->actSubmit->setEnabled(true);
    ui->actRevert->setEnabled(true);
}


void MainWindow::on_actSubmit_triggered()
{
    bool result = tabModel->submitAll();
    if(!result)
    {
        QMessageBox::information(this,"信息","数据提交错误，错误信息\n"
                                +tabModel->lastError().text());
    }
    else
    {
        ui->actSubmit->setEnabled(false);
        ui->actRevert->setEnabled(false);
    }
}


void MainWindow::on_actRevert_triggered()
{
    tabModel->revertAll();
    ui->actSubmit->setEnabled(false);
    ui->actRevert->setEnabled(false);
}


void MainWindow::on_actPhoto_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this,"选择照片","","照片(*.jpg *.png)");
    if(fileName.isEmpty())
    {
        return;
    }
    QByteArray data;
    QFile *file = new QFile(fileName);
    if(file->open(QIODevice::ReadOnly))
    {
        data = file->readAll();

        QModelIndex curIndex = theSelection->currentIndex();
        QSqlRecord curRec = tabModel->record(curIndex.row());
        curRec.setValue("photo",data);
        tabModel->setRecord(curIndex.row(),curRec);

        QPixmap pic;
        pic.load(fileName);
        ui->dbLabPhoto->setPixmap(pic.scaledToWidth(ui->dbLabPhoto->width()));

        file->close();
    }
    delete file;
}


void MainWindow::on_actPhotoClear_triggered()
{
    QModelIndex curIndex = theSelection->currentIndex();
    QSqlRecord curRec = tabModel->record(curIndex.row());
    curRec.setNull("photo");
    tabModel->setRecord(curIndex.row(),curRec);
    ui->dbLabPhoto->clear();
}


void MainWindow::on_actScan_triggered()
{
    if(tabModel->rowCount() != 0)
    {
        for(int i = 0;i < tabModel->rowCount();i++)
        {
            QSqlRecord aRec = tabModel->record(i);
            float salary = aRec.value("Salary").toFloat();
            salary = salary * 1.1;
            aRec.setValue("Salary",salary);
            tabModel->setRecord(i,aRec);
        }
        if(tabModel->submitAll())
        {
            QMessageBox::information(this,"信息","涨工资完成");
        }
    }
}


void MainWindow::on_comboFields_currentIndexChanged(int index)
{
    if(ui->radioBtnAscend->isCheckable())
    {
        tabModel->setSort(index,Qt::AscendingOrder);
    }
    else
    {
        tabModel->setSort(index,Qt::DescendingOrder);
    }
    tabModel->select();
}


void MainWindow::on_radioBtnAscend_clicked()
{
    tabModel->setSort(ui->comboFields->currentIndex(),Qt::AscendingOrder);
    tabModel->select();
}




void MainWindow::on_radioBtnDescend_clicked()
{
    tabModel->setSort(ui->comboFields->currentIndex(),Qt::DescendingOrder);
    tabModel->select();
}


void MainWindow::on_radioBtnMan_clicked()
{
    tabModel->setFilter("Gender='男'");
}


void MainWindow::on_radioBtnWoman_clicked()
{
    tabModel->setFilter("Gender='女'");
}


void MainWindow::on_radioBtnBoth_clicked()
{
    tabModel->setFilter("");
}

