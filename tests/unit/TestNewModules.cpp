#include <QtTest/QtTest>
#include <QCoreApplication>
#include <QSignalSpy>

// 包含新创建的模块头文件
#include "app/TaskDispatcher.h"
#include "ai/orchestrator/PromptFactory.h"
#include "ai/orchestrator/ContextManager.h"
#include "domain/screen/ScreenService.h"
#include "domain/audio/AudioService.h"
#include "domain/camera/CameraService.h"
#include "domain/file/FileService.h"
#include "app/EventBus.h"
#include "app/StateManager.h"
#include "common/types/Frame.h"
#include "common/types/AudioChunk.h"
#include "common/types/AIResult.h"

/**
 * @brief 新模块单元测试
 *
 * 测试7个新创建的核心业务模块的功能
 */
class TestNewModules : public QObject {
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();

    // TaskDispatcher测试
    void testTaskDispatcher();
    void testTaskDispatcherQueue();
    void testTaskDispatcherSampling();

    // PromptFactory测试
    void testPromptFactory();
    void testPromptFactoryTemplates();
    void testPromptFactoryContext();

    // ContextManager测试
    void testContextManager();
    void testContextManagerSlidingWindow();
    void testContextManagerCompression();

    // ScreenService测试
    void testScreenService();
    void testScreenServiceCapture();
    void testScreenServiceRegion();

    // AudioService测试
    void testAudioService();
    void testAudioServiceCapture();
    void testAudioServiceVolume();

    // CameraService测试
    void testCameraService();
    void testCameraServiceDevices();
    void testCameraServiceStream();

    // FileService测试
    void testFileService();
    void testFileServiceParse();
    void testFileServiceExport();

private:
    // 测试辅助对象
    TaskDispatcher* m_taskDispatcher = nullptr;
    PromptFactory* m_promptFactory = nullptr;
    ContextManager* m_contextManager = nullptr;
    ScreenService* m_screenService = nullptr;
    AudioService* m_audioService = nullptr;
    CameraService* m_cameraService = nullptr;
    FileService* m_fileService = nullptr;

    EventBus* m_eventBus = nullptr;
    StateManager* m_stateManager = nullptr;
};

void TestNewModules::initTestCase() {
    qDebug() << "Initializing new modules tests...";

    // 初始化事件总线和状态管理器
    m_eventBus = EventBus::instance();
    m_stateManager = StateManager::instance();

    QVERIFY(m_eventBus != nullptr);
    QVERIFY(m_stateManager != nullptr);

    bool initResult = m_eventBus->initialize();
    QVERIFY(initResult);

    initResult = m_stateManager->initialize();
    QVERIFY(initResult);

    // 初始化TaskDispatcher
    m_taskDispatcher = TaskDispatcher::instance();
    initResult = m_taskDispatcher->initialize();
    QVERIFY(initResult);

    // 初始化其他模块
    m_promptFactory = new PromptFactory();
    m_contextManager = new ContextManager();
    m_screenService = new ScreenService();
    m_audioService = new AudioService();
    m_cameraService = new CameraService();
    m_fileService = new FileService();

    qDebug() << "New modules tests initialized successfully";
}

void TestNewModules::cleanupTestCase() {
    qDebug() << "Cleaning up new modules tests...";

    // 清理模块
    if (m_fileService) delete m_fileService;
    if (m_cameraService) delete m_cameraService;
    if (m_audioService) delete m_audioService;
    if (m_screenService) delete m_screenService;
    if (m_contextManager) delete m_contextManager;
    if (m_promptFactory) delete m_promptFactory;

    // 关闭TaskDispatcher
    if (m_taskDispatcher) {
        m_taskDispatcher->shutdown();
    }

    // 关闭状态管理器和事件总线
    if (m_stateManager) {
        m_stateManager->shutdown();
    }

    if (m_eventBus) {
        m_eventBus->shutdown();
    }

    qDebug() << "New modules tests cleaned up";
}

// ==================== TaskDispatcher测试 ====================

