#pragma once
#include "NhanVien.h"
#include <chrono>
#include <string>
namespace crm {
class PT final : public NhanVien {
    std::string chuyenMon_; double doanhThuCaNhan_{}; int soCaDaDay_{}; int soBuoiChamCong_{};
    double donGiaBuoi_{}, giaBuoiChoHoiVien_{};
    int gioBatDau_{6}, gioKetThuc_{21};
public:
    PT(std::string ma, std::string ten, std::string chuyenMon);
    PT(std::string ma, std::string ten, std::string chuyenMon, std::string sdt, double luongCoBan, double donGiaBuoi,
       double tyLeHoaHong, int gioBatDau, int gioKetThuc, double giaBuoiChoHoiVien = 0);
    ~PT() override;
    std::string vaiTro() const override;
    const std::string& chuyenMon() const noexcept;
    void ghiNhanBuoiDay(double doanhThu = 0);
    void chamCong();
    double tinhHoaHong(double tyLe = 0.1) const;
    int soCaDaDay() const noexcept; int soBuoiChamCong() const noexcept;
    double doanhThuCaNhan() const noexcept;
    double donGiaBuoi() const noexcept; double giaBuoiChoHoiVien() const noexcept;
    int gioBatDau() const noexcept; int gioKetThuc() const noexcept;
    void capNhatPT(std::string ten, std::string chuyenMon, std::string sdt, double luongCoBan, double donGiaBuoi, double tyLeHoaHong, int gioBatDau, int gioKetThuc, double giaBuoiChoHoiVien);
    void napThongKe(int soCa, double doanhThu, int soChamCong);
    bool trongCaLam(int phutBatDau, int phutKetThuc) const noexcept;
    double tinhLuong(const KyLuong& ky) const override;
};
}
