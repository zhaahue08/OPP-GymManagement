#include "HethongCRM.h"
#include "MenuConsole.h"
#include <filesystem>
#include <iostream>

int main(){
    crm::HethongCRM system;
    try {
        const std::filesystem::path source=std::filesystem::exists("data/hoi_vien.csv")?"data":"sample_data";
        system.docCSV(source);
    } catch(const std::exception& error) {
        std::cerr<<"Khong nap duoc du lieu: "<<error.what()<<'\n';
    }
    crm::MenuConsole menu(system);
    menu.run();
    return 0;
}
