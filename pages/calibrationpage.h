#ifndef CALIBRATIONPAGE_H
#define CALIBRATIONPAGE_H

#include <QWidget>
#include <QGraphicsScene>

namespace Ui {
class CalibrationPage;
}

class CalibrationPage : public QWidget
{
    Q_OBJECT

public:
    explicit CalibrationPage(QWidget *parent = nullptr);
    ~CalibrationPage();

private:
    Ui::CalibrationPage *ui;

    QGraphicsScene *scene;
    QGraphicsEllipseItem *circle;      // 圆对象
    void renderInclination(double, double);
};

#endif // CALIBRATIONPAGE_H
