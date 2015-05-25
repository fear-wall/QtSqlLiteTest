#include "QtSqlLiteTest.h"
#include <QDebug>
#include <QSqlError>
#include <QSqlQuery>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QTextCodec>
#include <QSqlTableModel>
#include <QDateTime>
#include <QHeaderView>
#include <QMenu>

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
    connect(m_tableView, &QTableView::customContextMenuRequested, this, &QtSqlLiteTest::showContextmenu);
}

/**
* @brief 初始化界面
* @author LuChenQun
* @date 2015-5-25
* @return 无
*/
void QtSqlLiteTest::initWidget()
{
    m_addDataBtn = new QPushButton("插入", this);
    m_songNameLe = new QLineEdit("input song name", this);
    m_singerLe = new QLineEdit("input singer", this);

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
    m_tableView->verticalHeader()->hide();  // 左边自动生成的列序号不显示

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
#if 0
    // 如果仅仅插入到数据库，使用此方式
    QString addDataSql = QString("INSERT INTO %1 VALUES(NULL, '%2', '%3', '%4')")
                .arg(m_dbTable).arg(songName).arg(singer).arg(date);
    sqlQuery(addDataSql);
#else
    // 使用MVC模式，插入数据会自动更新列表
    int rowNum = m_model->rowCount();   //获得表的行数
    int colunmIndex = 1;                // 由于隐藏了一列，所以索引从1开始
    m_model->insertRow(rowNum);         //添加一行
    m_model->setData(m_model->index(rowNum, colunmIndex++), songName);
    m_model->setData(m_model->index(rowNum, colunmIndex++), singer);
    m_model->setData(m_model->index(rowNum, colunmIndex++), date);

    m_model->database().transaction();  // 开始事务操作
    if (m_model->submitAll())           // 提交所有被修改的数据到数据库中
    {
        m_model->database().commit();   // 提交成功，事务将真正修改数据库数据
    }
    else
    {
        m_model->database().rollback(); // 提交失败，事务回滚
    }
#endif
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
* @brief 右键执行菜单操作
* @author LuChenQun
* @date 2015/05/22
* @param[in] point 右键的位置
* @return void
*/
void QtSqlLiteTest::showContextmenu(const QPoint& point)
{
    QMenu *menu = new QMenu(m_tableView);
    menu->addAction(("删除"), this, SLOT(delData()));
    menu->exec(QCursor::pos());
}

/**
* @brief 将选中表格的数据复制到搜索框里面，同时自动关联过滤选项
* @author LuChenQun
* @date 2015/05/22
* @return void
*/
void QtSqlLiteTest::delData()
{
    int row = m_tableView->currentIndex().row();
    m_model->removeRow(row);
    m_model->database().transaction();  //开始事务操作
    if (m_model->submitAll())           // 提交所有被修改的数据到数据库中
    {
        m_model->database().commit();   //提交成功，事务将真正修改数据库数据
    }
    else
    {
        m_model->database().rollback(); //提交失败，事务回滚
    }
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
