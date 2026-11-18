#include "KhuyenMai.h"
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <utility>
namespace crm {
KhuyenMai::KhuyenMai(std::string ma, std::string ten, LoaiGiamGia loai, double giaTri, Date tuNgay, Date denNgay,
                     bool tuDong, std::string maGoi, int soLuot, std::string maHoiVien, bool chiHoiVienMoi)
    : ten_(std::move(ten)), giam_(std::move(ma), loai, giaTri), tuNgay_(tuNgay), denNgay_(denNgay), maGoi_(std::move(maGoi)),
      maHoiVien_(std::move(maHoiVien)), tuDong_(tuDong), chiHoiVienMoi_(chiHoiVienMoi), soLuotConLai_(soLuot) {
    if (ten_.empty() || !tuNgay_.ok() || !denNgay_.ok() || denNgay_ < tuNgay_ || soLuot < -1) throw std::invalid_argument("Chương trình khuyến mãi không hợp lệ.");
}
const std::string& KhuyenMai::ma() const noexcept { return giam_.ma(); }
const std::string& KhuyenMai::ten() const noexcept { return ten_; }
const Voucher& KhuyenMai::voucher() const noexcept { return giam_; }
Date KhuyenMai::tuNgay() const noexcept { return tuNgay_; } Date KhuyenMai::denNgay() const noexcept { return denNgay_; }
const std::string& KhuyenMai::maGoi() const noexcept { return maGoi_; } const std::string& KhuyenMai::maHoiVien() const noexcept { return maHoiVien_; }
bool KhuyenMai::tuDong() const noexcept { return tuDong_; } bool KhuyenMai::chiHoiVienMoi() const noexcept { return chiHoiVienMoi_; }
int KhuyenMai::soLuotConLai() const noexcept { return soLuotConLai_; } int KhuyenMai::daDung() const noexcept { return daDung_; }
void KhuyenMai::napDaDung(int soLan) { if (soLan < 0) throw std::invalid_argument("Số lượt đã dùng không hợp lệ."); daDung_ = soLan; }
bool KhuyenMai::conHieuLuc(Date ngay) const noexcept { return ngay.ok() && tuNgay_ <= ngay && ngay <= denNgay_ && soLuotConLai_ != 0; }
bool KhuyenMai::apDungDuoc(const std::string& maGoi, const std::string& maHV, bool laHoiVienMoi, Date ngay) const noexcept {
    if (!conHieuLuc(ngay)) return false;
    if (!maGoi_.empty() && maGoi_ != maGoi) return false;
    if (!maHoiVien_.empty() && maHoiVien_ != maHV) return false;
    return !chiHoiVienMoi_ || laHoiVienMoi;
}
double KhuyenMai::apDung(double gia) const { return giam_.apDung(gia); }
void KhuyenMai::suDung() {
    if (soLuotConLai_ == 0) throw std::logic_error("Khuyến mãi đã hết lượt sử dụng.");
    if (soLuotConLai_ > 0) --soLuotConLai_;
    ++daDung_;
}
std::string KhuyenMai::moTa() const {
    std::ostringstream out;
    out << "Giảm ";
    if (giam_.loai() == LoaiGiamGia::PhanTram) out << giam_.giaTri() << '%';
    else out << std::fixed << std::setprecision(0) << giam_.giaTri() << " VND";
    if (!maGoi_.empty()) out << " cho gói " << maGoi_;
    if (chiHoiVienMoi_) out << ", chỉ hội viên mới";
    if (!maHoiVien_.empty()) out << ", dành riêng " << maHoiVien_;
    return out.str();
}
}
