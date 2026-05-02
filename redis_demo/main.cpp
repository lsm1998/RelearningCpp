#include <iostream>
#include <string>
#include <vector>

#include <hiredis.h>

const char *HOST = "localhost";
const int PORT = 6379;

void check(redisContext *c, redisReply *r, const char *op)
{
    if (c == nullptr || c->err)
    {
        std::cerr << "连接错误: " << (c ? c->errstr : "null") << std::endl;
        exit(1);
    }
    if (r == nullptr)
    {
        std::cerr << op << " 失败: 返回为空" << std::endl;
        exit(1);
    }
    if (r->type == REDIS_REPLY_ERROR)
    {
        std::cerr << op << " 失败: " << r->str << std::endl;
        exit(1);
    }
}

void demo_string(redisContext *c)
{
    std::cout << "=== String 操作 ===" << std::endl;

    // SET
    auto *r = static_cast<redisReply *>(redisCommand(c, "SET hello world"));
    check(c, r, "SET");
    std::cout << "SET hello world: " << r->str << std::endl;
    freeReplyObject(r);

    // GET
    r = static_cast<redisReply *>(redisCommand(c, "GET hello"));
    check(c, r, "GET");
    std::cout << "GET hello: " << r->str << std::endl;
    freeReplyObject(r);

    // INCR
    r = static_cast<redisReply *>(redisCommand(c, "SET counter 0"));
    check(c, r, "SET counter");
    freeReplyObject(r);
    r = static_cast<redisReply *>(redisCommand(c, "INCR counter"));
    check(c, r, "INCR counter");
    std::cout << "INCR counter: " << r->integer << std::endl;
    freeReplyObject(r);
    r = static_cast<redisReply *>(redisCommand(c, "INCRBY counter 10"));
    check(c, r, "INCRBY counter");
    std::cout << "INCRBY counter 10: " << r->integer << std::endl;
    freeReplyObject(r);

    // EXISTS
    r = static_cast<redisReply *>(redisCommand(c, "EXISTS hello"));
    check(c, r, "EXISTS hello");
    std::cout << "EXISTS hello: " << r->integer << std::endl;
    freeReplyObject(r);

    std::cout << std::endl;
}

void demo_hash(redisContext *c)
{
    std::cout << "=== Hash 操作 ===" << std::endl;

    // HSET
    auto *r = static_cast<redisReply *>(redisCommand(c, "HSET user:1000 name Alice age 25 city Beijing"));
    check(c, r, "HSET");
    std::cout << "HSET user:1000: " << r->integer << " 个字段" << std::endl;
    freeReplyObject(r);

    // HGET
    r = static_cast<redisReply *>(redisCommand(c, "HGET user:1000 name"));
    check(c, r, "HGET");
    std::cout << "HGET user:1000 name: " << r->str << std::endl;
    freeReplyObject(r);

    // HGETALL
    r = static_cast<redisReply *>(redisCommand(c, "HGETALL user:1000"));
    check(c, r, "HGETALL");
    std::cout << "HGETALL user:1000:" << std::endl;
    for (size_t i = 0; i < r->elements; i += 2)
    {
        std::cout << "  " << r->element[i]->str << " = " << r->element[i + 1]->str << std::endl;
    }
    freeReplyObject(r);

    std::cout << std::endl;
}

void demo_list(redisContext *c)
{
    std::cout << "=== List 操作 ===" << std::endl;

    // 清空
    auto *r = static_cast<redisReply *>(redisCommand(c, "DEL mylist"));
    freeReplyObject(r);

    // LPUSH / RPUSH
    r = static_cast<redisReply *>(redisCommand(c, "LPUSH mylist world"));
    check(c, r, "LPUSH");
    freeReplyObject(r);
    r = static_cast<redisReply *>(redisCommand(c, "LPUSH mylist hello"));
    check(c, r, "LPUSH");
    freeReplyObject(r);
    r = static_cast<redisReply *>(redisCommand(c, "RPUSH mylist !"));
    check(c, r, "RPUSH");
    freeReplyObject(r);

    // LLEN
    r = static_cast<redisReply *>(redisCommand(c, "LLEN mylist"));
    check(c, r, "LLEN");
    std::cout << "LLEN mylist: " << r->integer << std::endl;
    freeReplyObject(r);

    // LRANGE
    r = static_cast<redisReply *>(redisCommand(c, "LRANGE mylist 0 -1"));
    check(c, r, "LRANGE");
    std::cout << "LRANGE mylist 0 -1: ";
    for (size_t i = 0; i < r->elements; i++)
    {
        std::cout << r->element[i]->str << " ";
    }
    std::cout << std::endl;
    freeReplyObject(r);

    std::cout << std::endl;
}

