#include "elevationanalyzer.h"
#include <algorithm>
#include <numeric>
#include <QtMath>

ElevationAnalyzer::ElevationAnalyzer(QObject *parent) : QObject(parent) {}

void ElevationAnalyzer::analyzeElevationData(const QVector<QVector<QPointF>>& elevationData)
{
    if (elevationData.size() != 3) {
        emit errorOccurred("Expected 3 sets of elevation data");
        return;
    }

    try {
        QVector<double> allDistances;
        for (const auto& dataset : elevationData) {
            for (const auto& point : dataset) {
                allDistances.append(point.x());
            }
        }
        auto last = std::unique(allDistances.begin(), allDistances.end());
        allDistances.erase(last, allDistances.end());

        // 为每组数据创建插值后的高程数据
        QVector<QVector<double>> interpolatedElevations(3);
        for (int i = 0; i < 3; ++i) {
            interpolatedElevations[i] = interpolateElevation(elevationData[i], allDistances);
        }

        // 计算两两之间的统计量并发射信号
        for (int i = 0; i < 3; ++i) {
            for (int j = i + 1; j < 3; ++j) {
                double maxDiff = calculateMaxElevationDifference(interpolatedElevations[i],
                                                                 interpolatedElevations[j]);
                double correlation = calculateCorrelation(interpolatedElevations[i],
                                                          interpolatedElevations[j]);

                emit analysisComplete(i+1, j+1, maxDiff, correlation);
            }
        }
        emit allFinished();
    } catch (const std::exception& e) {
        emit errorOccurred(QString("Analysis error: %1").arg(e.what()));
    }
}

QVector<double> ElevationAnalyzer::interpolateElevation(const QVector<QPointF>& data, const QVector<double>& distances) {
    QVector<double> elevations;
    if (data.isEmpty()) return elevations;

    for (double dist : distances) {
        // 查找包围给定距离的两个点
        auto it = std::lower_bound(data.begin(), data.end(), dist,
                                   [](const QPointF& point, double value) {
                                       return point.x() < value;
                                   });

        if (it == data.begin()) {
            // 距离小于第一个点，取第一个点的高程
            elevations.append(data.first().y());
        } else if (it == data.end()) {
            // 距离大于最后一个点，取最后一个点的高程
            elevations.append(data.last().y());
        } else {
            // 在两个点之间进行线性插值
            QPointF p1 = *(it - 1);
            QPointF p2 = *it;
            double ratio = (dist - p1.x()) / (p2.x() - p1.x());
            double elev = p1.y() + ratio * (p2.y() - p1.y());
            elevations.append(elev);
        }
    }

    return elevations;
}

// 计算最大高程差
double ElevationAnalyzer::calculateMaxElevationDifference(const QVector<double>& elev1, const QVector<double>& elev2) {
    if (elev1.size() != elev2.size()) return 0.0;

    double maxDiff = 0.0;
    for (int i = 0; i < elev1.size(); ++i) {
        double diff = qAbs(elev1[i] - elev2[i]);
        if (diff > maxDiff) {
            maxDiff = diff;
        }
    }
    return maxDiff;
}

// 计算Pearson相关系数
double ElevationAnalyzer::calculateCorrelation(const QVector<double>& elev1, const QVector<double>& elev2) {
    if (elev1.size() != elev2.size() || elev1.size() < 2) return 0.0;

    // 计算均值
    double mean1 = std::accumulate(elev1.begin(), elev1.end(), 0.0) / elev1.size();
    double mean2 = std::accumulate(elev2.begin(), elev2.end(), 0.0) / elev2.size();

    double numerator = 0.0;    // 协方差
    double denom1 = 0.0;      // elev1的方差
    double denom2 = 0.0;      // elev2的方差

    for (int i = 0; i < elev1.size(); ++i) {
        double diff1 = elev1[i] - mean1;
        double diff2 = elev2[i] - mean2;

        numerator += diff1 * diff2;
        denom1 += diff1 * diff1;
        denom2 += diff2 * diff2;
    }

    if (denom1 == 0.0 || denom2 == 0.0) return 0.0;

    return numerator / (qSqrt(denom1) * qSqrt(denom2));
}
