#ifndef EMPLOYEEMODELMASTER_H
#define EMPLOYEEMODELMASTER_H
#include <QObject>
#include <QList>
#include <QAbstractListModel>
#include <QSortFilterProxyModel>
#include <QSqlDatabase>
#include "employeedata.h"
class EmployeeModelMaster : public QAbstractListModel
{

/* this is the base model that interacts with the sql database, all other models mirror their data from here
 * the model is directly used for constructing the ScrollableEmployeeColumn and is masked with a
 *  QSortFilterProxyModel for adaptive search functionality*/

    Q_OBJECT

public:

    typedef QList<EmployeeData*>::const_iterator const_iterator;

    explicit EmployeeModelMaster(QObject *parent=0);

public:

    enum EmployeeModelMasterDataRole
    {
        nameRole=Qt::UserRole+1,
        positionRole,
        portraitRole,
        scoreRole
    };
    //virtual inherited members from QAbstractTableModel
public:

    int rowCount(const QModelIndex &parent = QModelIndex()) const;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole)const;

    bool setData(const QModelIndex &index,  QVariant &value, int role);

    Qt::ItemFlags flags(const QModelIndex &parent = QModelIndex()) const;


public:

    void configSQL();

    bool pullFromSQL();

    bool addPersonToSql (EmployeeData *Person);

    void addPerson(EmployeeData *person);

    void removePerson(int index);

    EmployeeData* getPerson(size_t index);

    const_iterator begin()const{return m_data.begin();}

    const_iterator end()const{return m_data.end();}



signals:

    void nameChanged(QString& newname);

    void updateMirrors(int newRowCount, EmployeeModelMaster* master);

protected:

    QHash<int, QByteArray> roleNames() const;

    QSqlDatabase db;

public:

    QList<EmployeeData*> m_data;


};

#endif // EMLOYEEMODELMASTER_H
