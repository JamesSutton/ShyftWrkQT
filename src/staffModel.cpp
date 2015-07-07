﻿#include "src/staffmodel.h"

#include <QObject>
#include <QDebug>
#include <QSortFilterProxyModel>
#include <QPluginLoader>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkCookieJar>
#include <QNetworkRequest>

StaffModel::StaffModel(QObject *parent)
    :RestClient(parent)
{
}


int StaffModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    if(m_data.isEmpty())
        return 0;

    return m_data.count();
}

QVariant StaffModel::data(const QModelIndex &index, int role) const
{
    if (index.row() < 0 || index.row() >= m_data.count())
        return QVariant();

    const EmployeeData *data = m_data[index.row()];
    switch(role)
    {
    case nameRole:
        return data->name();
    case portraitRole:
        return data->portrait();
    case positionRole:
        return data->positions();
    default:
        return QVariant();
    }
}

bool StaffModel::setData(const QModelIndex &index, QVariant &value, int role)
{
    if(index.isValid() && index.row() <= this->rowCount() && index.row() >= 0)
        return false;

        switch(role)
        {
        case nameRole:
            m_data[index.row()]->setName(value.toString());
            break;
        case portraitRole:
            m_data[index.row()]->setPortrait(value.toString());
            break;
        case positionRole:
            m_data[index.row()]->setPositions(value.toString());
            break;            
        default:
            return false;
        }

        emit dataChanged(index, index);

        return true;
}

Qt::ItemFlags StaffModel::flags(const QModelIndex &index) const
{
    if(index.isValid())
        return StaffModel::flags(index) | Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsSelectable;

    return Qt::NoItemFlags;
}


void StaffModel::addPerson(EmployeeData* person)
{
    if(std::find(m_data.begin(),m_data.end(), person) != m_data.end()) //This doesn't do anything
    {
        qDebug() << "name already exists";
        return;
    }

    beginInsertRows(QModelIndex(), this->rowCount(), this->rowCount());

    m_data << person;

    endInsertRows();
}


bool StaffModel::pullJsonData() //pulls from configured SQL server with parameters specified in configSQL().
{


    int nameField = query.record().indexOf("Name");
    int positionField = query.record().indexOf("Position");
    int portraitField = query.record().indexOf("Portrait");
    QUrl baseURL("http://shyftwrk.com:80");

    while(query.next()){
        QString Name = query.value(nameField).toString();
        QString Position = query.value(positionField).toString();
        QUrl relative = query.value(portraitField).toUrl();
        this->addPerson(new EmployeeData(baseURL.resolved(relative), Name, Position));
        setHeaderData(Position);
    }
    return true;
}

bool StaffModel::addPersonToSql( EmployeeData * Person)
{
        QSqlQuery query(db);
        query.prepare("INSERT INTO `Employees` (id, Name, Position, Portrait, Individual_Performance, Interpersonal_Performance)"
                        "Values (:id, :Name, :Position, :Portrait)");

        query.bindValue(":id", QVariant()); //QVariant() == NULL, which tells mysql to auto_inc
        query.bindValue(":Name", Person->name());
        query.bindValue(":Position", Person->positions());
        query.bindValue(":Portrait", Person->portrait());
        qDebug()<<"person added to database? "<<query.exec();
        if(!query.isValid())
        {
            return false;
        }
        return true;
}

void StaffModel::removePerson(int col)
{
    QList<EmployeeData*>::iterator itr;

    itr = m_data.begin();

    beginRemoveRows(QModelIndex(), col, col);

   std::advance(itr, col);

    m_data.erase(itr);

    endRemoveRows();

}

QVariant StaffModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    Q_UNUSED(orientation);  // orientation and role are unused as this is strictly header data storage, and all headers are from position
    Q_UNUSED(role);

    if(section > headers.size())
        return QVariant();
    return headers.value(section);
}


void StaffModel::setHeaderData(const QString &value)
{
    for(int i=0; i<=headers.size()-1; i++)
    {
        if(headers.value(i) == value)
            return;
    }
    headers[headers.size()] = value; // this should "append" headers
}

int StaffModel::headerSize()
{
    return headers.size();
}

void StaffModel::setHeaderDataSlot(const QString &value)
{
    setHeaderData(value);
}

QStringList StaffModel::headerList()
{
    QStringList temp;
    for(int i=0; i<headers.size(); i++)
    {
        temp.append(headers.value(i));
    }
    return temp;
}

QHash<int, QByteArray> StaffModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[nameRole] = "name";
    roles[portraitRole] = "portrait";
    roles[positionRole] = "position";
    roles[avgPerformanceRole] = "avgPerformance";
    roles[synergyRole] = "synergy";
    roles[avgShiftsRole] = "avgShifts";
    return roles;
}
EmployeeData* StaffModel::getPerson(size_t index)
{
    return m_data[index];
}

