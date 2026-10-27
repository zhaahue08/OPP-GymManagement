#include "NhanVien.h"
#include <algorithm>
#include <stdexcept>
#include <utility>
namespace crm {
NhanVien::NhanVien(std::string ma, std::string ten, std::string chucVu, std::string sdt, double luongCoBan, double tyLeHoaHong, Date ngayVaoLam)
    : Nguoi(std::move(ma), std::move(ten)), chucVu_(std::move(chucVu)), sdt_(std::move(sdt)), luongCoBan_(luongCoBan), tyLeHoaHong_(tyLeHoaHong), ngayVaoLam_(ngayVaoLam) {
    if (chucVu_.empty() || luongCoBan_ < 0 || tyLeHoaHong_ < 0 || tyLeHoaHong_ > 1 || !ngayVaoLam_.ok()) throw std::invalid_argument("Thông tin nhân viên không hợp lệ.");
}
NhanVien::~NhanVien() = default;
std::string NhanVien::vaiTro() const { return chucVu_; }
const std::string& NhanVien::chucVu() const noexcept { return chucVu_; } const std::string& NhanVien::sdt() const noexcept { return sdt_; }
double NhanVien::luongCoBan() const noexcept { return luongCoBan_; } double NhanVien::tyLeHoaHong() const noexcept { return tyLeHoaHong_; }
Date NhanVien::ngayVaoLam() const noexcept { return ngayVaoLam_; } bool NhanVien::dangLam() const noexcept { return dangLam_; }
void NhanVien::capNhat(std::string ten, std::string chucVu, std::string sdt, double luongCoBan, double tyLeHoaHong) {
    if (chucVu.empty() || luongCoBan < 0 || tyLeHoaHong < 0 || tyLeHoaHong > 1) throw std::invalid_argument("Thông tin nhân viên không hợp lệ.");
    doiTen(std::move(ten)); chucVu_ = std::move(chucVu); sdt_ = std::move(sdt); luongCoBan_ = luongCoBan; tyLeHoaHong_ = tyLeHoaHong;
}
void NhanVien::nghiViec() noexcept { dangLam_ = false; } void NhanVien::diLamLai() noexcept { dangLam_ = true; }
double NhanVien::hoaHong(const KyLuong& ky) const noexcept { return ky.doanhSo * tyLeHoaHong_; }
double NhanVien::tinhLuong(const KyLuong& ky) const {
    if (ky.congChuan <= 0 || ky.soCong < 0) throw std::invalid_argument("Kỳ lương không hợp lệ.");
    const double theoCong = luongCoBan_ * std::min(ky.soCong, ky.congChuan) / ky.congChuan;
    return theoCong + hoaHong(ky);
}
}
