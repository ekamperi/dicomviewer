#ifndef WORKSPACEWIDGET_H
#define WORKSPACEWIDGET_H

#include <QWidget>
#include <QHBoxLayout>
#include <QTreeWidget>

#include "series.h"

class WorkspaceWidget : public QWidget
{
    Q_OBJECT
public:
    explicit WorkspaceWidget(QWidget *parent = 0);

private:
    QHBoxLayout *pHLayout;

signals:

public slots:
    void loadSeries(const Series &series);
};

#endif // WORKSPACEWIDGET_H
