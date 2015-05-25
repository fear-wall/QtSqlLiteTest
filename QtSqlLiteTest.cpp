#include "QtSqlLiteTest.h"
#include <QDebug>
#include <QSqlError>
#include <QSqlQuery>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QTextCodec>
#include <QSqlTableModel>
#include <QDateTime>

QtSqlLiteTest::QtSqlLiteTest(QWidget *parent)
    : QWidget(parent)
{
    // （创建）打开数据库
    if(createDB(m_dbPath, m_dbName))
    {
        const QString createTablaSql = QString("create table if not exists %1 \
                (%2 INTEGER PRIMARY KEY autoincrement, %3 varchar, %4 varchar, %5 vachar)")
                    .arg(m_dbTable).arg(m_headTableList.at(0)).arg(m_headTableList.at(1))
                    .arg(m_headTableList.at(2)).arg(m_headTableList.at(3));
        sqlQuery(createTablaSql);
    }

    initWidget();
    initModel();

    // 槽连接
    connect(m_addDataBtn, &QPushButton::clicked, this, &QtSqlLiteTest::addData);
}

/**
* @brief 初始化界面
* @author LuChenQun
* @date 2015-5-25
* @return 无
*/
void QtSqlLiteTest::initWidget()
{
    m_addDataBtn = new QPushButton("addData", this);
    m_songNameLe = new QLineEdit("song name", this);
    m_singerLe = new QLineEdit("singer", this);

    QVBoxLayout *mainLayOut = new QVBoxLayout(this);

    QHBoxLayout *layWidget = new QHBoxLayout();
    layWidget->addWidget(m_songNameLe);
    layWidget->addWidget(m_singerLe);
    layWidget->addWidget(m_addDataBtn);
    layWidget->addWidget(m_addDataBtn);

    m_tableView = new QTableView(this);

    mainLayOut->addLayout(layWidget);
    mainLayOut->addWidget(m_tableView);
    this->setLayout(mainLayOut);
}

/**
* @brief 初始化，并设置模板
* @author LuChenQun
* @date 2015-5-25
* @return 无
*/
void QtSqlLiteTest::initModel()
{
    m_model = new QSqlTableModel(this);
    m_model->setTable(m_dbTable);
    m_model->setEditStrategy(QSqlTableModel::OnManualSubmit);
    m_model->select();
    m_tableView->setModel(m_model);
    m_tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_tableView->setContextMenuPolicy(Qt::CustomContextMenu);
    m_tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    m_tableView->hideColumn(0);
}

/**
* @brief 增加数据
* @author LuChenQun
* @date 2015-5-25 17:57:32
* @return 无
*/
void QtSqlLiteTest::addData()
{
    QString songName = m_songNameLe->text();
    QString singer = m_singerLe->text();
    QString date = QDateTime::currentDateTime().toString("yyyy/M/d hh:mm:ss");
//    QString addDataSql = "INSERT INTO logevent VALUES(NULL, '" + event + "', '" + msg + "', " + volume + ")";
//    sqlQuery(addDataSql);

    int rowNum = m_model->rowCount();//获得表的行数
    m_model->insertRow(rowNum); //添加一行
    m_model->setData(m_model->index(rowNum, 1), songName);
    m_model->setData(m_model->index(rowNum, 2), singer);
    m_model->setData(m_model->index(rowNum, 3), date);

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


/**
* @brief 创建数据库
* @author LuChenQun
* @date 2015-5-25 17:29:37
* @param[in] dbFilePath 数据库所在路径
* @param[in] dbFileName 数据库名字
* @return bool true 创建成功 false 创建失败
*/
bool QtSqlLiteTest::createDB(const QString dbFilePath, const QString dbFileName)
{
    if (!QFile::exists(dbFilePath + "/" + dbFileName))
    {
        QDir dir;
        if (!dir.exists(dbFilePath))
        {
            dir.mkpath(dbFilePath);
        }
    }

    m_db = QSqlDatabase::addDatabase("QSQLITE");
    m_db.setDatabaseName(dbFilePath + "/" + dbFileName);

    bool openRet = m_db.open();
    qDebug() << ((openRet) ? (dbFileName + " open sucees") : (dbFileName + " open fail, " + m_db.lastError().driverText()));

    return openRet;
}

/**
* @brief 执行sql语句
* @author LuChenQun
* @date 2015-5-25 17:29:37
* @param[in] sql sql语句
* @return bool true 执行成功 false 执行失败
*/
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
