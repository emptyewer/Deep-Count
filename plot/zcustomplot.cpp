#include "zcustomplot.h"
#include <QDebug>

zcustomplot::zcustomplot(QWidget * parent)
    : QCustomPlot(parent)
    , mZoomMode(true)
    , mRubberBand(new QRubberBand(QRubberBand::Rectangle, this))
    , lRubberBand(new QRubberBand(QRubberBand::Line, this))
{
    QPalette palette;
    QColor color(Qt::blue);
    palette.setBrush(QPalette::Foreground, QBrush(color));
    lRubberBand->setPalette(palette);
}

void zcustomplot::setZoomMode(bool mode)
{
    mZoomMode = mode;
}

void zcustomplot::mousePressEvent(QMouseEvent * event)
{
    if (mZoomMode)
    {
        if (event->button() == Qt::LeftButton)
        {
            mOrigin = event->pos();
            mRubberBand->setGeometry(QRect(mOrigin, QSize()));
            mRubberBand->show();
        }

        if (event->button() == Qt::RightButton)
        {
            rescaleAxes();
            replot();
        }
    }
    QCustomPlot::mousePressEvent(event);
}

void zcustomplot::mouseMoveEvent(QMouseEvent * event)
{
    if (mRubberBand->isVisible())
    {
        mRubberBand->setGeometry(QRect(mOrigin, event->pos()).normalized());
    }

    if (!mRubberBand->isVisible()) {
        if (event->pos().x() > xAxis->coordToPixel(xAxis->range().lower) &&
            event->pos().x() < xAxis->coordToPixel(xAxis->range().upper))
        {
            lRubberBand->setGeometry(QRect(QPoint(event->pos().x(), yAxis->coordToPixel(yAxis->range().upper)),
                                           QPoint(event->pos().x()+2, yAxis->coordToPixel(yAxis->range().lower))));
            lRubberBand->show();
        }
        else {
            lRubberBand->hide();
        }
    }

    QCustomPlot::mouseMoveEvent(event);
}

void zcustomplot::mouseReleaseEvent(QMouseEvent * event)
{
    if (mRubberBand->isVisible())
    {
        const QRect zoomRect = mRubberBand->geometry();
        int xp1, yp1, xp2, yp2;
        zoomRect.getCoords(&xp1, &yp1, &xp2, &yp2);
        double x1 = xAxis->pixelToCoord(xp1);
        double x2 = xAxis->pixelToCoord(xp2);
        double y1 = yAxis->pixelToCoord(yp1);
        double y2 = yAxis->pixelToCoord(yp2);

        xAxis->setRange(x1, x2);
        yAxis->setRange(y1, y2);

        mRubberBand->hide();
        replot();
    }
    QCustomPlot::mouseReleaseEvent(event);
}
