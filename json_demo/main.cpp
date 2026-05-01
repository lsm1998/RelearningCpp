#include <iostream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

int main()
{
    json j;
    j["name"] = "lsm";
    j["age"] = 18;

    std::cout << j.dump(4) << std::endl;
    return 0;
}
