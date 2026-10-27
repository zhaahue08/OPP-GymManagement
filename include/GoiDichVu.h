#pragma once
#include "DateUtils.h"
#include "Voucher.h"
#include <memory>
#include <string>
namespace crm {
class HoiVien;
class GoiDichVu;
enum class GoiTap { MotNgay, MotThang, BaThang, SauThang, MuoiBaThang };
enum class LoaiGoi { TheoNgay, TheoThang, TheoBuoi, KemPT };
std::string tenLoaiGoi(LoaiGoi loai);
LoaiGoi loaiGoiTuTen(const std::string& ten);

// Strategy: cách tính phí có thể thay đổi mà không sửa lớp GoiDichVu.
class ChienLuocGia {
public:
    virtual ~ChienLuocGia() = default;
    virtual std::string ten() const = 0;
    virtual double tinhPhiDichVu(const HoiVien& hv, const GoiDichVu& goi, const Voucher* voucher = nullptr) const = 0;
    double tinhPhiDichVu(const HoiVien& hv, GoiTap goi, const Voucher* voucher = nullptr) const;
};
class GiaTieuChuan final : public ChienLuocGia {
public:
    using ChienLuocGia::tinhPhiDichVu;
    std::string ten() const override;
    double tinhPhiDichVu(const HoiVien& hv, const GoiDichVu& goi, const Voucher* voucher = nullptr) const override;
};
// Giá tiêu chuẩn rồi giảm thêm theo hạng thành viên (Bạc 3%, Vàng 5%, Kim cương 10%).
class GiaTheoHangThanhVien final : public ChienLuocGia {
public:
    using ChienLuocGia::tinhPhiDichVu;
    std::string ten() const override;
    double tinhPhiDichVu(const HoiVien& hv, const GoiDichVu& goi, const Voucher* voucher = nullptr) const override;
    static double tyLeGiamTheoHang(const std::string& hang) noexcept;
};

class GoiDichVu {
    std::string ma_, ten_;
    LoaiGoi kieu_{LoaiGoi::TheoThang};
    GoiTap loai_{GoiTap::MotThang};
    int soThang_{}, soNgay_{}, soBuoi_{}, soBuoiPT_{};
    double gia_{}, giaSV_{};
    bool dangBan_{true};
public:
    explicit GoiDichVu(GoiTap loai);
    GoiDichVu(std::string ma, std::string ten, LoaiGoi kieu, int soThang, int soNgay, int soBuoi, int soBuoiPT, double gia, double giaSV = 0);
    void capNhat(std::string ten, LoaiGoi kieu, int soThang, int soNgay, int soBuoi, int soBuoiPT, double gia, double giaSV);
    GoiTap loai() const noexcept; int soThang() const noexcept; int soBuoi() const noexcept; double giaGoc() const noexcept;
    const std::string& ma() const noexcept; const std::string& ten() const noexcept; LoaiGoi kieu() const noexcept;
    int soNgay() const noexcept; int soBuoiPT() const noexcept; double giaSinhVien() const noexcept;
    bool dangBan() const noexcept; void datDangBan(bool dangBan) noexcept;
    std::string moTaThoiHan() const;
    double tinhPhiDichVu(const HoiVien& hv, const ChienLuocGia& strategy, const Voucher* voucher = nullptr) const;
    Date tinhNgayHetHan(Date batDau) const;
};
std::string tenGoi(GoiTap goi);
std::string maGoiChuan(GoiTap goi);
}
