#pragma once
#include <chrono>
#include <string>
namespace crm {
class LichHen {
    std::string ma_, maHoiVien_, maPT_;
    std::chrono::sys_seconds batDau_, ketThuc_;
public:
    LichHen(std::string ma, std::string maHoiVien, std::string maPT, std::chrono::sys_seconds batDau, std::chrono::sys_seconds ketThuc);
    const std::string& ma() const noexcept; const std::string& maHoiVien() const noexcept; const std::string& maPT() const noexcept;
    std::chrono::sys_seconds batDau() const noexcept; std::chrono::sys_seconds ketThuc() const noexcept;
    bool trungLich(const LichHen& other) const noexcept;
};
}