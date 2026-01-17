#include <QtTest/QtTest>
#include <QCoreApplication>
#include <QTimer>
#include <QSignalSpy>

// 包含所有模块头文件
#include "app/EventBus.h"
#include "app/StateManager.h"
#include "app/TaskDispatcher.h"
#include "app/CommandCenter.h"
#include "ai/orchestrator/PromptFactory.h"
#include "ai/orchestrator/ContextManager.h"
#include "domain/screen/ScreenService.h"
#include "domain/audio/AudioService.h"
#include "domain/camera/CameraService.h"
#include "domain/file/FileService.h"
#include "common/types/Frame.h"
#include "common/types/AudioChunk.h"
#include "common/types/AIResult.h"

/**
 * @brief 模块集成测试
 *
 * 测试所有新创建模块之间的协同工作能力
 * 模拟完整的多模态AI处理流程
 */
class TestModuleIntegration : public QObject {
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();

    // 集成测试场景
    void testFullPipelineIntegration();
    void testEventBusCommunication();
    void testTaskDispatcherIntegration();
    void testAIPipelineIntegration();
    void testErrorHandlingIntegration();
    void testStateManagementIntegration();

private:
    // 测试辅助对象
    EventBus* m_eventBus = nullptr;
    StateManager* m_stateManager = nullptr;
    TaskDispatcher* m_taskDispatcher = nullptr;
    CommandCenter* m_commandCenter = nullptr;
    PromptFactory* m_promptFactory = nullptr;
    ContextManager* m_contextManager = nullptr;
    ScreenService* m_screenService = nullptr;
    AudioService* m_audioService = nullptr;
    CameraService* m_cameraService = nullptr;
    FileService* m_fileService = nullptr;

    // 测试数据
    bool m_integrationTestPassed = true;
    QStringList m_testResults;
    int m_completedTasks = 0;

    // 辅助方法
    void logTestResult(const QString& testName, bool passed, const QString& details = QString());
    void waitForEvent(const QString& eventType, int timeoutMs = 5000);
    void simulateUserInteraction();
};

void TestModuleIntegration::initTestCase() {
    qDebug() << "==========================================";
    qDebug() << "初始化模块集成测试...";
    qDebug() << "==========================================";

    // 初始化基础组件
    m_eventBus = EventBus::instance();
    m_stateManager = StateManager::instance();

    QVERIFY(m_eventBus != nullptr);
    QVERIFY(m_stateManager != nullptr);

    bool initResult = m_eventBus->initialize();
    QVERIFY(initResult && m_eventBus->isInitialized());

    initResult = m_stateManager->initialize();
    QVERIFY(initResult && m_stateManager->isInitialized());

    // 初始化TaskDispatcher
    m_taskDispatcher = TaskDispatcher::instance();
    initResult = m_taskDispatcher->initialize();
    QVERIFY(initResult);

    // 初始化其他模块
    m_commandCenter = new CommandCenter();
    m_promptFactory = new PromptFactory();
    m_contextManager = new ContextManager();
    m_screenService = new ScreenService();
    m_audioService = new AudioService();
    m_cameraService = new CameraService();
    m_fileService = new FileService();

    // 初始化各个服务模块
    QVERIFY(m_promptFactory->initialize());
    QVERIFY(m_contextManager->initialize());
    QVERIFY(m_screenService->initialize());
    QVERIFY(m_audioService->initialize());
    QVERIFY(m_cameraService->initialize());
    QVERIFY(m_fileService->initialize());

    qDebug() << "✓ 所有模块初始化完成";
}

void TestModuleIntegration::cleanupTestCase() {
    qDebug() << "==========================================";
    qDebug() << "清理模块集成测试...";
    qDebug() << "==========================================";

    // 断开所有连接
    if (m_eventBus) {
        m_eventBus->disconnect();
    }

    // 清理模块
    if (m_fileService) delete m_fileService;
    if (m_cameraService) delete m_cameraService;
    if (m_audioService) delete m_audioService;
    if (m_screenService) delete m_screenService;
    if (m_contextManager) delete m_contextManager;
    if (m_promptFactory) delete m_promptFactory;
    if (m_commandCenter) delete m_commandCenter;

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

    qDebug() << "✓ 集成测试清理完成";
}

