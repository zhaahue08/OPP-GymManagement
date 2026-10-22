#include "Voucher.h"
#include <algorithm>
#include <stdexcept>
#include <utility>
namespace crm {
Voucher::Voucher(std::string ma, LoaiGiamGia loai, double giaTri) : ma_(std::move(ma)), loai_(loai), giaTri_(giaTri) {
    if (ma_.empty() || giaTri_ <= 0 || (loai_ == LoaiGiamGia::PhanTram && giaTri_ > 100)) throw std::invalid_argument("Voucher khong hop le.");
}
double Voucher::apDung(double giaGoc) const { if (giaGoc < 0) throw std::invalid_argument("Gia goc khong hop le."); return std::max(0.0, giaGoc - (loai_ == LoaiGiamGia::PhanTram ? giaGoc * giaTri_ / 100.0 : giaTri_)); }
const std::string& Voucher::ma() const noexcept { return ma_; }
LoaiGiamGia Voucher::loai() const noexcept { return loai_; }
double Voucher::giaTri() const noexcept { return giaTri_; }
}
