#include <iostream>

#include <easylogging++.h>

#include "od/order/order.h"

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

unsigned long long Credit() {
    algos::order::Order order;
    config::InputTable parser = std::make_shared<CSVParser>("input_data/OD/creditcard_2023.csv", ',', true);
    order.SetOption("table", std::move(parser));
    order.LoadData();
    unsigned long long time = order.Execute();
    std::cout << "Credit time: " << time << '\n';
    return time;
}

unsigned long long Ditag() {
    algos::order::Order order;
    config::InputTable parser = std::make_shared<CSVParser>("input_data/OD/DITAG.csv", ';', false);
    order.SetOption("table", std::move(parser));
    order.LoadData();
    unsigned long long time = order.Execute();
    std::cout << "Ditag time: " << time << '\n';
    return time;
}

unsigned long long PFW() {
    algos::order::Order order;
    config::InputTable parser = std::make_shared<CSVParser>("input_data/OD/PFW_2021_public.csv", ',', true);
    order.SetOption("table", std::move(parser));
    order.LoadData();
    unsigned long long time = order.Execute();
    std::cout << "PFW time: " << time << '\n';
    return time;
}

unsigned long long Diabetes() {
    algos::order::Order order;
    config::InputTable parser = std::make_shared<CSVParser>("input_data/OD/diabetes_binary_health_indicators_BRFSS2021.csv", ',', true);
    order.SetOption("table", std::move(parser));
    order.LoadData();
    unsigned long long time = order.Execute();
    std::cout << "Diabetes time: " << time << '\n';
    return time;
}

int main(int argc, char *argv[]) {
    el::Loggers::configureFromGlobal("logging.conf");
    std::vector<unsigned long long> time(8, 0);
    unsigned int iterations_num = 3;
    for (unsigned int i = 0; i < iterations_num; ++i) { 
        time[0] += Diabetes();
        time[1] += PFW();
        time[2] += Ditag();
        time[3] += Credit();
        time[4] += Epic();
        time[5] += Modis();
        time[6] += Bay();
        time[7] += Legacy();
    }
    for (auto& t : time) {
        t /= iterations_num;
        std::cout << t << ",";
    }
    std::cout << '\n';
}