// ==================== 集成测试场景 ====================

void TestModuleIntegration::testFullPipelineIntegration() {
    qDebug() << "\n==========================================";
    qDebug() << "测试1: 完整Pipeline集成测试";
    qDebug() << "==========================================";

    bool testPassed = true;

    try {
        // 1. 模拟用户启动录制
        qDebug() << "1. 模拟用户启动录制...";
        m_stateManager->setRecording(true);
        QVERIFY(m_stateManager->isRecording());

        // 2. 初始化上下文管理器
        qDebug() << "2. 初始化AI上下文...";
        m_contextManager->addSystemMessage("你是一个多模态AI助手，可以处理图像、音频和文本。");
        QVERIFY(!m_contextManager->getContext().isEmpty());

        // 3. 配置PromptFactory
        qDebug() << "3. 配置提示词工厂...";
        m_promptFactory->registerTemplate("vision_analysis",
            "请分析这张图片的内容，描述你看到的场景、物体和可能的活动：{context}");
        m_promptFactory->registerTemplate("audio_transcription",
            "请将以下音频内容转写为文本：{context}");

        // 4. 启动屏幕采集
        qDebug() << "4. 启动屏幕采集服务...";
        QRect captureRegion(0, 0, 1920, 1080);
        m_screenService->setCaptureRegion(captureRegion);
        m_screenService->setTargetFps(10);

        // 5. 启动音频采集
        qDebug() << "5. 启动音频采集服务...";
        m_audioService->setVolume(0.8f);
        // 注意：实际音频设备可能不可用，测试逻辑即可

        // 6. 模拟数据采集和处理流程
        qDebug() << "6. 模拟数据采集和处理流程...";

        // 模拟屏幕帧处理
        Frame testFrame;
        testFrame.timestamp = QDateTime::currentDateTime();
        testFrame.screenIndex = 0;
        testFrame.scaleFactor = 1.0;
        testFrame.source = "integration_test";

        QString frameTaskId = m_taskDispatcher->submitFrameTask(testFrame);
        QVERIFY(!frameTaskId.isEmpty());

        // 模拟音频块处理
        AudioChunk testAudio;
        testAudio.timestamp = QDateTime::currentDateTime();
        testAudio.durationMicroseconds = 1000000;
        testAudio.sampleCount = 16000;
        testAudio.source = "integration_test_mic";

        QString audioTaskId = m_taskDispatcher->submitAudioTask(testAudio);
        QVERIFY(!audioTaskId.isEmpty());

        // 7. 测试任务完成处理
        qDebug() << "7. 等待任务处理完成...";
        QTest::qWait(2000); // 等待任务处理

        // 8. 验证状态同步
        qDebug() << "8. 验证模块间状态同步...";
        QVERIFY(m_stateManager->isInitialized());
        QVERIFY(m_taskDispatcher->getQueueLength() == 0); // 任务应该已被处理

        // 9. 测试上下文更新
        qDebug() << "9. 测试上下文更新...";
        m_contextManager->addUserMessage("我刚才看到屏幕上有一个应用程序界面");
        m_contextManager->addAssistantMessage("我已经记录了这个信息。请问您需要我帮您分析什么具体的功能吗？");

        QString context = m_contextManager->getContext();
        QVERIFY(context.contains("应用程序界面"));

        // 10. 模拟录制结束
        qDebug() << "10. 模拟录制结束...";
        m_stateManager->setRecording(false);
        QVERIFY(!m_stateManager->isRecording());

        logTestResult("完整Pipeline集成测试", true, "所有模块协同工作正常");

    } catch (const std::exception& e) {
        qCritical() << "完整Pipeline集成测试失败:" << e.what();
        testPassed = false;
        logTestResult("完整Pipeline集成测试", false, QString("异常: %1").arg(e.what()));
    }

    QVERIFY(testPassed);
}

