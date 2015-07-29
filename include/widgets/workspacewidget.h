#ifndef WORKSPACEWIDGET_H
#define WORKSPACEWIDGET_H

#include <QWidget>
#include <QHBoxLayout>
#include <QTreeWidget>

class WorkspaceWidget : public QWidget
{
    Q_OBJECT
public:
    explicit WorkspaceWidget(QWidget *parent = 0);

private:
    QHBoxLayout *pHLayout;

signals:

public slots:
    void loadPatient(QTreeWidget *pTreeWidget);
};

#endif // WORKSPACEWIDGET_H