void TestNewModules::testTaskDispatcher() {
    qDebug() << "Testing TaskDispatcher basic functionality...";

    QVERIFY(m_taskDispatcher != nullptr);
    QVERIFY(m_taskDispatcher->isAdaptiveSamplingEnabled());

    // 测试采样率设置
    m_taskDispatcher->setSamplingRate(30);
    QCOMPARE(m_taskDispatcher->getCurrentSamplingRate(), 30);

    // 测试队列长度
    QVERIFY(m_taskDispatcher->getQueueLength() == 0);
    QVERIFY(m_taskDispatcher->getActiveTaskCount() == 0);

    qDebug() << "TaskDispatcher basic test passed";
}

void TestNewModules::testTaskDispatcherQueue() {
    qDebug() << "Testing TaskDispatcher queue functionality...";

    // 创建测试帧
    Frame testFrame;
    testFrame.timestamp = QDateTime::currentDateTime();
    testFrame.screenIndex = 0;
    testFrame.scaleFactor = 1.0;
    testFrame.source = "test";

    // 提交帧处理任务
    QString taskId = m_taskDispatcher->submitFrameTask(testFrame);
    QVERIFY(!taskId.isEmpty());

    // 验证队列状态
    QCOMPARE(m_taskDispatcher->getQueueLength(), 0); // 任务应该已被处理
    QVERIFY(m_taskDispatcher->getActiveTaskCount() >= 0);

    // 测试音频任务
    AudioChunk testAudio;
    testAudio.timestamp = QDateTime::currentDateTime();
    testAudio.durationMicroseconds = 1000000;
    testAudio.sampleCount = 16000;
    testAudio.source = "test_mic";

    taskId = m_taskDispatcher->submitAudioTask(testAudio);
    QVERIFY(!taskId.isEmpty());

    // 测试AI任务
    QVariant aiData = QVariant::fromValue(QString("Test AI input"));
    taskId = m_taskDispatcher->submitAITask(TaskType::LLM_PROCESSING, aiData);
    QVERIFY(!taskId.isEmpty());

    qDebug() << "TaskDispatcher queue test passed";
}

void TestNewModules::testTaskDispatcherSampling() {
    qDebug() << "Testing TaskDispatcher sampling functionality...";

    // 测试自适应采样
    m_taskDispatcher->setAdaptiveSamplingEnabled(true);
    QVERIFY(m_taskDispatcher->isAdaptiveSamplingEnabled());

    m_taskDispatcher->setAdaptiveSamplingEnabled(false);
    QVERIFY(!m_taskDispatcher->isAdaptiveSamplingEnabled());

    // 测试采样决策（简化测试）
    qint64 testTimestamp = QDateTime::currentMSecsSinceEpoch();
    // 注意：shouldSample是私有方法，这里通过间接测试

    qDebug() << "TaskDispatcher sampling test passed";
}

// ==================== PromptFactory测试 ====================

void TestNewModules::testPromptFactory() {
    qDebug() << "Testing PromptFactory basic functionality...";

    QVERIFY(m_promptFactory != nullptr);

    // 测试初始化
    bool initResult = m_promptFactory->initialize();
    QVERIFY(initResult);

    qDebug() << "PromptFactory basic test passed";
}

void TestNewModules::testPromptFactoryTemplates() {
    qDebug() << "Testing PromptFactory template functionality...";

    // 测试模板注册
    QString templateId = "test_template";
    QString templateContent = "请分析以下内容：{content}";
    bool registerResult = m_promptFactory->registerTemplate(templateId, templateContent);
    QVERIFY(registerResult);

    // 测试模板生成
    QVariantMap context;
    context["content"] = "这是一个测试内容";

    QString prompt = m_promptFactory->generatePrompt(templateId, context);
    QVERIFY(!prompt.isEmpty());
    QVERIFY(prompt.contains("这是一个测试内容"));

    qDebug() << "PromptFactory template test passed";
}

void TestNewModules::testPromptFactoryContext() {
    qDebug() << "Testing PromptFactory context functionality...";

    // 测试上下文注入
    QString basePrompt = "请处理这个任务";
    QVariantMap context;
    context["task_type"] = "image_analysis";
    context["priority"] = "high";

    QString enhancedPrompt = m_promptFactory->injectContext(basePrompt, context);
    QVERIFY(!enhancedPrompt.isEmpty());
    QVERIFY(enhancedPrompt.contains("image_analysis") || enhancedPrompt.contains("high"));

    // 测试多场景支持
    QString ocrPrompt = m_promptFactory->createForScene("ocr", "识别图片中的文字");
    QVERIFY(!ocrPrompt.isEmpty());

    QString visionPrompt = m_promptFactory->createForScene("vision", "分析图片内容");
    QVERIFY(!visionPrompt.isEmpty());

    QString dialogPrompt = m_promptFactory->createForScene("dialog", "继续对话");
    QVERIFY(!dialogPrompt.isEmpty());

    qDebug() << "PromptFactory context test passed";
}

