#ifndef ELEVATIONANALYZER_H
#define ELEVATIONANALYZER_H

#include <QVector>
#include <QObject>
#include <QThread>
#include <QPointF>

class ElevationAnalyzer  : public QObject{
    Q_OBJECT
public:
    explicit ElevationAnalyzer(QObject *parent = nullptr);

public slots:
    // 计算两两之间的最大高程差和相关性
    void analyzeElevationData(const QVector<QVector<QPointF>>& elevationData);

signals:
    void analysisComplete(int dataset1, int dataset2, double maxDiff, double correlation);
    void allFinished();
    void errorOccurred(const QString& message);

private:
    // 线性插值函数
    QVector<double> interpolateElevation(const QVector<QPointF>& data, const QVector<double>& distances);

    // 计算最大高程差
    double calculateMaxElevationDifference(const QVector<double>& elev1, const QVector<double>& elev2);

    // 计算Pearson相关系数
    double calculateCorrelation(const QVector<double>& elev1, const QVector<double>& elev2);
};

#endif // ELEVATIONANALYZER_H
