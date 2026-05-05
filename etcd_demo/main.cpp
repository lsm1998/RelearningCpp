#include <iostream>

#include <etcd/KeepAlive.hpp>
#include <etcd/SyncClient.hpp>
#include <etcd/v3/Transaction.hpp>

int main()
{
    etcd::SyncClient etcd("http://localhost:2379");

    std::cout << "=== etcd C++ Client Demo ===" << std::endl;

    etcd.set("/demo/hello", "world");
    std::cout << "PUT  /demo/hello = world" << std::endl;

    etcd.set("/demo/counter", "42");
    std::cout << "PUT  /demo/counter = 42" << std::endl;

    etcd.set("/demo/name", "etcd-cpp-apiv3");
    std::cout << "PUT  /demo/name = etcd-cpp-apiv3" << std::endl;

    auto resp = etcd.get("/demo/hello");
    if (resp.is_ok())
    {
        std::cout << "GET  /demo/hello -> " << resp.value().as_string() << std::endl;
    }
    else
    {
        std::cerr << "GET /demo/hello failed: " << resp.error_message() << std::endl;
    }

    auto items = etcd.ls("/demo/").values();
    std::cout << "\nLS  /demo/ (" << items.size() << " keys):" << std::endl;
    for (auto const &v : items)
    {
        std::cout << "  " << v.key() << " = " << v.as_string() << std::endl;
    }

    int64_t lease_id = etcd.leasegrant(10).value().lease();
    std::cout << "\nLease granted: ID=" << lease_id << " TTL=10s" << std::endl;

    etcd.set("/demo/ephemeral", "I will expire", lease_id);
    std::cout << "PUT  /demo/ephemeral (with lease " << lease_id << ")" << std::endl;

    auto eph = etcd.get("/demo/ephemeral");
    std::cout << "GET  /demo/ephemeral -> " << eph.value().as_string() << std::endl;

    auto keep_alive = etcd.leasekeepalive(lease_id);
    std::cout << "Lease keepalive -> Lease=" << keep_alive->Lease() << std::endl;

    auto del_resp = etcd.rm("/demo/counter");
    std::cout << "\nDELETE /demo/counter (ok=" << del_resp.is_ok() << ")" << std::endl;

    etcd.set("/demo/balance", "100");
    std::cout << "\nPUT  /demo/balance = 100" << std::endl;

    etcdv3::Transaction txn;
    txn.setup_compare_and_swap("/demo/balance", "100", "200");
    auto txn_resp = etcd.txn(txn);
    std::cout << "TXN CAS /demo/balance 100->200: "
              << (txn_resp.is_ok() ? "committed" : "rejected")
              << std::endl;

    auto bal = etcd.get("/demo/balance");
    std::cout << "GET  /demo/balance -> " << bal.value().as_string() << std::endl;

    etcd.rmdir("/demo/", true);
    std::cout << "\n清理完成: DELETE /demo/*" << std::endl;

    std::cout << "\netcd demo 结束" << std::endl;
    return 0;
}
