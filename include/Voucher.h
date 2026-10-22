#pragma once
#include <string>
namespace crm {
enum class LoaiGiamGia { PhanTram, TienMat };
class Voucher {
    std::string ma_; LoaiGiamGia loai_; double giaTri_;
public:
    Voucher(std::string ma, LoaiGiamGia loai, double giaTri);
    double apDung(double giaGoc) const;
    const std::string& ma() const noexcept;
    LoaiGiamGia loai() const noexcept;
    double giaTri() const noexcept;
};
}
