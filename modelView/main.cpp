#include <QApplication>
#include <QFile>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QPushButton>
#include <QTableView>
#include <QVBoxLayout>
#include <QWidget>
#include "CustomTableModel.h"
#include "SpinBoxDelegate.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QFile qssFile(":/style.css");
    if (qssFile.open(QFile::ReadOnly | QFile::Text)) {
        app.setStyleSheet(qssFile.readAll());
        qssFile.close();
    }

    QWidget *window = new QWidget;
    window->setWindowTitle("自定义表模型 Demo - 员工信息表");
    window->resize(700, 420);

    CustomTableModel *model = new CustomTableModel(window);

    QTableView *tableView = new QTableView(window);
    tableView->setModel(model);

    tableView->horizontalHeader()->setStretchLastSection(true);
    tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    tableView->setAlternatingRowColors(true);
    tableView->setSortingEnabled(true);

    // SpinBoxDelegate *ageDelegate = new SpinBoxDelegate(tableView);
    // tableView->setItemDelegateForColumn(CustomTableModel::ColAge, ageDelegate);

    QPushButton *insertBtn = new QPushButton("插入行", window);
    QPushButton *removeBtn = new QPushButton("删除行", window);
    QPushButton *appendBtn = new QPushButton("追加行", window);

    QHBoxLayout *btnLayout = new QHBoxLayout;
    btnLayout->addStretch();
    btnLayout->addWidget(insertBtn);
    btnLayout->addWidget(removeBtn);
    btnLayout->addWidget(appendBtn);
    btnLayout->addStretch();

    QObject::connect(insertBtn, &QPushButton::clicked, [=]() {
        QModelIndex cur = tableView->currentIndex();
        int row = cur.isValid() ? cur.row() : 0;
        model->insertRow(row);
    });

    QObject::connect(removeBtn, &QPushButton::clicked, [=]() {
        QModelIndex cur = tableView->currentIndex();
        if (cur.isValid()) {
            model->removeRow(cur.row());
        }
    });

    QObject::connect(appendBtn, &QPushButton::clicked, [=]() {
        Employee emp;
        emp.name = "新员工";
        emp.age = 0;
        emp.department = "未分配";
        emp.salary = 0.0;
        model->appendEmployee(emp);
    });

    QVBoxLayout *mainLayout = new QVBoxLayout(window);
    mainLayout->addWidget(tableView);
    mainLayout->addLayout(btnLayout);

    window->show();
    return app.exec();
}
