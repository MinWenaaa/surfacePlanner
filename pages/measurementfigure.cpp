#include "measurementfigure.h"
#include "ui_measurementfigure.h"

measurementFigure::measurementFigure(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::measurementFigure), seriousNum(0), pointNum(0), last_x(0), last_y(0), currentDistance(0)
{
    ui->setupUi(this);
    view_3d = ui->view_3d;
    view_elevation = ui->view_elevation;
    view_plan = ui->view_plan;
}

void measurementFigure::setUpView(int type) {
    elevationChart = new QChart();
    eleAxisX = new QValueAxis();  eleAxisY = new QValueAxis();
    eleAxisY->setRange(0, 2); eleAxisX->setRange(0, 30);
    elevationChart->addAxis(eleAxisX, Qt::AlignBottom);
    elevationChart->addAxis(eleAxisY, Qt::AlignLeft);

    m_elevationChartView = new QChartView(elevationChart);
    m_elevationChartView->setRenderHint(QPainter::Antialiasing);
    m_elevationChartView->setStyleSheet("background: transparent; border: none;");
    elevationChart->setBackgroundBrush(Qt::transparent);

    QVBoxLayout *layout2 = new QVBoxLayout(type ? view_3d : view_elevation);
    layout2->setContentsMargins(0, 0, 0, 0);
    layout2->addWidget(m_elevationChartView);



    planChart = new QChart();
    planAxisX = new QValueAxis(); planAxisY = new QValueAxis();
    planAxisX->setRange(0, 30); planAxisY->setRange(0, 30);
    planChart->addAxis(planAxisX, Qt::AlignBottom);
    planChart->addAxis(planAxisY, Qt::AlignLeft);

    m_planChartView = new QChartView(planChart);
    m_planChartView->setRenderHint(QPainter::Antialiasing);
    m_planChartView->setStyleSheet("background: transparent; border: none;");
    planChart->setBackgroundBrush(Qt::transparent);

    QVBoxLayout *layout3 = new QVBoxLayout(view_plan);
    layout3->setContentsMargins(0, 0, 0, 0);
    layout3->addWidget(m_planChartView);



    m_scatter = new Q3DScatter();
    m_scatter->setAspectRatio(1.0);
    m_scatter->setHorizontalAspectRatio(1.0);
    m_scatter->scene()->activeCamera()->setZoomLevel(150);
    m_scatter->setAxisX(new QValue3DAxis); m_scatter->setAxisY(new QValue3DAxis); m_scatter->setAxisZ(new QValue3DAxis);
    m_scatter->axisX()->setRange(-10, 10); m_scatter->axisY()->setRange(-10, 10); m_scatter->axisZ()->setRange(-10, 10);
    QWidget *container = QWidget::createWindowContainer(m_scatter);
    container->setAttribute(Qt::WA_TranslucentBackground);
    container->setStyleSheet("background: transparent; border: none;");
    QVBoxLayout *layout = new QVBoxLayout(type ? view_elevation : view_3d);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(container);

    Q3DTheme *theme = m_scatter->activeTheme();
    //theme->setGridEnabled(false);
    theme->setLabelBorderEnabled(false);  // 启用坐标轴边框
    theme->setBackgroundEnabled(false);  // 禁用主题背景
    theme->setBackgroundColor(Qt::transparent);  // 主题背景透明
    theme->setWindowColor(backgroundColor);

}

