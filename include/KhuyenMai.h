#pragma once
#include "DateUtils.h"
#include "Voucher.h"
#include <string>
namespace crm {
// Chương trình khuyến mãi: bọc một Voucher kèm thời gian, điều kiện và số lượt.
class KhuyenMai {
    std::string ten_;
    Voucher giam_;
    Date tuNgay_{}, denNgay_{};
    std::string maGoi_, maHoiVien_;
    bool tuDong_{}, chiHoiVienMoi_{};
    int soLuotConLai_{-1};
    int daDung_{};
public:
    KhuyenMai(std::string ma, std::string ten, LoaiGiamGia loai, double giaTri, Date tuNgay, Date denNgay,
              bool tuDong, std::string maGoi = {}, int soLuot = -1, std::string maHoiVien = {}, bool chiHoiVienMoi = false);
    const std::string& ma() const noexcept; const std::string& ten() const noexcept;
    const Voucher& voucher() const noexcept;
    Date tuNgay() const noexcept; Date denNgay() const noexcept;
    const std::string& maGoi() const noexcept; const std::string& maHoiVien() const noexcept;
    bool tuDong() const noexcept; bool chiHoiVienMoi() const noexcept;
    int soLuotConLai() const noexcept; int daDung() const noexcept; void napDaDung(int soLan);
    bool conHieuLuc(Date ngay = today()) const noexcept;
    bool apDungDuoc(const std::string& maGoi, const std::string& maHV, bool laHoiVienMoi, Date ngay = today()) const noexcept;
    double apDung(double gia) const;
    void suDung();
    std::string moTa() const;
};
}
