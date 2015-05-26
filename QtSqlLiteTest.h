#ifndef QTSQLLITETEST_H
#define QTSQLLITETEST_H

#include <QWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QSqlDatabase>
#include <QTableView>
#include <QSqlTableModel>
#include <QDir>
#include <QString>

class QtSqlLiteTest : public QWidget
{
    Q_OBJECT

public:
    QtSqlLiteTest(QWidget *parent = 0);
    ~QtSqlLiteTest();

    bool createDB(const QString dbPath, const QString dbName);
    bool sqlQuery(const QString sql);
    QVariant getRecordValue(int rowIndex, QString columnName);
private:
    void initWidget();
    void initModel();
    bool updateData();
private slots:
    void addData();
    void showContextmenu(const QPoint& point);
    void delData();
    void alterData();
private:
    QSqlDatabase m_db;
    QPushButton* m_addDataBtn;
    QLineEdit *m_songNameLe;
    QLineEdit *m_singerLe;

    QTableView *m_tableView;
    QSqlTableModel *m_model;

    const QString m_dbPath = QDir::currentPath();
    const QString m_dbName = "myDB.db";
    const QString m_dbTable = "song_info";

    /** @brief 表头说明 */
    const QStringList m_headInfoList =
        (QStringList() << QStringLiteral("索引") << QStringLiteral("歌名") << QStringLiteral("歌手") << QStringLiteral("日期"));

    /** @brief sqlite数据库里面表头内容，跟m_headInfoList一一对应 */
    const QStringList m_headTableList =
        (QStringList() << ("id") << ("song_name") << ("singer") << ("date"));
};

#endif // QTSQLLITETEST_H
