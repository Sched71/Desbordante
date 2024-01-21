#include <iostream>

#include <easylogging++.h>

#include "order/order.h"

INITIALIZE_EASYLOGGINGPP

unsigned long long Epic() {
    algos::order::Order order;
    config::InputTable parser = std::make_shared<CSVParser>("input_data/OD/EpicMeds.csv", '|', true);
    order.SetOption("table", std::move(parser));
    order.LoadData();
    unsigned long long time = order.Execute();
    std::cout << "Epic time: " << time << '\n';
    return time;
}

unsigned long long Modis() {
    algos::order::Order order;
    config::InputTable parser = std::make_shared<CSVParser>("input_data/OD/modis_2000-2019_Australia.csv", ',', true);
    order.SetOption("table", std::move(parser));
    order.LoadData();
    unsigned long long time = order.Execute();
    std::cout << "Modis time: " << time << '\n';
    return time;
}

unsigned long long Bay() {
    algos::order::Order order;
    config::InputTable parser = std::make_shared<CSVParser>("input_data/OD/bay_wheels_data_wrangled.csv", ',', true);
    order.SetOption("table", std::move(parser));
    order.LoadData();
    unsigned long long time = order.Execute();
    std::cout << "Bay time: " << time << '\n';
    return time;
}

unsigned long long Legacy() {
    algos::order::Order order;
    config::InputTable parser = std::make_shared<CSVParser>("input_data/OD/LegacyAlt.csv", ',', true);
    order.SetOption("table", std::move(parser));
    order.LoadData();
    unsigned long long time = order.Execute();
    std::cout << "Legacy time: " << time << '\n';
    return time;
}

int main(int argc, char *argv[]) {
    el::Loggers::configureFromGlobal("logging.conf");
    Epic();
    Modis();
    Bay();
    Legacy();
}