void demo_set(redisContext *c)
{
    std::cout << "=== Set 操作 ===" << std::endl;

    auto *r = static_cast<redisReply *>(redisCommand(c, "DEL myset"));
    freeReplyObject(r);

    // SADD
    r = static_cast<redisReply *>(redisCommand(c, "SADD myset a b c d"));
    check(c, r, "SADD");
    std::cout << "SADD myset a b c d: " << r->integer << " 个元素" << std::endl;
    freeReplyObject(r);

    // SCARD
    r = static_cast<redisReply *>(redisCommand(c, "SCARD myset"));
    check(c, r, "SCARD");
    std::cout << "SCARD myset: " << r->integer << std::endl;
    freeReplyObject(r);

    // SMEMBERS
    r = static_cast<redisReply *>(redisCommand(c, "SMEMBERS myset"));
    check(c, r, "SMEMBERS");
    std::cout << "SMEMBERS myset: ";
    for (size_t i = 0; i < r->elements; i++)
    {
        std::cout << r->element[i]->str << " ";
    }
    std::cout << std::endl;
    freeReplyObject(r);

    // SISMEMBER
    r = static_cast<redisReply *>(redisCommand(c, "SISMEMBER myset a"));
    check(c, r, "SISMEMBER");
    std::cout << "SISMEMBER myset a: " << (r->integer ? "是" : "否") << std::endl;
    freeReplyObject(r);

    std::cout << std::endl;
}

void demo_sorted_set(redisContext *c)
{
    std::cout << "=== Sorted Set 操作 ===" << std::endl;

    auto *r = static_cast<redisReply *>(redisCommand(c, "DEL leaderboard"));
    freeReplyObject(r);

    // ZADD
    r = static_cast<redisReply *>(redisCommand(c, "ZADD leaderboard 100 Alice 85 Bob 95 Charlie 70 David"));
    check(c, r, "ZADD");
    std::cout << "ZADD leaderboard: " << r->integer << " 个元素" << std::endl;
    freeReplyObject(r);

    // ZCARD
    r = static_cast<redisReply *>(redisCommand(c, "ZCARD leaderboard"));
    check(c, r, "ZCARD");
    std::cout << "ZCARD leaderboard: " << r->integer << std::endl;
    freeReplyObject(r);

    // ZRANGE ... WITHSCORES (升序)
    r = static_cast<redisReply *>(redisCommand(c, "ZRANGE leaderboard 0 -1 WITHSCORES"));
    check(c, r, "ZRANGE");
    std::cout << "排行榜 (升序):" << std::endl;
    for (size_t i = 0; i < r->elements; i += 2)
    {
        std::cout << "  " << (i / 2 + 1) << ". " << r->element[i]->str
                  << " - " << r->element[i + 1]->str << " 分" << std::endl;
    }
    freeReplyObject(r);

    // ZREVRANGE ... WITHSCORES (降序)
    r = static_cast<redisReply *>(redisCommand(c, "ZREVRANGE leaderboard 0 -1 WITHSCORES"));
    check(c, r, "ZREVRANGE");
    std::cout << "排行榜 (降序):" << std::endl;
    for (size_t i = 0; i < r->elements; i += 2)
    {
        std::cout << "  " << (i / 2 + 1) << ". " << r->element[i]->str
                  << " - " << r->element[i + 1]->str << " 分" << std::endl;
    }
    freeReplyObject(r);

    std::cout << std::endl;
}

int main()
{
    // 连接 Redis
    redisContext *c = redisConnect(HOST, PORT);
    if (c == nullptr || c->err)
    {
        std::cerr << "连接 Redis 失败: " << (c ? c->errstr : "无法分配内存") << std::endl;
        return 1;
    }

    std::cout << "=== 连接 Redis 成功 ===" << std::endl;
    std::cout << "服务器: " << HOST << ":" << PORT << std::endl;

    // PING
    auto *r = static_cast<redisReply *>(redisCommand(c, "PING"));
    check(c, r, "PING");
    std::cout << "PING: " << r->str << std::endl << std::endl;
    freeReplyObject(r);

    demo_string(c);
    demo_hash(c);
    demo_list(c);
    demo_set(c);
    demo_sorted_set(c);

    redisFree(c);
    std::cout << "连接已关闭" << std::endl;
    return 0;
}
