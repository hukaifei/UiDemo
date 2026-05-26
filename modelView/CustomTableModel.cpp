#include "CustomTableModel.h"
#include <QColor>
#include <QFont>
#include <QIcon>

CustomTableModel::CustomTableModel(QObject *parent)
    : QAbstractTableModel(parent)
{
    m_employees = {
        {"张三", 28, "技术部", 15000.0},
        {"李四", 32, "市场部", 12000.0},
        {"王五", 25, "人事部", 9000.0},
        {"赵六", 30, "技术部", 18000.0},
        {"孙七", 27, "财务部", 11000.0},
        {"周八", 35, "技术部", 22000.0},
        {"吴九", 29, "市场部", 13000.0},
    };
}

int CustomTableModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return m_employees.size();
}

int CustomTableModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return ColumnCount;
}

QVariant CustomTableModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= m_employees.size())
        return QVariant();

    const Employee &emp = m_employees.at(index.row());

    if (role == Qt::DisplayRole || role == Qt::EditRole) {
        switch (index.column()) {
        case ColName:       return emp.name;
        case ColAge:        return emp.age;
        case ColDepartment: return emp.department;
        case ColSalary:     return QString::number(emp.salary, 'f', 2);
        }
    }

    if (role == Qt::TextAlignmentRole) {
        switch (index.column()) {
        case ColName:       return int(Qt::AlignLeft | Qt::AlignVCenter);
        case ColAge:        return int(Qt::AlignCenter | Qt::AlignVCenter);
        case ColDepartment: return int(Qt::AlignLeft | Qt::AlignVCenter);
        case ColSalary:     return int(Qt::AlignRight | Qt::AlignVCenter);
        }
    }

    if (role == Qt::ForegroundRole) {
        if (emp.salary >= 15000.0)
            return QColor(Qt::red);
        if (emp.department == "技术部")
            return QColor(Qt::darkBlue);
    }

    if (role == Qt::FontRole) {
        if (emp.salary >= 15000.0) {
            QFont font;
            font.setBold(true);
            return font;
        }
    }

    if (role == Qt::ToolTipRole) {
        return QString("%1, %2岁, %3, 月薪%4元")
            .arg(emp.name)
            .arg(emp.age)
            .arg(emp.department)
            .arg(QString::number(emp.salary, 'f', 2));
    }

    return QVariant();
}

QVariant CustomTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        switch (section) {
        case ColName:       return "姓名";
        case ColAge:        return "年龄";
        case ColDepartment: return "部门";
        case ColSalary:     return "月薪(元)";
        }
    }

    if (orientation == Qt::Vertical && role == Qt::DisplayRole) {
        return QString::number(section + 1);
    }

    return QVariant();
}

bool CustomTableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid() || index.row() >= m_employees.size())
        return false;

    if (role == Qt::EditRole) {
        Employee &emp = m_employees[index.row()];
        switch (index.column()) {
        case ColName:
            emp.name = value.toString();
            break;
        case ColAge:
            emp.age = value.toInt();
            break;
        case ColDepartment:
            emp.department = value.toString();
            break;
        case ColSalary:
            emp.salary = value.toDouble();
            break;
        default:
            return false;
        }
        emit dataChanged(index, index, {role});
        return true;
    }
    return false;
}

Qt::ItemFlags CustomTableModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
}

bool CustomTableModel::insertRows(int row, int count, const QModelIndex &parent)
{
    if (parent.isValid())
        return false;
    if (row < 0 || row > m_employees.size())
        return false;

    beginInsertRows(parent, row, row + count - 1);
    for (int i = 0; i < count; ++i) {
        Employee emp;
        emp.name = "新员工";
        emp.age = 0;
        emp.department = "未分配";
        emp.salary = 0.0;
        m_employees.insert(row + i, emp);
    }
    endInsertRows();
    return true;
}

bool CustomTableModel::removeRows(int row, int count, const QModelIndex &parent)
{
    if (parent.isValid())
        return false;
    if (row < 0 || row + count > m_employees.size())
        return false;

    beginRemoveRows(parent, row, row + count - 1);
    for (int i = 0; i < count; ++i) {
        m_employees.removeAt(row);
    }
    endRemoveRows();
    return true;
}

void CustomTableModel::appendEmployee(const Employee &emp)
{
    int row = m_employees.size();
    beginInsertRows(QModelIndex(), row, row);
    m_employees.append(emp);
    endInsertRows();
}

Employee CustomTableModel::employeeAt(int row) const
{
    return m_employees.value(row);
}
