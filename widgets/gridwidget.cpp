#include "include/widgets/gridwidget.h"
#include "include/widgets/imagewidget.h"

#include <QApplication>
#include <QDebug>
#include <QKeyEvent>

GridWidget::GridWidget(QWidget *parent) :
    QWidget(parent)
{
    this->pScrollArea = new QScrollArea;
    Q_ASSERT(this->pScrollArea);

    this->pContainerWidget = new QWidget;
    Q_ASSERT(this->pContainerWidget);

    this->pFlowLayout = new FlowLayout(this);
    Q_ASSERT(this->pFlowLayout);

    this->pVLayout = new QVBoxLayout;
    Q_ASSERT(this->pVLayout);
}

GridWidget::~GridWidget(void)
{
    qDebug() << Q_FUNC_INFO;

    /* This should cause all the image widgets to be deleted as well */
    delete this->pVLayout;
}

void GridWidget::addSlices(const QVector<Slice *> &vecSlices)
{
    qDebug() << Q_FUNC_INFO;

    this->pVecSlices = &vecSlices;

    /* Populate the flow grid layout */
    int nSlices = vecSlices.size();
    for (int i = 0; i < nSlices; i++) {
        Slice *pSlice = vecSlices.at(i);
        Q_ASSERT(pSlice);

        /* Create a thumbnail image of the slice */
        ImageWidget *pMyImageWidget = new ImageWidget();
        Q_ASSERT(pMyImageWidget);

        /* Associate slice <---> image widget */
        pMyImageWidget->setSlice(pSlice);

        /* If user double clicks on a slice, we'd like to load it.
         * Hence, propagate the signa.  */
        connect(pMyImageWidget, SIGNAL(sliceDoubleClicked(const Slice *)),
                          this, SIGNAL(sliceDoubleClicked(const Slice *)));

        this->pFlowLayout->addWidget(pMyImageWidget);
    }

    this->pContainerWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    this->pContainerWidget->setLayout(this->pFlowLayout);
    this->pScrollArea->setWidgetResizable(true);
    this->pScrollArea->setWidget(this->pContainerWidget);
    this->setLayout(this->pVLayout);
    this->pVLayout->addWidget(this->pScrollArea);

    /* Set background color */
    QPalette pal(palette());
    pal.setColor(QPalette::Background, QColor("#D3D3D3"));
    this->pContainerWidget->setAutoFillBackground(true);
    this->pContainerWidget->setPalette(pal);
}

void GridWidget::selectAll(void)
{
    qDebug() << Q_FUNC_INFO;

    Q_ASSERT(this->pFlowLayout);
    int nImageWidgets = this->pFlowLayout->count();
    for (int i = 0; i < nImageWidgets; i++) {
        ImageWidget *pImageWidget = (ImageWidget *)this->pFlowLayout->itemAt(i)->widget();
        Q_ASSERT(pImageWidget);
        bool isSelected = pImageWidget->isSelected();
        pImageWidget->Selectable::setSelection(!isSelected);    // toggle
        pImageWidget->update();
    }
}

void GridWidget::changeWindow(const QWidget *currentWidget, HUWindows::window newWindow)
{
    qDebug() << Q_FUNC_INFO;

    /*
     * When user changes the window level at the main screen widget, a signal is emitted
     * towards all the stacked widgets (GridWidget, SliceWidget, etc). We shall react to
     * it if and only if we are the currently activated widget in the StackedWidget.
     *
     * Otherwise, we may be looking at a particular slice and GridWidget will be changing
     * the window to all slices, which will be slow and unnecessary.
     */
    if (currentWidget != this) {
        return;
        /* NEVER REACHED */
    }

    /* This usually takes some time, so let others know, e.g. the MainWindow,
     * so that they change the cursor from default icon to wait cursor.
     */
    emit this->heavyTaskInitiated();

    int nImageWidgets = this->pFlowLayout->count();
    for (int i = 0; i < nImageWidgets; i++) {
        ImageWidget *pImageWidget = (ImageWidget *)this->pFlowLayout->itemAt(i)->widget();
        Q_ASSERT(pImageWidget);

        pImageWidget->changeWindow(newWindow);
    }

    /* We are done */
    emit this->heavyTaskCompleted();
}

bool GridWidget::event(QEvent *pEvent)
{
    if (pEvent->type() == QEvent::KeyPress) {
        QKeyEvent *pke = static_cast<QKeyEvent *>(pEvent);
        int key = pke->key();
        if (key == Qt::Key_Escape) {
            emit this->backToStartupMenu();
        } else if (key == Qt::Key_A
                   && (QApplication::keyboardModifiers() & Qt::ControlModifier)) {
            selectAll();
        }
    }
    return true;
}
