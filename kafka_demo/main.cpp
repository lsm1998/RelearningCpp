#include <csignal>
#include <cstring>
#include <iostream>
#include <thread>

#include <rdkafkacpp.h>

static volatile sig_atomic_t g_running = 1;

static void sigterm_handler(int)
{
    g_running = 0;
}

void demo_producer()
{
    RdKafka::Conf *conf = RdKafka::Conf::create(RdKafka::Conf::CONF_GLOBAL);
    std::string errstr;

    // 设置 broker 地址
    conf->set("bootstrap.servers", "localhost:9092", errstr);
    conf->set("client.id", "cpp-producer", errstr);

    // 投递状态回调
    RdKafka::Producer *producer = RdKafka::Producer::create(conf, errstr);
    if (!producer)
    {
        std::cerr << "创建 Producer 失败: " << errstr << std::endl;
        delete conf;
        return;
    }
    delete conf;

    std::cout << "=== Kafka Producer 已创建 ===" << std::endl;
    std::cout << "bootstrap.servers: localhost:9092" << std::endl;
    std::cout << "Producer name: " << producer->name() << std::endl;

    const std::string topic_name = "demo_topic";
    int delivered = 0;

    for (int i = 0; i < 5 && g_running; i++)
    {
        std::string msg = "Hello Kafka #" + std::to_string(i);

        RdKafka::ErrorCode err = producer->produce(
            topic_name,                          // topic
            RdKafka::Topic::PARTITION_UA,        // 任意分区
            RdKafka::Producer::RK_MSG_COPY,      // 拷贝消息内容
            const_cast<char *>(msg.c_str()),      // 消息体
            msg.size(),                           // 消息长度
            nullptr,                              // key
            0,                                    // key 长度
            0,                                    // 时间戳 (0 = 当前时间)
            nullptr                               // 消息头
        );

        if (err == RdKafka::ERR_NO_ERROR)
        {
            std::cout << "  已投递 [" << i << "]: " << msg << std::endl;
        }
        else
        {
            std::cerr << "  投递失败 [" << i << "]: " << RdKafka::err2str(err) << std::endl;

            // 如果是 broker 不可达，不必继续
            if (err == RdKafka::ERR__QUEUE_FULL)
            {
                producer->poll(1000);
                i--; // 重试
                continue;
            }
        }

        producer->poll(0);
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }

    // 等待所有消息发送完毕
    std::cout << "等待消息发送完毕..." << std::endl;
    while (producer->outq_len() > 0 && g_running)
    {
        producer->poll(100);
    }

    delete producer;
    std::cout << "Producer 已关闭" << std::endl;
}

void demo_consumer()
{
    RdKafka::Conf *conf = RdKafka::Conf::create(RdKafka::Conf::CONF_GLOBAL);
    std::string errstr;

    conf->set("bootstrap.servers", "localhost:9092", errstr);
    conf->set("group.id", "cpp-consumer-group", errstr);
    conf->set("auto.offset.reset", "earliest", errstr);

    RdKafka::KafkaConsumer *consumer = RdKafka::KafkaConsumer::create(conf, errstr);
    if (!consumer)
    {
        std::cerr << "创建 Consumer 失败: " << errstr << std::endl;
        delete conf;
        return;
    }
    delete conf;

    std::cout << "\n=== Kafka Consumer 已创建 ===" << std::endl;
    std::cout << "Group: cpp-consumer-group" << std::endl;

    std::vector<std::string> topics = {"demo_topic"};
    RdKafka::ErrorCode err = consumer->subscribe(topics);
    if (err)
    {
        std::cerr << "订阅失败: " << RdKafka::err2str(err) << std::endl;
        delete consumer;
        return;
    }
    std::cout << "已订阅 topic: demo_topic" << std::endl;
    std::cout << "等待消息 (Ctrl+C 退出)..." << std::endl;

    while (g_running)
    {
        RdKafka::Message *msg = consumer->consume(1000);
        switch (msg->err())
        {
        case RdKafka::ERR_NO_ERROR:
            std::cout << "  收到 [" << msg->topic_name() << "] offset=" << msg->offset()
                      << ": " << std::string(static_cast<const char *>(msg->payload()), msg->len())
                      << std::endl;
            break;
        case RdKafka::ERR__TIMED_OUT:
            break; // 超时，继续等待
        case RdKafka::ERR__PARTITION_EOF:
            std::cout << "  已到达分区末尾" << std::endl;
            break;
        default:
            std::cerr << "  消费错误: " << msg->errstr() << std::endl;
            g_running = 0;
            break;
        }
        delete msg;
    }

    consumer->close();
    delete consumer;
    std::cout << "Consumer 已关闭" << std::endl;
}

int main(int argc, char *argv[])
{
    signal(SIGINT, sigterm_handler);
    signal(SIGTERM, sigterm_handler);

    bool producer_mode = true;
    if (argc > 1 && std::strcmp(argv[1], "consumer") == 0)
    {
        producer_mode = false;
    }

    if (producer_mode)
    {
        demo_producer();
    }
    else
    {
        demo_consumer();
    }
    return 0;
}
