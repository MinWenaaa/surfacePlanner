#include "measurementfigure.h"
#include "ui_measurementfigure.h"

measurementFigure::measurementFigure(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::measurementFigure)
{
    ui->setupUi(this);
    view_3d = ui->view_3d;
    view_elevation = ui->view_elevation;
    view_plan = ui->view_plan;
    setUpView();
}

void measurementFigure::setUpView() {
    elevationChart = new QChart();
    eleAxisX = new QValueAxis();  eleAxisY = new QValueAxis();
    eleAxisY->setRange(0, 2); eleAxisX->setRange(0, 30);
    elevationChart->addAxis(eleAxisX, Qt::AlignBottom);
    elevationChart->addAxis(eleAxisY, Qt::AlignLeft);

    m_elevationChartView = new QChartView(elevationChart);
    m_elevationChartView->setRenderHint(QPainter::Antialiasing);
    m_elevationChartView->setStyleSheet("background: transparent; border: none;");
    elevationChart->setBackgroundBrush(Qt::transparent);

    QVBoxLayout *layout2 = new QVBoxLayout(view_elevation);
    layout2->setContentsMargins(0, 0, 0, 0);
    layout2->addWidget(m_elevationChartView);

    for (int i = 0; i < 3; ++i) {
        QLineSeries *series = new QLineSeries();
        series->setName(QString("重复测量%1").arg(i+1));
        series->setColor(colors[i]);
        series->setPen(Qt::NoPen);
        series->setMarkerSize(8);
        m_elevationSeries.append(series);

    }




    planChart = new QChart();
    planAxisX = new QValueAxis(); planAxisY = new QValueAxis();
    planAxisX->setRange(0, 30); planAxisY->setRange(0, 30);
    planChart->addAxis(planAxisX, Qt::AlignBottom);
    planChart->addAxis(planAxisY, Qt::AlignLeft);

    m_planChartView = new QChartView(planChart);
    m_planChartView->setRenderHint(QPainter::Antialiasing);
    m_planChartView->setStyleSheet("background: transparent; border: none;");
    planChart->setBackgroundBrush(Qt::transparent);

    QVBoxLayout *layout3 = new QVBoxLayout(view_3d);
    layout3->setContentsMargins(0, 0, 0, 0);
    layout3->addWidget(m_planChartView);

    for (int i = 0; i < 3; ++i) {
        QScatterSeries *series = new QScatterSeries();
        series->setName(QString("重复测量%1").arg(i+1));
        series->setColor(colors[i]);
        series->setMarkerShape(QScatterSeries::MarkerShapeCircle);
        series->setMarkerSize(8);
        m_planSeries.append(series);
    }



    m_scatter = new Q3DScatter();
    m_scatter->setAspectRatio(1.0);
    m_scatter->setHorizontalAspectRatio(1.0);
    m_scatter->scene()->activeCamera()->setZoomLevel(150);
    m_scatter->setAxisX(new QValue3DAxis); m_scatter->setAxisY(new QValue3DAxis); m_scatter->setAxisZ(new QValue3DAxis);
    m_scatter->axisX()->setRange(-10, 10); m_scatter->axisY()->setRange(-10, 10); m_scatter->axisZ()->setRange(-10, 10);
    QWidget *container = QWidget::createWindowContainer(m_scatter);
    container->setAttribute(Qt::WA_TranslucentBackground);
    container->setStyleSheet("background: transparent; border: none;");
    QVBoxLayout *layout = new QVBoxLayout(view_plan);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(container);

    Q3DTheme *theme = m_scatter->activeTheme();
    theme->setGridEnabled(false);
    theme->setLabelBorderEnabled(false);  // 启用坐标轴边框
    theme->setBackgroundEnabled(false);  // 禁用主题背景
    theme->setBackgroundColor(Qt::transparent);  // 主题背景透明
    theme->setWindowColor(backgroundColor);

    for (int i = 0; i < 3; ++i) {
        QScatter3DSeries *series = new QScatter3DSeries();
        series->setMesh(QAbstract3DSeries::MeshPoint);
        series->setItemSize(0.1f);
        series->setBaseColor(colors[i]);
        m_3dSeries.append(series);
    }
}

measurementFigure::~measurementFigure()
{
    delete ui;
}
