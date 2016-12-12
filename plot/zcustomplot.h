#ifndef ZCUSTOMPLOT_H
#define ZCUSTOMPLOT_H

#include <QPoint>
#include "qcustomplot.h"

class QRubberBand;
class QMouseEvent;
class QWidget;

class zcustomplot : public QCustomPlot
{
    Q_OBJECT

public:
    zcustomplot(QWidget * parent = 0);
//    virtual ~zcustomplot();
    void setZoomMode(bool mode);

protected:
    void mousePressEvent(QMouseEvent * event);
    void mouseMoveEvent(QMouseEvent * event);
    void mouseReleaseEvent(QMouseEvent * event);

private:
    bool mZoomMode;
    QRubberBand *mRubberBand;
    QRubberBand *lRubberBand;
    QPoint mOrigin;
};


#endif // ZCUSTOMPLOT_H