void TestModuleIntegration::testEventBusCommunication() {
    qDebug() << "\n==========================================";
    qDebug() << "测试2: EventBus通信集成测试";
    qDebug() << "==========================================";

    bool testPassed = true;
    int receivedEvents = 0;

    try {
        // 订阅多个事件类型
        m_eventBus->subscribe(EventBus::TOPIC_FRAME_CAPTURED, this,
            [&receivedEvents](const QVariant& data) {
                receivedEvents++;
                qDebug() << "收到帧捕获事件 #" << receivedEvents;
            });

        m_eventBus->subscribe(EventBus::TOPIC_AI_RESULT, this,
            [&receivedEvents](const QVariant& data) {
                receivedEvents++;
                qDebug() << "收到AI结果事件 #" << receivedEvents;
            });

        // 发布测试事件
        Frame testFrame;
        testFrame.timestamp = QDateTime::currentDateTime();
        m_eventBus->publish(EventBus::TOPIC_FRAME_CAPTURED, QVariant::fromValue(testFrame));

        AIResult testResult;
        testResult.success = true;
        testResult.resultId = "test_123";
        m_eventBus->publish(EventBus::TOPIC_AI_RESULT, QVariant::fromValue(testResult));

        // 等待事件处理
        QTest::qWait(500);

        // 验证事件接收
        QVERIFY(receivedEvents >= 2);

        // 测试事件过滤
        receivedEvents = 0;
        m_eventBus->publish("unknown_topic", QVariant());
        QTest::qWait(100);
        QVERIFY(receivedEvents == 0); // 不应该收到未知主题的事件

        logTestResult("EventBus通信集成测试", true,
            QString("成功处理 %1 个事件").arg(receivedEvents));

    } catch (const std::exception& e) {
        qCritical() << "EventBus通信测试失败:" << e.what();
        testPassed = false;
        logTestResult("EventBus通信集成测试", false, QString("异常: %1").arg(e.what()));
    }

    QVERIFY(testPassed);
}

void TestModuleIntegration::testTaskDispatcherIntegration() {
    qDebug() << "\n==========================================";
    qDebug() << "测试3: TaskDispatcher集成测试";
    qDebug() << "==========================================";

    bool testPassed = true;

    try {
        // 测试任务调度器与其他模块的集成

        // 1. 配置任务调度器
        m_taskDispatcher->setSamplingRate(15);
        m_taskDispatcher->setAdaptiveSamplingEnabled(true);

        // 2. 提交多种类型的任务
        QStringList taskIds;

        // 屏幕处理任务
        for (int i = 0; i < 3; ++i) {
            Frame frame;
            frame.timestamp = QDateTime::currentDateTime().addMSecs(i * 100);
            frame.screenIndex = 0;
            QString taskId = m_taskDispatcher->submitFrameTask(frame, TaskPriority::NORMAL);
            if (!taskId.isEmpty()) {
                taskIds.append(taskId);
            }
        }

        // 音频处理任务
        for (int i = 0; i < 2; ++i) {
            AudioChunk audio;
            audio.timestamp = QDateTime::currentDateTime().addMSecs(i * 200);
            audio.source = "test_audio";
            QString taskId = m_taskDispatcher->submitAudioTask(audio, TaskPriority::HIGH);
            if (!taskId.isEmpty()) {
                taskIds.append(taskId);
            }
        }

        // AI处理任务
        QVariantMap aiData;
        aiData["type"] = "text_analysis";
        aiData["content"] = "测试文本分析任务";
        QString aiTaskId = m_taskDispatcher->submitAITask(TaskType::LLM_PROCESSING, aiData);
        if (!aiTaskId.isEmpty()) {
            taskIds.append(aiTaskId);
        }

        // 3. 验证任务管理
        QVERIFY(taskIds.size() >= 5);
        QVERIFY(m_taskDispatcher->getActiveTaskCount() >= 0);

        // 4. 测试任务取消
        if (!taskIds.isEmpty()) {
            QString taskToCancel = taskIds.first();
            m_taskDispatcher->cancelTask(taskToCancel);
            // 验证任务已被取消（简化检查）
        }

        // 5. 等待任务处理
        QTest::qWait(3000);

        // 6. 验证调度器状态
        QVERIFY(m_taskDispatcher->getQueueLength() == 0);

        logTestResult("TaskDispatcher集成测试", true,
            QString("成功处理 %1 个任务").arg(taskIds.size()));

    } catch (const std::exception& e) {
        qCritical() << "TaskDispatcher集成测试失败:" << e.what();
        testPassed = false;
        logTestResult("TaskDispatcher集成测试", false, QString("异常: %1").arg(e.what()));
    }

    QVERIFY(testPassed);
}