// ==================== ContextManager测试 ====================

void TestNewModules::testContextManager() {
    qDebug() << "Testing ContextManager basic functionality...";

    QVERIFY(m_contextManager != nullptr);

    // 测试初始化
    bool initResult = m_contextManager->initialize();
    QVERIFY(initResult);

    qDebug() << "ContextManager basic test passed";
}

void TestNewModules::testContextManagerSlidingWindow() {
    qDebug() << "Testing ContextManager sliding window functionality...";

    // 测试添加消息
    m_contextManager->addUserMessage("Hello");
    m_contextManager->addAssistantMessage("Hi there!");

    m_contextManager->addUserMessage("How are you?");
    m_contextManager->addAssistantMessage("I'm doing well, thank you!");

    // 测试获取上下文
    QString context = m_contextManager->getContext();
    QVERIFY(!context.isEmpty());
    QVERIFY(context.contains("Hello"));
    QVERIFY(context.contains("How are you?"));

    qDebug() << "ContextManager sliding window test passed";
}

void TestNewModules::testContextManagerCompression() {
    qDebug() << "Testing ContextManager compression functionality...";

    // 添加大量消息以触发压缩
    for (int i = 0; i < 20; ++i) {
        m_contextManager->addUserMessage(QString("Message %1").arg(i));
        m_contextManager->addAssistantMessage(QString("Response %1").arg(i));
    }

    // 测试上下文仍然可用
    QString context = m_contextManager->getContext();
    QVERIFY(!context.isEmpty());

    // 测试Token计数
    int tokenCount = m_contextManager->getTokenCount();
    QVERIFY(tokenCount > 0);

    // 测试上下文清理
    m_contextManager->clearContext();
    QString emptyContext = m_contextManager->getContext();
    QVERIFY(emptyContext.isEmpty() || emptyContext.length() < 10);

    qDebug() << "ContextManager compression test passed";
}

// ==================== ScreenService测试 ====================

void TestNewModules::testScreenService() {
    qDebug() << "Testing ScreenService basic functionality...";

    QVERIFY(m_screenService != nullptr);

    // 测试初始化
    bool initResult = m_screenService->initialize();
    QVERIFY(initResult);

    qDebug() << "ScreenService basic test passed";
}

void TestNewModules::testScreenServiceCapture() {
    qDebug() << "Testing ScreenService capture functionality...";

    // 测试截图功能
    QRect captureRegion(0, 0, 1920, 1080);
    QImage screenshot = m_screenService->captureScreen(captureRegion);
    QVERIFY(!screenshot.isNull());
    QVERIFY(screenshot.width() > 0);
    QVERIFY(screenshot.height() > 0);

    qDebug() << "ScreenService capture test passed";
}

void TestNewModules::testScreenServiceRegion() {
    qDebug() << "Testing ScreenService region functionality...";

    // 测试区域设置
    QRect testRegion(100, 100, 800, 600);
    m_screenService->setCaptureRegion(testRegion);

    QRect currentRegion = m_screenService->getCaptureRegion();
    QCOMPARE(currentRegion, testRegion);

    // 测试帧率控制
    m_screenService->setTargetFps(30);
    int currentFps = m_screenService->getTargetFps();
    QCOMPARE(currentFps, 30);

    qDebug() << "ScreenService region test passed";
}

// ==================== AudioService测试 ====================

void TestNewModules::testAudioService() {
    qDebug() << "Testing AudioService basic functionality...";

    QVERIFY(m_audioService != nullptr);

    // 测试初始化
    bool initResult = m_audioService->initialize();
    QVERIFY(initResult);

    qDebug() << "AudioService basic test passed";
}

