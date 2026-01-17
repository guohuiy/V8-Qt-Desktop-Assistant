#include <QtTest/QtTest>
#include <QCoreApplication>
#include "common/types/Frame.h"
#include "common/types/AudioChunk.h"
#include "common/types/AIResult.h"
#include "common/types/Command.h"
#include "infrastructure/logging/Logger.h"
#include "infrastructure/config/ConfigManager.h"

class TestCoreComponents : public QObject {
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();

    void testFrame();
    void testAudioChunk();
    void testAIResult();
    void testCommand();
    void testLogger();
    void testConfigManager();

private:
    ConfigManager* m_configManager = nullptr;
};

void TestCoreComponents::initTestCase() {
    // 初始化测试环境
    Logger::initialize("test_logs.txt", LogLevel::Debug);

    // 初始化配置管理器
    m_configManager = new ConfigManager(this);
    m_configManager->initialize("test_config.json");
}

void TestCoreComponents::cleanupTestCase() {
    // 清理测试环境
    if (m_configManager) {
        delete m_configManager;
        m_configManager = nullptr;
    }

    Logger::shutdown();
}

void TestCoreComponents::testFrame() {
    // 测试Frame类
    Frame frame;

    // 测试默认值
    QVERIFY(frame.timestamp.isValid());
    QVERIFY(frame.screenIndex == 0);
    QVERIFY(qFuzzyCompare(frame.scaleFactor, 1.0));
    QVERIFY(frame.source.isEmpty());

    // 测试设置值
    frame.screenIndex = 1;
    frame.scaleFactor = 1.5;
    frame.source = "test";

    QVERIFY(frame.screenIndex == 1);
    QVERIFY(qFuzzyCompare(frame.scaleFactor, 1.5));
    QVERIFY(frame.source == "test");

    Logger::info("Frame test passed");
}

void TestCoreComponents::testAudioChunk() {
    // 测试AudioChunk类
    AudioChunk chunk;

    // 测试默认值
    QVERIFY(chunk.timestamp.isValid());
    QVERIFY(chunk.durationMicroseconds == 0);
    QVERIFY(chunk.sampleCount == 0);
    QVERIFY(chunk.source.isEmpty());

    // 测试设置值
    chunk.durationMicroseconds = 1000000; // 1秒
    chunk.sampleCount = 16000; // 16kHz采样率
    chunk.source = "mic";

    QVERIFY(chunk.durationMicroseconds == 1000000);
    QVERIFY(chunk.sampleCount == 16000);
    QVERIFY(chunk.source == "mic");

    Logger::info("AudioChunk test passed");
}

void TestCoreComponents::testAIResult() {
    // 测试AIResult类
    AIResult result;

    // 测试默认值
    QVERIFY(result.timestamp.isValid());
    QVERIFY(result.processingTime == 0.0);
    QVERIFY(result.confidence == 0.0);
    QVERIFY(result.success == false);

    // 测试设置值
    result.resultId = "test-123";
    result.engineType = "ocr";
    result.modelName = "tesseract";
    result.processingTime = 0.5;
    result.confidence = 0.95;
    result.success = true;

    QVERIFY(result.resultId == "test-123");
    QVERIFY(result.engineType == "ocr");
    QVERIFY(result.modelName == "tesseract");
    QVERIFY(qFuzzyCompare(result.processingTime, 0.5));
    QVERIFY(qFuzzyCompare(result.confidence, 0.95));
    QVERIFY(result.success == true);

    Logger::info("AIResult test passed");
}

void TestCoreComponents::testCommand() {
    // 测试Command类
    Command command;

    // 测试默认值
    QVERIFY(command.timestamp.isValid());
    QVERIFY(command.type == CommandType::Unknown);
    QVERIFY(command.source == CommandSource::Unknown);

    // 测试设置值
    command.commandId = "cmd-123";
    command.type = CommandType::StartRecording;
    command.source = CommandSource::UI;
    command.rawText = "Start recording";

    QVERIFY(command.commandId == "cmd-123");
    QVERIFY(command.type == CommandType::StartRecording);
    QVERIFY(command.source == CommandSource::UI);
    QVERIFY(command.rawText == "Start recording");

    Logger::info("Command test passed");
}

void TestCoreComponents::testLogger() {
    // 测试Logger类
    QVERIFY(Logger::isInitialized());

    // 测试日志级别
    Logger::setLogLevel(LogLevel::Debug);
    QVERIFY(Logger::getLogLevel() == LogLevel::Debug);

    Logger::setLogLevel(LogLevel::Info);
    QVERIFY(Logger::getLogLevel() == LogLevel::Info);

    Logger::info("Logger test passed");
}

void TestCoreComponents::testConfigManager() {
    // 测试ConfigManager类
    QVERIFY(m_configManager != nullptr);
    QVERIFY(m_configManager->isInitialized());

    // 测试配置操作
    m_configManager->setValue("test_key", "test_value");
    QVariant value = m_configManager->getValue("test_key", "default");

    QVERIFY(value.toString() == "test_value");

    // 测试不存在的键
    QVariant defaultValue = m_configManager->getValue("nonexistent_key", "default");
    QVERIFY(defaultValue.toString() == "default");

    Logger::info("ConfigManager test passed");
}

QTEST_MAIN(TestCoreComponents)
#include "TestCoreComponents.moc"
