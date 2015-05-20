#include "QtSqlLiteTest.h"
#include <QDebug>
#include <QSqlError>
#include <QSqlQuery>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QTextCodec>
#include <QSqlTableModel>

QtSqlLiteTest::QtSqlLiteTest(QWidget *parent)
    : QWidget(parent)
{
    // 打开数据库
    if(createDB("/debug", "myDB.db"))
    {
        const QString createTablaSql = "create table if not exists logevent (id INTEGER PRIMARY KEY autoincrement, event varchar, msg varchar, volume int)";
        sqlQuery(createTablaSql);
    }

    // 初始化界面
    m_addDataBtn = new QPushButton("addData", this);
    m_eventLE = new QLineEdit("测试中文", this);
    m_msgLE = new QLineEdit("msg", this);
    m_volumeLE = new QLineEdit("100", this);

    QVBoxLayout *mainLayOut = new QVBoxLayout(this);

    QHBoxLayout *layout1 = new QHBoxLayout();
    layout1->addWidget(m_eventLE);
    layout1->addWidget(m_msgLE);
    layout1->addWidget(m_volumeLE);
    layout1->addWidget(m_addDataBtn);

    m_tableView = new QTableView(this);

    mainLayOut->addLayout(layout1);
    mainLayOut->addWidget(m_tableView);

    m_model = new QSqlTableModel(this);
    m_model->setTable("logevent");
    m_model->setEditStrategy(QSqlTableModel::OnManualSubmit);
    m_model->select();
    m_tableView->setModel(m_model);
    m_tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);

    this->setLayout(mainLayOut);

    // 槽连接
    connect(m_addDataBtn, &QPushButton::clicked, this, &QtSqlLiteTest::addData);
}

void QtSqlLiteTest::addData()
{
    QString event = m_eventLE->text();
    QString msg = m_msgLE->text();
    QString volume = m_volumeLE->text();
//    QString addDataSql = "INSERT INTO logevent VALUES(NULL, '" + event + "', '" + msg + "', " + volume + ")";
//    sqlQuery(addDataSql);

    int rowNum = m_model->rowCount();//获得表的行数
    m_model->insertRow(rowNum); //添加一行
    m_model->setData(m_model->index(rowNum, 1), event);
    m_model->setData(m_model->index(rowNum, 2), msg);
    m_model->setData(m_model->index(rowNum, 3), volume);

    m_model->database().transaction(); //开始事务操作
    if (m_model->submitAll()) // 提交所有被修改的数据到数据库中
    {
        m_model->database().commit(); //提交成功，事务将真正修改数据库数据
    } else {
        m_model->database().rollback(); //提交失败，事务回滚
    }
}

QtSqlLiteTest::~QtSqlLiteTest()
{
    m_db.close();
}

bool QtSqlLiteTest::createDB(const QString dbFilePath, const QString dbFileName)
{
    m_db = QSqlDatabase::addDatabase("QSQLITE");
    m_db.setDatabaseName(dbFileName);

    bool openRet = m_db.open();
    qDebug() << ((openRet) ? (dbFileName + " open sucees") : (dbFileName + " open fail, " + m_db.lastError().driverText()));

    return openRet;
}

bool QtSqlLiteTest::sqlQuery(const QString sql)
{
    QSqlQuery query;
    bool queryRet = query.exec(sql);
    if(!queryRet)
    {
        qDebug() << (sql + " query fail");
    }
    return queryRet;
}
