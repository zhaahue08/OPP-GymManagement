#include "SanPham.h"
#include <stdexcept>
#include <utility>
namespace crm {
SanPham::SanPham(std::string ma, std::string ten, std::string danhMuc, std::string donVi, double giaBan, double giaNhap, int tonKho, int mucToiThieu)
    : ma_(std::move(ma)), tonKho_(tonKho) {
    if (ma_.empty() || tonKho_ < 0) throw std::invalid_argument("Mã sản phẩm hoặc tồn kho không hợp lệ.");
    capNhat(std::move(ten), std::move(danhMuc), std::move(donVi), giaBan, giaNhap, mucToiThieu);
}
void SanPham::capNhat(std::string ten, std::string danhMuc, std::string donVi, double giaBan, double giaNhap, int mucToiThieu) {
    if (ten.empty() || danhMuc.empty() || giaBan < 0 || giaNhap < 0 || mucToiThieu < 0) throw std::invalid_argument("Thông tin sản phẩm không hợp lệ.");
    ten_ = std::move(ten); danhMuc_ = std::move(danhMuc); donVi_ = donVi.empty() ? "cái" : std::move(donVi);
    giaBan_ = giaBan; giaNhap_ = giaNhap; mucToiThieu_ = mucToiThieu;
}
const std::string& SanPham::ma() const noexcept { return ma_; } const std::string& SanPham::ten() const noexcept { return ten_; }
const std::string& SanPham::danhMuc() const noexcept { return danhMuc_; } const std::string& SanPham::donVi() const noexcept { return donVi_; }
double SanPham::giaBan() const noexcept { return giaBan_; } double SanPham::giaNhap() const noexcept { return giaNhap_; }
int SanPham::tonKho() const noexcept { return tonKho_; } int SanPham::mucToiThieu() const noexcept { return mucToiThieu_; }
bool SanPham::dangBan() const noexcept { return dangBan_; } void SanPham::datDangBan(bool value) noexcept { dangBan_ = value; }
bool SanPham::sapHet() const noexcept { return dangBan_ && tonKho_ <= mucToiThieu_; }
void SanPham::nhapKho(int soLuong, double donGiaNhap) {
    if (soLuong <= 0 || donGiaNhap < 0) throw std::invalid_argument("Số lượng/đơn giá nhập không hợp lệ.");
    // Giá vốn bình quân gia quyền.
    const double tongVon = giaNhap_ * tonKho_ + donGiaNhap * soLuong;
    tonKho_ += soLuong;
    giaNhap_ = tongVon / tonKho_;
}
void SanPham::xuatKho(int soLuong) {
    if (soLuong <= 0) throw std::invalid_argument("Số lượng xuất phải lớn hơn 0.");
    if (soLuong > tonKho_) throw std::logic_error("Không đủ hàng trong kho: " + ten_ + " (còn " + std::to_string(tonKho_) + ").");
    tonKho_ -= soLuong;
}
int SanPham::dieuChinhTon(int thucTe) {
    if (thucTe < 0) throw std::invalid_argument("Số lượng kiểm kê không hợp lệ.");
    const int chenhLech = thucTe - tonKho_;
    tonKho_ = thucTe;
    return chenhLech;
}
PhieuKho::PhieuKho(std::string ma, std::string sp, Date ngay, std::string loai, int sl, double gia, std::string note)
    : ma_(std::move(ma)), maSanPham_(std::move(sp)), loai_(std::move(loai)), ghiChu_(std::move(note)), ngay_(ngay), soLuong_(sl), donGia_(gia) {
    if (ma_.empty() || maSanPham_.empty() || loai_.empty() || !ngay_.ok() || donGia_ < 0) throw std::invalid_argument("Phiếu kho không hợp lệ.");
}
const std::string& PhieuKho::ma() const noexcept { return ma_; } const std::string& PhieuKho::maSanPham() const noexcept { return maSanPham_; }
const std::string& PhieuKho::loai() const noexcept { return loai_; } const std::string& PhieuKho::ghiChu() const noexcept { return ghiChu_; }
Date PhieuKho::ngay() const noexcept { return ngay_; } int PhieuKho::soLuong() const noexcept { return soLuong_; } double PhieuKho::donGia() const noexcept { return donGia_; }
}
