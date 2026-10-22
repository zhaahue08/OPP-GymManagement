#pragma once
#include "Nguoi.h"
#include <chrono>
#include <string>
namespace crm {
class PT final : public Nguoi {
    std::string chuyenMon_; double doanhThuCaNhan_{}; int soCaDaDay_{}; int soBuoiChamCong_{};
public:
    PT(std::string ma, std::string ten, std::string chuyenMon);
    ~PT() override;
    std::string vaiTro() const override;
    const std::string& chuyenMon() const noexcept;
    void ghiNhanBuoiDay(double doanhThu = 0);
    void chamCong();
    double tinhHoaHong(double tyLe = 0.1) const;
    int soCaDaDay() const noexcept; int soBuoiChamCong() const noexcept;
};
}