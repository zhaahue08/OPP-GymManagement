#pragma once
#include "DateUtils.h"
#include <array>
#include <map>
#include <string>
#include <vector>
namespace crm {
class HethongCRM;
struct DongTaiChinh {
    std::string ky;
    double doanhThuGoi{}, doanhThuBanLe{}, doanhThuDichVu{}, thuKhac{}, chiPhi{}, congNo{};
    int soHoaDon{};
    double tongThu() const noexcept { return doanhThuGoi + doanhThuBanLe + doanhThuDichVu + thuKhac; }
    double loiNhuan() const noexcept { return tongThu() - chiPhi; }
};
struct DongDangKy { std::string ky; int moi{}, giaHan{}, chuyenNhuong{}; double doanhThu{}; };
struct DongNangSuat { std::string maNhanVien, ten, chucVu; int soHopDong{}, soHoaDonBanLe{}; double doanhSoGoi{}, doanhSoBanLe{}, hoaHong{}; };
struct DongQuayLai { std::string maHoiVien, ten, trangThai, lanCuoi; int soLanDangKy{}, soLuotTap{}, soNgayVang{}; bool quayLai{}; };
struct ThongKeQuayLai {
    int tongHoiVien{}, soQuayLai{}, soHoatDong30Ngay{}, soLauKhongDen{};
    double tyLeQuayLai{};
    std::vector<DongQuayLai> chiTiet;
};
struct DongSanPhamBan { std::string maSanPham, ten, danhMuc; int soLuong{}; double doanhThu{}, giaVon{}; double loiNhuanGop() const noexcept { return doanhThu - giaVon; } };
struct DongDanhGia { std::string ma, ten; int soDanhGia{}; double diemTrungBinh{}; std::array<int,5> soPhieu{}; };

// Các phép thống kê chỉ đọc dữ liệu từ HethongCRM.
class BaoCao {
public:
    static std::vector<DongTaiChinh> taiChinh(const HethongCRM& crm, KyBaoCao ky);
    static std::vector<DongDangKy> dangKy(const HethongCRM& crm, KyBaoCao ky);
    static std::vector<DongNangSuat> nangSuatNhanVien(const HethongCRM& crm, Date tu, Date den);
    static ThongKeQuayLai khachQuayLai(const HethongCRM& crm, int nguongVang = 14);
    static std::array<int,24> luotTheoGio(const HethongCRM& crm, int soNgayGanDay = 30);
    static std::array<int,7> luotTheoThu(const HethongCRM& crm, int soNgayGanDay = 30);
    static std::vector<DongSanPhamBan> banHangTheoSanPham(const HethongCRM& crm, Date tu, Date den);
    static std::map<std::string,double> chiPhiTheoDanhMuc(const HethongCRM& crm, Date tu, Date den);
    static std::vector<DongDanhGia> danhGiaNhanSu(const HethongCRM& crm);
};
}
