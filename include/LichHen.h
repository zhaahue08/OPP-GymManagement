#pragma once
#include <chrono>
#include <string>
namespace crm {
class LichHen {
    std::string ma_, maHoiVien_, maPT_;
    std::chrono::sys_seconds batDau_, ketThuc_;
    std::string trangThai_{"Đã đặt"}, ghiChu_;
public:
    LichHen(std::string ma, std::string maHoiVien, std::string maPT, std::chrono::sys_seconds batDau, std::chrono::sys_seconds ketThuc);
    const std::string& ma() const noexcept; const std::string& maHoiVien() const noexcept; const std::string& maPT() const noexcept;
    std::chrono::sys_seconds batDau() const noexcept; std::chrono::sys_seconds ketThuc() const noexcept;
    bool trungLich(const LichHen& other) const noexcept;
    bool trungLichHoiVien(const LichHen& other) const noexcept;
    int thoiLuongPhut() const noexcept;
    const std::string& trangThai() const noexcept; const std::string& ghiChu() const noexcept;
    bool dangHoatDong() const noexcept;
    void hoanThanh(); void huy(std::string lyDo = {}); void danhDauVang();
    void napTrangThai(std::string trangThai, std::string ghiChu);
};
}
