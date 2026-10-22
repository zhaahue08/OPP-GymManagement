#include "GoiDichVu.h"
#include "HoiVien.h"
#include <stdexcept>
namespace crm {
GoiDichVu::GoiDichVu(GoiTap loai) : loai_(loai) {
    switch (loai_) {
        case GoiTap::MotNgay: soThang_=0; soBuoi_=1; gia_=36000; break;
        case GoiTap::MotThang: soThang_=1; soBuoi_=0; gia_=339000; break;
        case GoiTap::BaThang: soThang_=3; soBuoi_=0; gia_=899000; break;
        case GoiTap::SauThang: soThang_=6; soBuoi_=0; gia_=1499000; break;
        case GoiTap::MuoiBaThang: soThang_=13; soBuoi_=0; gia_=2999000; break;
        default: throw std::invalid_argument("Goi tap khong hop le.");
    }
}
GoiTap GoiDichVu::loai() const noexcept { return loai_; } int GoiDichVu::soThang() const noexcept { return soThang_; } int GoiDichVu::soBuoi() const noexcept { return soBuoi_; } double GoiDichVu::giaGoc() const noexcept { return gia_; }
double GoiDichVu::tinhPhiDichVu(const HoiVien& hv, const ChienLuocGia& strategy, const Voucher* voucher) const { return strategy.tinhPhiDichVu(hv, loai_, voucher); }
Date GoiDichVu::tinhNgayHetHan(Date batDau) const { if (!batDau.ok()) throw std::invalid_argument("Ngay bat dau khong hop le."); return soThang_ == 0 ? Date{std::chrono::sys_days{batDau}+std::chrono::days{1}} : addMonths(batDau, soThang_); }
double GiaTieuChuan::tinhPhiDichVu(const HoiVien& hv, GoiTap goi, const Voucher* voucher) const {
    const double base = (goi == GoiTap::MotThang && hv.laSinhVien()) ? 240000.0 : GoiDichVu(goi).giaGoc();
    return voucher ? voucher->apDung(base) : base;
}
std::string tenGoi(GoiTap goi) { switch(goi) { case GoiTap::MotNgay:return "1 ngay"; case GoiTap::MotThang:return "1 thang"; case GoiTap::BaThang:return "3 thang"; case GoiTap::SauThang:return "6 thang (5+1)"; case GoiTap::MuoiBaThang:return "13 thang (12+1)"; } return "Khong ro"; }
}