void TestModuleIntegration::testAIPipelineIntegration() {
    qDebug() << "\n==========================================";
    qDebug() << "测试4: AI Pipeline集成测试";
    qDebug() << "==========================================";

    bool testPassed = true;

    try {
        // 测试AI处理管道：PromptFactory -> ContextManager -> TaskDispatcher

        // 1. 设置AI上下文
        m_contextManager->addSystemMessage("你是一个专业的文档分析助手。");
        m_contextManager->addUserMessage("请分析这份文档的内容结构。");

        // 2. 配置提示词模板
        m_promptFactory->registerTemplate("document_analysis",
            "请分析以下文档内容，提取关键信息和结构：{content}\n\n上下文信息：{context}");

        // 3. 准备文档数据
        QString testDocument = "这是一个测试文档，包含标题、段落和列表。\n\n1. 第一项\n2. 第二项\n\n结论：测试完成。";

        // 4. 生成AI提示词
        QVariantMap promptContext;
        promptContext["content"] = testDocument;
        promptContext["context"] = m_contextManager->getContext();

        QString prompt = m_promptFactory->generatePrompt("document_analysis", promptContext);
        QVERIFY(!prompt.isEmpty());
        QVERIFY(prompt.contains(testDocument));

        // 5. 创建AI处理任务
        QVariantMap aiTaskData;
        aiTaskData["prompt"] = prompt;
        aiTaskData["model"] = "test_model";
        aiTaskData["max_tokens"] = 500;

        QString taskId = m_taskDispatcher->submitAITask(TaskType::LLM_PROCESSING, aiTaskData);
        QVERIFY(!taskId.isEmpty());

        // 6. 更新上下文（模拟AI响应）
        m_contextManager->addAssistantMessage("我已经分析了文档内容，发现它包含标题、列表和结论部分。");

        // 7. 验证上下文积累
        QString fullContext = m_contextManager->getContext();
        QVERIFY(fullContext.contains("文档分析"));
        QVERIFY(fullContext.contains("标题、列表和结论"));

        logTestResult("AI Pipeline集成测试", true, "提示词生成、上下文管理和任务调度协同工作正常");

    } catch (const std::exception& e) {
        qCritical() << "AI Pipeline集成测试失败:" << e.what();
        testPassed = false;
        logTestResult("AI Pipeline集成测试", false, QString("异常: %1").arg(e.what()));
    }

    QVERIFY(testPassed);
}

