#pragma once
#include "DongHang.h"
#include <iosfwd>
#include <string>
#include <vector>
namespace crm {
class HoaDon {
    std::string ma_, maHoiVien_, ngay_; std::vector<DongHang> dongHang_;
public:
    HoaDon() = default;
    HoaDon(std::string ma, std::string maHoiVien, std::string ngay);
    void themHang(std::string ten, int soLuong, double donGia);
    double tongTien() const noexcept;
    const std::vector<DongHang>& dongHang() const noexcept;
    const std::string& ma() const noexcept; const std::string& maHoiVien() const noexcept; const std::string& ngay() const noexcept;
    friend std::ostream& operator<<(std::ostream& out, const HoaDon& hd);
    friend std::istream& operator>>(std::istream& in, HoaDon& hd);
};
}
