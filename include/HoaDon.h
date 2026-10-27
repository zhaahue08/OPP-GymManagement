#pragma once
#include "DongHang.h"
#include <iosfwd>
#include <string>
#include <vector>
namespace crm {
// Mã hội viên đặc biệt cho khách vãng lai mua hàng tại quầy.
inline constexpr const char* KHACH_LE = "KHACH_LE";
class HoaDon {
    std::string ma_, maHoiVien_, ngay_; std::vector<DongHang> dongHang_;
    std::string loai_{"Bán lẻ"}, phuongThuc_{"Tiền mặt"}, maNhanVien_, ghiChu_;
    double giamGia_{}, daThanhToan_{};
    bool coCongNo_{};
public:
    HoaDon() = default;
    HoaDon(std::string ma, std::string maHoiVien, std::string ngay);
    void themHang(std::string ten, int soLuong, double donGia, std::string maSanPham = {});
    double tamTinh() const noexcept;
    double tongTien() const noexcept;
    const std::vector<DongHang>& dongHang() const noexcept;
    const std::string& ma() const noexcept; const std::string& maHoiVien() const noexcept; const std::string& ngay() const noexcept;
    const std::string& loai() const noexcept; void datLoai(std::string loai);
    const std::string& phuongThuc() const noexcept;
    const std::string& maNhanVien() const noexcept; void datNhanVien(std::string maNhanVien);
    const std::string& ghiChu() const noexcept; void datGhiChu(std::string ghiChu);
    double giamGia() const noexcept; void datGiamGia(double soTien);
    // Mặc định hóa đơn được coi là trả đủ; gọi ghiNhanThanhToan để ghi công nợ.
    void ghiNhanThanhToan(double daTra, std::string phuongThuc);
    void thuNo(double soTien, std::string phuongThuc = {});
    double daThanhToan() const noexcept; double conNo() const noexcept; bool coCongNo() const noexcept;
    friend std::ostream& operator<<(std::ostream& out, const HoaDon& hd);
    friend std::istream& operator>>(std::istream& in, HoaDon& hd);
};
}
