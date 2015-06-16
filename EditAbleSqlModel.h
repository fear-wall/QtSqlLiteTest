#ifndef EDITABLESQLMODEL_H
#define EDITABLESQLMODEL_H
#include <QSqlTableModel>
#include <QTableWidget>

class EditAbleSqlModel : public QSqlTableModel
{
public:
    EditAbleSqlModel(QObject *parent = 0);
    ~EditAbleSqlModel();
    Qt::ItemFlags flags(const QModelIndex &index) const Q_DECL_OVERRIDE;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) Q_DECL_OVERRIDE;
};

#endif // EDITABLESQLMODEL_H
