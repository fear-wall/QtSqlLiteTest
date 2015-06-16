#include "EditAbleSqlModel.h"

EditAbleSqlModel::EditAbleSqlModel(QObject *parent)
    : QSqlTableModel(parent)
{

}

EditAbleSqlModel::~EditAbleSqlModel()
{

}


Qt::ItemFlags EditAbleSqlModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags flags = QSqlQueryModel::flags(index);
    if(index.column() >= 2)
    {
        flags |= Qt::ItemIsEditable;
    }

    return flags;
}


bool EditAbleSqlModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    return this->setData(index, value);
}
