#pragma once
#include "DateUtils.h"
#include <string>
namespace crm {
// Hàng hóa bán kèm: nước uống, thực phẩm bổ sung, đồ ăn kiêng, phụ kiện...
class SanPham {
    std::string ma_, ten_, danhMuc_, donVi_;
    double giaBan_{}, giaNhap_{};
    int tonKho_{}, mucToiThieu_{};
    bool dangBan_{true};
public:
    SanPham(std::string ma, std::string ten, std::string danhMuc, std::string donVi, double giaBan, double giaNhap, int tonKho = 0, int mucToiThieu = 5);
    void capNhat(std::string ten, std::string danhMuc, std::string donVi, double giaBan, double giaNhap, int mucToiThieu);
    const std::string& ma() const noexcept; const std::string& ten() const noexcept;
    const std::string& danhMuc() const noexcept; const std::string& donVi() const noexcept;
    double giaBan() const noexcept; double giaNhap() const noexcept;
    int tonKho() const noexcept; int mucToiThieu() const noexcept;
    bool dangBan() const noexcept; void datDangBan(bool value) noexcept;
    bool sapHet() const noexcept;
    void nhapKho(int soLuong, double donGiaNhap);
    void xuatKho(int soLuong);
    int dieuChinhTon(int thucTe);
};
class PhieuKho {
    std::string ma_, maSanPham_, loai_, ghiChu_;
    Date ngay_{};
    int soLuong_{};
    double donGia_{};
public:
    PhieuKho(std::string ma, std::string maSanPham, Date ngay, std::string loai, int soLuong, double donGia, std::string ghiChu = {});
    const std::string& ma() const noexcept; const std::string& maSanPham() const noexcept;
    const std::string& loai() const noexcept; const std::string& ghiChu() const noexcept;
    Date ngay() const noexcept; int soLuong() const noexcept; double donGia() const noexcept;
};
}
