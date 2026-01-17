#include <QtTest/QtTest>
#include <QApplication>
#include <QTimer>
#include "app/Application.h"
#include "app/ErrorHandler.h"
#include "infrastructure/config/ConfigManager.h"
#include "infrastructure/logging/Logger.h"

class TestIntegration : public QObject {
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();

    void testApplicationInitialization();
    void testConfigErrorHandlingIntegration();
    void testLoggingErrorReportingIntegration();
    void testApplicationShutdown();

private:
    QApplication* m_app = nullptr;
    Application* m_application = nullptr;
};

void TestIntegration::initTestCase() {
    // 创建测试应用程序
    int argc = 1;
    char* argv[] = { const_cast<char*>("TestIntegration") };
    m_app = new QApplication(argc, argv);

    // 初始化应用程序
    m_application = new Application();
}

void TestIntegration::cleanupTestCase() {
    // 清理应用程序
    if (m_application) {
        delete m_application;
        m_application = nullptr;
    }

    if (m_app) {
        delete m_app;
        m_app = nullptr;
    }
}

void TestIntegration::testApplicationInitialization() {
    // 测试应用程序初始化
    QVERIFY(m_application != nullptr);

    // 测试配置管理器初始化
    ConfigManager* configManager = ConfigManager::instance();
    QVERIFY(configManager != nullptr);

    // 测试错误处理器初始化
    ErrorHandler* errorHandler = ErrorHandler::instance();
    QVERIFY(errorHandler != nullptr);

    // 测试日志系统初始化
    QVERIFY(Logger::isInitialized());
}

void TestIntegration::testConfigErrorHandlingIntegration() {
    // 测试配置管理器和错误处理器的集成
    ConfigManager* configManager = ConfigManager::instance();
    ErrorHandler* errorHandler = ErrorHandler::instance();

    // 设置一个配置值
    configManager->setValue("test.integration.key", "test_value");

    // 验证配置值
    QVariant value = configManager->getValue("test.integration.key");
    QVERIFY(value.toString() == "test_value");

    // 测试配置保存和加载
    bool saveResult = configManager->save();
    QVERIFY(saveResult);

    // 模拟配置文件损坏的情况
    // 注意：实际测试中可能需要临时修改配置文件
    // 这里我们只测试错误报告机制
    Error testError(2001, ErrorSeverity::Warning, ErrorCategory::FileIO,
                   "Configuration file access test");
    bool reportResult = errorHandler->reportError(testError);
    QVERIFY(reportResult);
}

void TestIntegration::testLoggingErrorReportingIntegration() {
    // 测试日志系统和错误报告的集成
    Logger* logger = Logger::instance();
    ErrorHandler* errorHandler = ErrorHandler::instance();

    QVERIFY(logger != nullptr);
    QVERIFY(errorHandler != nullptr);

    // 测试日志记录
    Logger::info("Integration test: Info message");
    Logger::warning("Integration test: Warning message");
    Logger::error("Integration test: Error message");

    // 测试错误报告到日志
    Error testError(1001, ErrorSeverity::Error, ErrorCategory::System,
                   "Integration test error", "Detailed error description");
    bool reportResult = errorHandler->reportError(testError);
    QVERIFY(reportResult);

    // 验证错误统计
    QJsonObject stats = errorHandler->getErrorStatistics();
    QVERIFY(stats.contains("totalErrors"));
    QVERIFY(stats["totalErrors"].toInt() > 0);
}

void TestIntegration::testApplicationShutdown() {
    // 测试应用程序关闭时的资源清理
    QVERIFY(m_application != nullptr);

    // 模拟应用程序运行
    // 注意：这里我们只测试初始化和清理，不启动完整的事件循环

    // 验证所有单例实例仍然有效
    QVERIFY(ConfigManager::instance() != nullptr);
    QVERIFY(ErrorHandler::instance() != nullptr);
    QVERIFY(Logger::instance() != nullptr);

    // 测试配置保存
    ConfigManager::instance()->setValue("test.shutdown.flag", true);
    bool saveResult = ConfigManager::instance()->save();
    QVERIFY(saveResult);
}

// 异步测试辅助类
class AsyncTestHelper : public QObject {
    Q_OBJECT

public:
    AsyncTestHelper() : m_timer(new QTimer(this)) {
        connect(m_timer, &QTimer::timeout, this, &AsyncTestHelper::onTimeout);
    }

    void startTest(int timeoutMs) {
        m_timer->setSingleShot(true);
        m_timer->start(timeoutMs);
    }

signals:
    void testCompleted();

private slots:
    void onTimeout() {
        emit testCompleted();
    }

private:
    QTimer* m_timer;
};

QTEST_MAIN(TestIntegration)
#include "TestIntegration.moc"
