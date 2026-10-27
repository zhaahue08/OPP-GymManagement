#pragma once
#include <string>
namespace crm {
class DongHang {
public:
    std::string ten;
    int soLuong{};
    double donGia{};
    std::string maSanPham;
    DongHang() = default;
    DongHang(std::string ten,int soLuong,double donGia,std::string maSanPham = {});
    double thanhTien() const noexcept;
};
}
