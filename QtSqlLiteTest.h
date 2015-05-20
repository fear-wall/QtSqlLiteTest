#ifndef QTSQLLITETEST_H
#define QTSQLLITETEST_H

#include <QWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QSqlDatabase>
#include <QTableView>
#include <QSqlTableModel>

class QtSqlLiteTest : public QWidget
{
    Q_OBJECT

public:
    QtSqlLiteTest(QWidget *parent = 0);
    ~QtSqlLiteTest();

    bool createDB(const QString dbPath, const QString dbName);
    bool sqlQuery(const QString sql);
private slots:
    void addData();
private:
    QSqlDatabase m_db;
    QPushButton* m_addDataBtn;
    QLineEdit *m_eventLE;
    QLineEdit *m_msgLE;
    QLineEdit *m_volumeLE;

    QTableView *m_tableView;
    QSqlTableModel *m_model;
};

#endif // QTSQLLITETEST_H