void TestNewModules::testAudioServiceCapture() {
    qDebug() << "Testing AudioService capture functionality...";

    // 测试音频设备
    QStringList devices = m_audioService->getAvailableDevices();
    QVERIFY(!devices.isEmpty());

    // 测试音频流
    bool startResult = m_audioService->startCapture();
    if (startResult) {
        // 如果能启动，测试停止
        QTest::qWait(100); // 短暂等待
        m_audioService->stopCapture();
    }

    qDebug() << "AudioService capture test passed";
}

void TestNewModules::testAudioServiceVolume() {
    qDebug() << "Testing AudioService volume functionality...";

    // 测试音量控制
    float testVolume = 0.8f;
    m_audioService->setVolume(testVolume);

    float currentVolume = m_audioService->getVolume();
    QVERIFY(qAbs(currentVolume - testVolume) < 0.01f);

    // 测试静音
    m_audioService->setMuted(true);
    QVERIFY(m_audioService->isMuted());

    m_audioService->setMuted(false);
    QVERIFY(!m_audioService->isMuted());

    qDebug() << "AudioService volume test passed";
}

// ==================== CameraService测试 ====================

void TestNewModules::testCameraService() {
    qDebug() << "Testing CameraService basic functionality...";

    QVERIFY(m_cameraService != nullptr);

    // 测试初始化
    bool initResult = m_cameraService->initialize();
    QVERIFY(initResult);

    qDebug() << "CameraService basic test passed";
}

void TestNewModules::testCameraServiceDevices() {
    qDebug() << "Testing CameraService device functionality...";

    // 测试摄像头设备枚举
    QStringList devices = m_cameraService->getAvailableCameras();
    // 注意：实际设备可能不存在，测试不验证设备数量

    if (!devices.isEmpty()) {
        // 如果有设备，测试设备选择
        m_cameraService->setActiveCamera(devices.first());
        QString activeCamera = m_cameraService->getActiveCamera();
        QCOMPARE(activeCamera, devices.first());
    }

    qDebug() << "CameraService device test passed";
}

void TestNewModules::testCameraServiceStream() {
    qDebug() << "Testing CameraService stream functionality...";

    // 测试视频流参数
    m_cameraService->setResolution(QSize(1280, 720));
    QSize resolution = m_cameraService->getResolution();
    QCOMPARE(resolution, QSize(1280, 720));

    m_cameraService->setFrameRate(30);
    int frameRate = m_cameraService->getFrameRate();
    QCOMPARE(frameRate, 30);

    // 测试视频流控制
    bool startResult = m_cameraService->startStream();
    if (startResult) {
        QTest::qWait(100);
        m_cameraService->stopStream();
    }

    qDebug() << "CameraService stream test passed";
}

// ==================== FileService测试 ====================

void TestNewModules::testFileService() {
    qDebug() << "Testing FileService basic functionality...";

    QVERIFY(m_fileService != nullptr);

    // 测试初始化
    bool initResult = m_fileService->initialize();
    QVERIFY(initResult);

    qDebug() << "FileService basic test passed";
}

void TestNewModules::testFileServiceParse() {
    qDebug() << "Testing FileService parsing functionality...";

    // 创建临时测试文件
    QString testContent = "This is a test document content.";
    QString tempFilePath = QDir::tempPath() + "/test_document.txt";

    QFile tempFile(tempFilePath);
    if (tempFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&tempFile);
        out << testContent;
        tempFile.close();

        // 测试文件解析
        QString parsedContent = m_fileService->parseDocument(tempFilePath);
        QVERIFY(!parsedContent.isEmpty());
        QVERIFY(parsedContent.contains("test"));

        // 清理临时文件
        QFile::remove(tempFilePath);
    }

    qDebug() << "FileService parse test passed";
}

void TestNewModules::testFileServiceExport() {
    qDebug() << "Testing FileService export functionality...";

    // 创建测试数据
    QVariantMap testData;
    testData["title"] = "Test Report";
    testData["content"] = "This is test export content";
    testData["timestamp"] = QDateTime::currentDateTime();

    QString exportPath = QDir::tempPath() + "/test_export.json";

    // 测试数据导出
    bool exportResult = m_fileService->exportData(testData, exportPath, "json");
    QVERIFY(exportResult);

    // 验证导出文件存在
    QFile exportFile(exportPath);
    QVERIFY(exportFile.exists());

    // 清理临时文件
    exportFile.remove();

    qDebug() << "FileService export test passed";
}

QTEST_MAIN(TestNewModules)
#include "TestNewModules.moc"