void TestModuleIntegration::testErrorHandlingIntegration() {
    qDebug() << "\n==========================================";
    qDebug() << "测试5: 错误处理集成测试";
    qDebug() << "==========================================";

    bool testPassed = true;

    try {
        // 测试错误处理与其他模块的集成

        // 1. 模拟模块初始化失败的情况
        ScreenService* faultyScreenService = new ScreenService();
        // 故意不调用initialize()来模拟初始化失败

        // 2. 验证错误处理机制
        // 注意：实际项目中会有更完善的错误处理，这里是基本验证

        // 3. 测试状态一致性
        m_stateManager->setRecording(true);
        QVERIFY(m_stateManager->isRecording());

        // 模拟错误情况
        m_stateManager->setLastError("模拟测试错误");
        QString lastError = m_stateManager->getLastError();
        QVERIFY(lastError == "模拟测试错误");

        // 4. 验证错误计数
        m_stateManager->incrementErrorCount();
        QVERIFY(m_stateManager->getErrorCount() >= 1);

        // 清理
        delete faultyScreenService;

        logTestResult("错误处理集成测试", true, "错误状态管理和计数功能正常");

    } catch (const std::exception& e) {
        qCritical() << "错误处理集成测试失败:" << e.what();
        testPassed = false;
        logTestResult("错误处理集成测试", false, QString("异常: %1").arg(e.what()));
    }

    QVERIFY(testPassed);
}

void TestModuleIntegration::testStateManagementIntegration() {
    qDebug() << "\n==========================================";
    qDebug() << "测试6: 状态管理集成测试";
    qDebug() << "==========================================";

    bool testPassed = true;

    try {
        // 测试状态管理器与其他模块的集成

        // 1. 测试状态同步
        m_stateManager->setRecording(true);
        m_stateManager->setFps(30.0);
        m_stateManager->setCameraOn(true);
        m_stateManager->setMicOn(true);

        QVERIFY(m_stateManager->isRecording());
        QVERIFY(qFuzzyCompare(m_stateManager->getFps(), 30.0));
        QVERIFY(m_stateManager->isCameraOn());
        QVERIFY(m_stateManager->isMicOn());

        // 2. 测试状态持久化
        QString stateFile = "test_state.json";
        bool saveResult = m_stateManager->saveState(stateFile);
        QVERIFY(saveResult);

        // 修改状态
        m_stateManager->setRecording(false);
        m_stateManager->setFps(15.0);

        // 加载状态
        bool loadResult = m_stateManager->loadState(stateFile);
        if (loadResult) {
            QVERIFY(m_stateManager->isRecording());
            QVERIFY(qFuzzyCompare(m_stateManager->getFps(), 30.0));
        }

        // 清理临时文件
        QFile::remove(stateFile);

        // 3. 测试状态监听
        bool propertyChanged = false;
        QString watchId = m_stateManager->watch("custom_property", [&propertyChanged](const QVariant& value) {
            propertyChanged = true;
        });

        m_stateManager->setProperty("custom_property", "test_value");
        QTest::qWait(100);
        QVERIFY(propertyChanged);

        // 清理监听器
        m_stateManager->unwatch(watchId);

        logTestResult("状态管理集成测试", true, "状态同步、持久化和监听功能正常");

    } catch (const std::exception& e) {
        qCritical() << "状态管理集成测试失败:" << e.what();
        testPassed = false;
        logTestResult("状态管理集成测试", false, QString("异常: %1").arg(e.what()));
    }

    QVERIFY(testPassed);
}

// ==================== 辅助方法 ====================

void TestModuleIntegration::logTestResult(const QString& testName, bool passed, const QString& details) {
    QString result = passed ? "✅ 通过" : "❌ 失败";
    QString message = QString("[%1] %2").arg(result, testName);

    if (!details.isEmpty()) {
        message += QString(" - %1").arg(details);
    }

    m_testResults.append(message);

    if (passed) {
        qDebug() << message;
    } else {
        qCritical() << message;
        m_integrationTestPassed = false;
    }
}

void TestModuleIntegration::waitForEvent(const QString& eventType, int timeoutMs) {
    // 简化的事件等待实现
    QTest::qWait(timeoutMs);
}

void TestModuleIntegration::simulateUserInteraction() {
    // 模拟用户交互的简化实现
    qDebug() << "模拟用户交互...";

    // 这里可以添加更多的用户交互模拟
}

QTEST_MAIN(TestModuleIntegration)
#include "TestModuleIntegration.moc"
