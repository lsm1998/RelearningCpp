#include <iostream>
#include <pqxx/pqxx>

int main()
{
    try
    {
        pqxx::connection conn("host=localhost port=5432 dbname=postgres user=postgres password=postgres");

        if (!conn.is_open())
        {
            std::cerr << "连接失败" << std::endl;
            return 1;
        }

        std::cout << "=== PostgreSQL 连接成功 ===" << std::endl;
        std::cout << "服务器版本: " << conn.server_version() << std::endl;

        pqxx::work txn(conn);

        // 建表
        txn.exec(R"(
            CREATE TABLE IF NOT EXISTS demo_users (
                id   SERIAL PRIMARY KEY,
                name VARCHAR(100) NOT NULL,
                age  INT NOT NULL
            )
        )");
        std::cout << "表 demo_users 已就绪" << std::endl;

        // 插入数据  ON CONFLICT 需要 PostgreSQL 9.5+
        auto result = txn.exec(R"(
            INSERT INTO demo_users (name, age) VALUES
                ('Alice',   25),
                ('Bob',     30),
                ('Charlie', 35)
            ON CONFLICT DO NOTHING
        )");
        std::cout << "插入 " << result.affected_rows() << " 行数据" << std::endl;

        // 参数化查询
        int min_age = 20;
        pqxx::result rows = txn.exec(
            "SELECT id, name, age FROM demo_users WHERE age > $1 ORDER BY id",
            pqxx::params{min_age});

        std::cout << "\n查询结果 (" << rows.size() << " 行, "
                  << rows.columns() << " 列):" << std::endl;

        // 列名
        for (auto const &col : rows[0])
        {
            std::cout << col.name() << "\t";
        }
        std::cout << std::endl << std::string(40, '-') << std::endl;

        // 数据
        for (auto const &row : rows)
        {
            for (auto const &field : row)
            {
                std::cout << field.c_str() << "\t";
            }
            std::cout << std::endl;
        }

        txn.commit();
    }
    catch (std::exception const &e)
    {
        std::cerr << "错误: " << e.what() << std::endl;
        return 1;
    }

    std::cout << "\n连接已关闭" << std::endl;
    return 0;
}
