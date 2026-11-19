// Điểm bắt đầu của bản Console.
// Luồng chạy đi từ main sang MenuConsole::run, rồi gọi HethongCRM và các lớp nghiệp vụ.
// Các thao tác sửa dữ liệu trong bộ nhớ được ghi ra CSV khi chọn mục 6 của menu chính.

#include "HethongCRM.h"
#include "MenuConsole.h"
#include <filesystem>
#include <iostream>

int main(){
    // Tạo hệ thống trước menu để hệ thống sống suốt thời gian menu sử dụng tham chiếu đến nó.
    crm::HethongCRM system;
    try {
        // Ưu tiên dữ liệu làm việc nếu có hoi_vien.csv, nếu chưa có thì dùng dữ liệu mẫu.
        // Đường dẫn tương đối được tính theo thư mục làm việc lúc chạy Console.
        const std::filesystem::path source=std::filesystem::exists("data/hoi_vien.csv")?"data":"sample_data";
        system.docCSV(source);
    } catch(const std::exception& error) {
        std::cerr<<"Khong nap duoc du lieu: "<<error.what()<<'\n';
    }
    // Menu thao tác trên dữ liệu vừa nạp. Trở về từ run() thì chương trình kết thúc.
    crm::MenuConsole menu(system);
    menu.run();
    return 0;
}
