#include <QtTest/QtTest>
#include <QElapsedTimer>
#include <QImage>
#include <QVector>
#include "common/types/Frame.h"
#include "common/types/AIResult.h"
#include "infrastructure/cache/LRUCache.h"

class BenchmarkCoreComponents : public QObject {
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();

    void benchmarkFrameCreation();
    void benchmarkAIResultProcessing();
    void benchmarkLRUCachePerformance();
    void benchmarkMemoryAllocation();

private:
    LRUCache<QString, QImage>* m_imageCache = nullptr;
};

void BenchmarkCoreComponents::initTestCase() {
    m_imageCache = new LRUCache<QString, QImage>(1000, 100 * 1024 * 1024, 3600); // 1000项，100MB，1小时TTL
}

void BenchmarkCoreComponents::cleanupTestCase() {
    delete m_imageCache;
}

void BenchmarkCoreComponents::benchmarkFrameCreation() {
    // 基准测试Frame对象的创建和销毁
    QBENCHMARK {
        for (int i = 0; i < 1000; ++i) {
            Frame frame;
            frame.screenIndex = i % 3;
            frame.scaleFactor = 1.0 + (i % 10) * 0.1;
            frame.source = QString("screen_%1").arg(i);
            frame.imageData = QImage(1920, 1080, QImage::Format_RGB32);
        }
    }
}

void BenchmarkCoreComponents::benchmarkAIResultProcessing() {
    // 基准测试AI结果处理
    QVector<AIResult> results;

    QBENCHMARK {
        for (int i = 0; i < 100; ++i) {
            AIResult result;
            result.resultId = QString("result_%1").arg(i);
            result.engineType = "ocr";
            result.modelName = "tesseract";
            result.processingTime = 0.1 + (i % 10) * 0.01;
            result.confidence = 0.8 + (i % 20) * 0.01;
            result.success = (i % 5) != 0; // 80%成功率

            // 添加OCR文本结果
            result.ocrResult.text = QString("Sample OCR text for result %1").arg(i);
            result.ocrResult.confidence = result.confidence;

            results.append(result);
        }

        // 模拟处理结果
        for (AIResult& result : results) {
            result.processingTime += 0.001; // 模拟额外处理时间
        }

        results.clear();
    }
}

void BenchmarkCoreComponents::benchmarkLRUCachePerformance() {
    // 基准测试LRU缓存性能
    const int numOperations = 10000;

    QBENCHMARK {
        // 写入操作
        for (int i = 0; i < numOperations; ++i) {
            QString key = QString("key_%1").arg(i);
            QImage image(640, 480, QImage::Format_RGB32);
            image.fill(QColor(i % 256, (i * 2) % 256, (i * 3) % 256));

            m_imageCache->put(key, image, image.sizeInBytes());
        }

        // 读取操作
        for (int i = 0; i < numOperations; ++i) {
            QString key = QString("key_%1").arg(i % numOperations);
            QImage cachedImage = m_imageCache->get(key);
            Q_UNUSED(cachedImage)
        }

        // 混合操作（80%读，20%写）
        for (int i = 0; i < numOperations * 2; ++i) {
            if (i % 5 < 4) { // 80%读取
                QString key = QString("key_%1").arg(i % numOperations);
                QImage cachedImage = m_imageCache->get(key);
                Q_UNUSED(cachedImage)
            } else { // 20%写入
                QString key = QString("key_%1").arg(numOperations + i);
                QImage image(640, 480, QImage::Format_RGB32);
                image.fill(QColor::fromRgb(i % 256, (i * 2) % 256, (i * 3) % 256));
                m_imageCache->put(key, image, image.sizeInBytes());
            }
        }
    }
}

void BenchmarkCoreComponents::benchmarkMemoryAllocation() {
    // 基准测试内存分配性能
    QBENCHMARK {
        QVector<QImage*> images;

        // 分配大量图像对象
        for (int i = 0; i < 100; ++i) {
            QImage* image = new QImage(1920, 1080, QImage::Format_RGB32);
            image->fill(QColor(i % 256, (i * 2) % 256, (i * 3) % 256));
            images.append(image);
        }

        // 释放内存
        for (QImage* image : images) {
            delete image;
        }

        images.clear();
    }
}

QTEST_MAIN(BenchmarkCoreComponents)
#include "BenchmarkCoreComponents.moc"
