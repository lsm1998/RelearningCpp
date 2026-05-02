#include <iostream>
#include "common/message.pb.h"
#include "common/enums.pb.h"
#include "payment/payment.pb.h"

int main()
{
    payment::QueryOrderReq req;
    req.set_order_no("123");
    std::cout << req.order_no() << std::endl;
    return 0;
}