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
#include <QSqlRecord>
#include <QSqlIndex>

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
    connect(m_delDataBtn, &QPushButton::clicked, this, &QtSqlLiteTest::delData);
    connect(m_tableView, &QTableView::customContextMenuRequested, this, &QtSqlLiteTest::showContextmenu);
//    // 删除一行之前发出的信号
//    connect(m_model, &QSqlTableModel::beforeDelete, this, [](int row){
//        qDebug() << "delete data in database for row = " << row;
//    });
//    // 插入一行之前发出的信号
//    connect(m_model, &QSqlTableModel::beforeInsert, this, [](QSqlRecord &record){
//        qDebug() << "insert data in database for row = " << record;
//    });
    connect(m_tableView->selectionModel(), &QItemSelectionModel::currentChanged,
            this, [=](const QModelIndex &cur, const QModelIndex &prv){

        qDebug() << "cur:(" << cur.row() << "," << cur.column() << ")";
        qDebug() << "prv:(" << prv.row() << "," << prv.column() << ")";

        QModelIndex curIndex = m_tableView->model()->index(cur.row(), cur.column());
        QModelIndex prvIndex = m_tableView->model()->index(prv.row(), prv.column());
        qDebug() << "tableView currentColumnChanged change, prv = "
                 << prvIndex.data().toString() << ", cur = " << curIndex.data().toString();


        if(prv.row() != -1 && prv.column() != -1)
        {
            QVariant data =  prvIndex.data();
//            m_model->setData(m_model->index(prv.row(), prv.column()), m_tableView->model()->index(prv.row(), prv.column()));
            m_model->setData(m_model->index(prv.row(),prv.column()), data);
            qDebug() << "updateData ret = " << updateData();
        }
    });

    qDebug() << getRecordValue(3, m_headTableList.at(2)).toString();// 获取列名为 singer ，行号索引为3的数据库里面的数据
    qDebug() << m_model->fieldIndex(m_headTableList.at(3));         // 获取表名为 xxx 所在数据库表的索引
}

/**
* @brief 析构函数
* @author LuChenQun
* @date 2015-5-26 16:24:31
*/
QtSqlLiteTest::~QtSqlLiteTest()
{
    m_db.close();
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
    m_delDataBtn = new QPushButton("删除",this);
    m_songNameLe = new QLineEdit("input song name", this);
    m_singerLe = new QLineEdit("input singer", this);

    QVBoxLayout *mainLayOut = new QVBoxLayout(this);

    QHBoxLayout *layWidget = new QHBoxLayout();
    layWidget->addWidget(m_songNameLe);
    layWidget->addWidget(m_singerLe);
    layWidget->addWidget(m_addDataBtn);
    layWidget->addWidget(m_delDataBtn);

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
#if EDIT_ABLE_SQLMODEL
    m_model = new EditAbleSqlModel(this);
#else
    m_model = new QSqlTableModel(this);
#endif
    m_model->setTable(m_dbTable);
    m_model->setEditStrategy(QSqlTableModel::OnManualSubmit);
    m_model->select();

    // 没有数据，插入几条做测试
    int rowNum = m_model->rowCount();
    if(0 == rowNum)
    {
        for(int i=1; i<=5; i++)
        {
            sqlQuery(QString("INSERT INTO %1 VALUES(NULL, '歌名%2', '歌手%3', '%4')")
                    .arg(m_dbTable).arg(i).arg(i).arg("2015/5/26 15:07:12"));
        }
        m_model->select();
    }

    // 设置头说明
    int columnIndex = 0;
    foreach (QString head, m_headInfoList) {
        m_model->setHeaderData(columnIndex++, Qt::Horizontal, head);
    }

    m_tableView->setModel(m_model);
    m_tableView->setEditTriggers(QAbstractItemView::DoubleClicked);
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
    int rowNum = m_model->rowCount();   // 获得表的行数
    int colunmIndex = 1;                // 由于隐藏了一列，所以索引从1开始
    m_model->insertRow(rowNum);         // 添加一行
    m_model->setData(m_model->index(rowNum, colunmIndex++), songName);
    m_model->setData(m_model->index(rowNum, colunmIndex++), singer);
    m_model->setData(m_model->index(rowNum, colunmIndex++), date);

    qDebug() << "updateData ret = " << updateData();
#endif
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
    updateData();
}

/**
* @brief 修改数据
* @author LuChenQun
* @date 2015-5-26 16:27:02
* @param[in] parameter‐name parameter description
* @return description of the return value
*/
void QtSqlLiteTest::alterData()
{
    int row = m_tableView->currentIndex().row();
    int column = m_tableView->currentIndex().column();
    if (row >= 0 && column >= 0)
    {
        m_model->setData(m_model->index(row, column), m_model->data(m_model->index(row, column)));
        updateData();
    }
}

/**
* @brief 将修改的数据更新到数据库
* @author LuChenQun
* @date 2015-5-26 09:26:37
* @return bool true 更新数据成功 false 更新数目失败
*/
bool QtSqlLiteTest::updateData()
{
    bool commitState = false;
    m_model->database().transaction();  //开始事务操作
    if (m_model->submitAll())           // 提交所有被修改的数据到数据库中
    {
        commitState = m_model->database().commit();   //提交成功，事务将真正修改数据库数据
    }
    else
    {
        m_model->database().rollback(); //提交失败，事务回滚
    }

    if(!commitState) {qDebug() << m_model->lastError().text();};

    return commitState;
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

/**
* @brief 获取数据库某一列的数据
* @author LuChenQun
* @date 2015-5-26 14:54:29
* @param[in] columnIndex 行索引
* @param[in] columnName 列名字
* @return QVariant 数据
*/
QVariant QtSqlLiteTest::getRecordValue(int rowIndex, QString columnName)
{
    QSqlQueryModel model;
    model.setQuery(QString("SELECT * FROM %1").arg(m_dbTable));
    return model.record(rowIndex).value(columnName);
}
