#pragma once
#include "CheckIn.h"
#include "GoiDichVu.h"
#include "GiaoDich.h"
#include "HoaDon.h"
#include "HoiVien.h"
#include "KiemTraVeSinh.h"
#include "LichHen.h"
#include "PT.h"
#include "ThietBi.h"
#include <filesystem>
#include <map>
#include <memory>
#include <vector>
namespace crm {
class HethongCRM {
    std::map<std::string,std::shared_ptr<HoiVien>> hoiViens_;
    std::map<std::string,std::unique_ptr<PT>> pts_;
    std::vector<LichHen> lichHens_;
    std::vector<CheckIn> checkIns_;
    std::vector<HoaDon> hoaDons_;
    std::vector<GiaoDich> chiPhis_;
    std::vector<ThietBi> thietBis_;
    std::vector<KiemTraVeSinh> kiemTraVeSinhs_;
    std::unique_ptr<ChienLuocGia> chienLuocGia_;
public:
    HethongCRM(); ~HethongCRM();
    HethongCRM(const HethongCRM&) = delete; HethongCRM& operator=(const HethongCRM&) = delete;
    void themHoiVien(std::shared_ptr<HoiVien> hv); void themPT(std::unique_ptr<PT> pt);
    std::shared_ptr<HoiVien> timHoiVien(const std::string& ma) const;
    PT& timPT(const std::string& ma) const;
    double dangKyGoi(const std::string& maHV,GoiTap goi,const Voucher* voucher=nullptr);
    void datLich(LichHen lich); void checkIn(const std::string& maHV);
    void banHang(HoaDon hoaDon);
    void ghiNhanChi(GiaoDich chiPhi);
    void themThietBi(ThietBi thietBi);
    void ghiNhanVeSinh(KiemTraVeSinh bienBan);
    std::vector<std::string> nhacSinhNhat(Date ngay=today()) const;
    std::vector<std::string> nhacGiaHan(int trongSoNgay=7) const;
    std::vector<std::string> nhacLichTap(int trongSoNgay=1) const;
    double tongDoanhThu() const noexcept;
    double tongChi() const noexcept;
    double soDu() const noexcept;
    const auto& hoiViens() const noexcept { return hoiViens_; }
    const auto& pts() const noexcept { return pts_; }
    const auto& lichHens() const noexcept { return lichHens_; }
    const auto& checkIns() const noexcept { return checkIns_; }
    const auto& hoaDons() const noexcept { return hoaDons_; }
    const auto& chiPhis() const noexcept { return chiPhis_; }
    const auto& thietBis() const noexcept { return thietBis_; }
    const auto& kiemTraVeSinhs() const noexcept { return kiemTraVeSinhs_; }
    void luuCSV(const std::filesystem::path& thuMuc) const;
    void docCSV(const std::filesystem::path& thuMuc);
};
}
