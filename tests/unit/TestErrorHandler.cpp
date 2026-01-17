#include <QtTest/QtTest>
#include <QSignalSpy>
#include "app/ErrorHandler.h"
#include "common/types/ErrorCodes.h"

class TestErrorHandler : public QObject {
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();

    void testSingleton();
    void testErrorReporting();
    void testErrorRecovery();
    void testDegradedMode();
    void testErrorStatistics();
    void testSignals();

private:
    ErrorHandler* m_errorHandler = nullptr;
};

void TestErrorHandler::initTestCase() {
    m_errorHandler = ErrorHandler::instance();
    QVERIFY(m_errorHandler != nullptr);
}

void TestErrorHandler::cleanupTestCase() {
    // 单例模式，不需要手动删除
}

void TestErrorHandler::testSingleton() {
    // 测试单例模式
    ErrorHandler* instance1 = ErrorHandler::instance();
    ErrorHandler* instance2 = ErrorHandler::instance();

    QVERIFY(instance1 == instance2);
    QVERIFY(instance1 != nullptr);
}

void TestErrorHandler::testErrorReporting() {
    // 测试错误报告
    Error error(ERROR_FILE_NOT_FOUND, ErrorSeverity::Warning,
               ErrorCategory::FileIO, "Test file not found");

    bool result = m_errorHandler->reportError(error);
    QVERIFY(result);

    // 测试简化报告
    result = m_errorHandler->reportError(ERROR_MEMORY_ALLOCATION_FAILED,
                                       ErrorSeverity::Error,
                                       ErrorCategory::Memory,
                                       "Memory allocation failed");
    QVERIFY(result);
}

void TestErrorHandler::testErrorRecovery() {
    // 测试错误恢复策略注册
    bool recoveryCalled = false;

    m_errorHandler->registerRecoveryStrategy(ERROR_NETWORK_CONNECTION_FAILED,
        [&recoveryCalled]() -> bool {
            recoveryCalled = true;
            return true;
        });

    // 触发恢复
    bool recovered = m_errorHandler->tryRecover(Error(ERROR_NETWORK_CONNECTION_FAILED,
                                                     ErrorSeverity::Error,
                                                     ErrorCategory::Network,
                                                     "Network connection failed"));
    QVERIFY(recovered);
    QVERIFY(recoveryCalled);
}

void TestErrorHandler::testDegradedMode() {
    // 测试降级模式
    m_errorHandler->enableDegradedMode(ErrorCategory::AI, 5000); // 5秒

    QVERIFY(m_errorHandler->isDegradedModeEnabled(ErrorCategory::AI));
    QVERIFY(!m_errorHandler->isDegradedModeEnabled(ErrorCategory::Network));

    // 等待降级模式结束
    QTest::qWait(6000);
    QVERIFY(!m_errorHandler->isDegradedModeEnabled(ErrorCategory::AI));
}

void TestErrorHandler::testErrorStatistics() {
    // 清除统计数据
    m_errorHandler->clearStatistics();

    // 报告一些错误
    m_errorHandler->reportError(ERROR_FILE_NOT_FOUND, ErrorSeverity::Warning, ErrorCategory::FileIO, "test1");
    m_errorHandler->reportError(ERROR_MEMORY_ALLOCATION_FAILED, ErrorSeverity::Error, ErrorCategory::Memory, "test2");
    m_errorHandler->reportError(ERROR_NETWORK_CONNECTION_FAILED, ErrorSeverity::Critical, ErrorCategory::Network, "test3");

    // 获取统计数据
    QJsonObject stats = m_errorHandler->getErrorStatistics();

    QVERIFY(stats.contains("totalErrors"));
    QVERIFY(stats.contains("errorsBySeverity"));
    QVERIFY(stats.contains("errorsByCategory"));

    QVERIFY(stats["totalErrors"].toInt() >= 3);
}

void TestErrorHandler::testSignals() {
    // 测试信号发射
    QSignalSpy errorSpy(m_errorHandler, SIGNAL(errorOccurred(const Error&)));
    QSignalSpy recoverySpy(m_errorHandler, SIGNAL(recoveryAttempted(const Error&, bool)));
    QSignalSpy degradedSpy(m_errorHandler, SIGNAL(degradedModeEnabled(ErrorCategory)));

    // 报告错误
    Error error(ERROR_SYSTEM_UNKNOWN, ErrorSeverity::Info, ErrorCategory::System, "test");
    m_errorHandler->reportError(error);

    QVERIFY(errorSpy.count() > 0);

    // 启用降级模式
    m_errorHandler->enableDegradedMode(ErrorCategory::AI, 1000);
    QVERIFY(degradedSpy.count() > 0);
}

QTEST_MAIN(TestErrorHandler)
#include "TestErrorHandler.moc"
