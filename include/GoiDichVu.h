#pragma once
#include "DateUtils.h"
#include "Voucher.h"
#include <memory>
#include <string>
namespace crm {
class HoiVien;
enum class GoiTap { MotNgay, MotThang, BaThang, SauThang, MuoiBaThang };
class ChienLuocGia {
public: virtual ~ChienLuocGia() = default; virtual double tinhPhiDichVu(const HoiVien& hv, GoiTap goi, const Voucher* voucher = nullptr) const = 0;
};
class GiaTieuChuan final : public ChienLuocGia {
public: double tinhPhiDichVu(const HoiVien& hv, GoiTap goi, const Voucher* voucher = nullptr) const override;
};
class GoiDichVu {
    GoiTap loai_; int soThang_; int soBuoi_; double gia_;
public:
    explicit GoiDichVu(GoiTap loai);
    GoiTap loai() const noexcept; int soThang() const noexcept; int soBuoi() const noexcept; double giaGoc() const noexcept;
    double tinhPhiDichVu(const HoiVien& hv, const ChienLuocGia& strategy, const Voucher* voucher = nullptr) const;
    Date tinhNgayHetHan(Date batDau) const;
};
std::string tenGoi(GoiTap goi);
}
