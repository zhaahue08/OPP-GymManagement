#pragma once
#include "DateUtils.h"
#include "InBody.h"
#include <string>
namespace crm {
// Lịch sử mua/gia hạn/chuyển nhượng gói tập - dùng cho thống kê đăng ký mới, gia hạn và năng suất sale.
class DangKyGoi {
    std::string ma_, maHoiVien_, maGoi_, tenGoi_, loai_, maNhanVien_, maKhuyenMai_, maHoaDon_;
    Date ngay_{}, tuNgay_{}, denNgay_{};
    double giaGoc_{}, phi_{};
public:
    DangKyGoi(std::string ma, std::string maHoiVien, std::string maGoi, std::string tenGoi, std::string loai, Date ngay, Date tuNgay, Date denNgay,
              double giaGoc, double phi, std::string maNhanVien = {}, std::string maKhuyenMai = {}, std::string maHoaDon = {});
    const std::string& ma() const noexcept; const std::string& maHoiVien() const noexcept; const std::string& maGoi() const noexcept;
    const std::string& tenGoi() const noexcept; const std::string& loai() const noexcept; const std::string& maNhanVien() const noexcept;
    const std::string& maKhuyenMai() const noexcept; const std::string& maHoaDon() const noexcept;
    Date ngay() const noexcept; Date tuNgay() const noexcept; Date denNgay() const noexcept;
    double giaGoc() const noexcept; double phi() const noexcept;
};
class LichSuBaoTri {
    std::string maThietBi_, noiDung_, nguoiThucHien_;
    Date ngay_{};
    double chiPhi_{};
public:
    LichSuBaoTri(std::string maThietBi, Date ngay, std::string noiDung, double chiPhi, std::string nguoiThucHien = {});
    const std::string& maThietBi() const noexcept; const std::string& noiDung() const noexcept; const std::string& nguoiThucHien() const noexcept;
    Date ngay() const noexcept; double chiPhi() const noexcept;
};
class ChamCong {
    std::string maNhanVien_;
    DateTime vao_, ra_{};
public:
    ChamCong(std::string maNhanVien, DateTime vao, DateTime ra = {});
    const std::string& maNhanVien() const noexcept;
    DateTime vao() const noexcept; DateTime ra() const noexcept; bool daRa() const noexcept;
    Date ngay() const noexcept;
    double soGio() const;
    void ketThuc(DateTime luc = now());
};
class PhanHoi {
    std::string ma_, maHoiVien_, maDoiTuong_, noiDung_, trangThai_{"Mới"}, xuLy_;
    int diem_{};
    Date ngay_{};
public:
    PhanHoi(std::string ma, std::string maHoiVien, std::string maDoiTuong, int diem, std::string noiDung, Date ngay = today());
    const std::string& ma() const noexcept; const std::string& maHoiVien() const noexcept; const std::string& maDoiTuong() const noexcept;
    const std::string& noiDung() const noexcept; const std::string& trangThai() const noexcept; const std::string& xuLy() const noexcept;
    int diem() const noexcept; Date ngay() const noexcept;
    void danhDauDaXuLy(std::string ketQua);
    void napTrangThai(std::string trangThai, std::string xuLy);
};
class PhieuInBody {
    std::string maHoiVien_;
    Date ngay_{};
    InBody chiSo_;
public:
    PhieuInBody(std::string maHoiVien, Date ngay, InBody chiSo);
    const std::string& maHoiVien() const noexcept; Date ngay() const noexcept; const InBody& chiSo() const noexcept;
};
class NhatKyTap {
    std::string maHoiVien_, maPT_, noiDung_;
    Date ngay_{};
    int thoiLuongPhut_{};
public:
    NhatKyTap(std::string maHoiVien, Date ngay, std::string noiDung, int thoiLuongPhut, std::string maPT = {});
    const std::string& maHoiVien() const noexcept; const std::string& maPT() const noexcept; const std::string& noiDung() const noexcept;
    Date ngay() const noexcept; int thoiLuongPhut() const noexcept;
};
class GhiChuCSKH {
    std::string maHoiVien_, loai_, noiDung_, nguoiGhi_;
    Date ngay_{};
public:
    GhiChuCSKH(std::string maHoiVien, std::string loai, std::string noiDung, Date ngay = today(), std::string nguoiGhi = {});
    const std::string& maHoiVien() const noexcept; const std::string& loai() const noexcept; const std::string& noiDung() const noexcept;
    const std::string& nguoiGhi() const noexcept; Date ngay() const noexcept;
};
}