void measurementFigure::addSerious(const QString& name, const QColor& color) {
    seriousNum++; currentDistance = 0; pointNum = 0;
    QLineSeries *eleSeries = new QLineSeries();
    eleSeries->setName(name); eleSeries->setColor(color);
    m_elevationSeries.append(eleSeries);

    QScatterSeries *planSeries = new QScatterSeries();
    planSeries->setName(name);
    planSeries->setColor(color);
    planSeries->setMarkerSize(5);
    planSeries->setBorderColor(Qt::transparent);
    m_planSeries.append(planSeries);

    QScatter3DSeries *tSeries = new QScatter3DSeries();
    tSeries->setMesh(QAbstract3DSeries::MeshPoint);
    tSeries->setItemSize(0.1f);
    tSeries->setBaseColor(color);
    m_3dSeries.append(tSeries);

    tempElevationSerious = eleSeries;
    tempPlanSerious = planSeries;
    temp3dSerious = tSeries;
    planChart->addSeries(tempPlanSerious);
    elevationChart->addSeries(tempElevationSerious);
    m_scatter->addSeries(temp3dSerious);
    tempPlanSerious->attachAxis(planAxisX);  // 平面图绑定平面图的X轴
    tempPlanSerious->attachAxis(planAxisY);  // 平面图绑定平面图的Y轴
    tempElevationSerious->attachAxis(eleAxisX);  // 高程图绑定高程图的X轴
    tempElevationSerious->attachAxis(eleAxisY);  // 高程图绑定高程图的Y轴
}

void measurementFigure::addPoint(double x, double y, double z) {
    if(pointNum) {
        currentDistance += std::sqrt((x-last_x)*(x-last_x) + (y-last_y) *(y-last_y));
    }
    if(seriousNum==1) {
        if(!pointNum) {
            max_x = min_x = x; max_y = min_y = y; max_z = min_z = z;
            eleAxisY->setRange(z - 4, z + 4);
            planAxisX->setRange(min_x-1, max_x+1);
            planAxisY->setRange(min_y-1, max_y+1);
        }
        min_x = x < min_x ? x : min_x; max_x = x > max_x ? x : max_x;
        min_x = y < min_x ? y : min_x; max_y = y > max_y ? y : max_y;
        min_z = z < min_z ? z : min_z; max_z = z > max_z ? z : max_z;

        if(min_x < planAxisX->min()+0.5 || max_x > planAxisX->max()-0.5){
            planAxisX->setRange(min_x-1, max_x+1);
            m_scatter->axisX()->setRange(min_x-1, max_x+1);
        }
        if(min_y < planAxisY->min()+0.5 || max_y > planAxisY->max()-0.5) {
            planAxisY->setRange(min_y-1, max_y+1);
            m_scatter->axisZ()->setRange(min_y-1, max_y+1);
        }
        if(min_z < eleAxisY->min()+1 || max_z > eleAxisY->max()-1){
            eleAxisY->setRange(min_z-2, max_z+2);
            m_scatter->axisY()->setRange(min_z-2, max_z+2);
        }
        if(currentDistance > eleAxisX->max() - 50) {
            eleAxisX->setRange(0, currentDistance+100);
        }
    }
    pointNum++;

    QPointF elevationPoint(currentDistance, z);
    QPointF planPoint(x, y);
    QVector3D threeDPoint(x, z, y);

    tempElevationSerious->append(elevationPoint);
    tempPlanSerious->append(planPoint);
    temp3dSerious->dataProxy()->addItem(threeDPoint);

    planChart->update();
    elevationChart->update();
}

void measurementFigure::clearAllData() {
    for(int i=0; i<seriousNum;i++) {
        if(!m_elevationSeries[i]) return;
        m_elevationSeries[i]->clear();
        m_planSeries[i]->clear();
        m_3dSeries[i]->dataProxy()->removeItems(0, m_3dSeries[i]->dataProxy()->itemCount());
        planChart->removeSeries(m_planSeries[i]);
        elevationChart->removeSeries(m_elevationSeries[i]);
        m_scatter->removeSeries(m_3dSeries[i]);
    }
}

QVector<QVector<QPointF>>& measurementFigure::getEleData() {
    m_eleDataCache.clear();
    for (QLineSeries* series : m_elevationSeries) {
        if (series) {
            m_eleDataCache.append(series->pointsVector());
        }
    }
    return m_eleDataCache;
}

measurementFigure::~measurementFigure()
{
    delete ui;
}
