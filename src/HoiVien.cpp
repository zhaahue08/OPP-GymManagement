#include "HoiVien.h"
#include <stdexcept>
#include <utility>

namespace crm {
HoiVien::HoiVien(std::string ma, std::string ten, std::string sdt, Date ngaySinh, bool sinhVien)
    : Nguoi(std::move(ma), std::move(ten)), sdt_(std::move(sdt)), ngaySinh_(ngaySinh), sinhVien_(sinhVien), trangThai_(taoTrangThaiMoi()) {
    if (sdt_.empty() || !ngaySinh_.ok()) throw std::invalid_argument("Thong tin hoi vien khong hop le.");
}
HoiVien::~HoiVien() = default;
const std::string& HoiVien::ma() const noexcept { return Nguoi::ma(); } const std::string& HoiVien::ten() const noexcept { return Nguoi::ten(); } const std::string& HoiVien::sdt() const noexcept { return sdt_; }
std::string HoiVien::vaiTro() const { return "Hoi vien"; }
bool HoiVien::laSinhVien() const noexcept { return sinhVien_; } int HoiVien::soBuoiConLai() const noexcept { return soBuoiConLai_; } bool HoiVien::gioiHanBuoi() const noexcept { return gioiHanBuoi_; } Date HoiVien::ngayHetHan() const noexcept { return ngayHetHan_; }
Date HoiVien::ngaySinh() const noexcept { return ngaySinh_; }
std::string HoiVien::trangThai() const { return trangThai_->ten(); } const std::vector<std::string>& HoiVien::lichSuCheckIn() const noexcept { return lichSuCheckIn_; }
void HoiVien::napLichSuCheckIn(std::vector<std::string> history) { for(const auto& date:history) (void)parseDate(date); lichSuCheckIn_=std::move(history); }
void HoiVien::datTrangThai(std::unique_ptr<TrangThaiThe> state) { if (!state) throw std::invalid_argument("Trang thai the rong."); trangThai_ = std::move(state); }
void HoiVien::kichHoat(Date hetHan, int soBuoi, bool gioiHanBuoi) { if (!hetHan.ok() || soBuoi < 0) throw std::invalid_argument("Han the/so buoi khong hop le."); trangThai_->kichHoat(*this); ngayHetHan_ = hetHan; soBuoiConLai_ = soBuoi; gioiHanBuoi_ = gioiHanBuoi; }
void HoiVien::dongBang() { trangThai_->dongBang(*this); } void HoiVien::moBang() { trangThai_->moBang(*this); }
void HoiVien::capNhatHetHan(Date ngay) { if (!ngay.ok()) throw std::invalid_argument("Han the khong hop le."); ngayHetHan_ = ngay; }
HoiVien& HoiVien::operator++() {
    if (!trangThai_->duocCheckIn()) throw std::logic_error("The hoi vien khong o trang thai kich hoat.");
    if (ngayHetHan_.ok() && today() >= ngayHetHan_) { trangThai_->danhDauHetHan(*this); throw std::logic_error("The hoi vien da het han."); }
    if (gioiHanBuoi_ && soBuoiConLai_ <= 0) throw std::logic_error("Hoi vien da su dung het so buoi.");
    if (gioiHanBuoi_) --soBuoiConLai_;
    lichSuCheckIn_.push_back(formatDate(today())); return *this;
}
HoiVien HoiVien::operator++(int) {
    HoiVien previous(ma(),ten(),sdt_,ngaySinh_,sinhVien_);
    previous.lichSuCheckIn_=lichSuCheckIn_;
    if(ngayHetHan_.ok()) {
        previous.kichHoat(ngayHetHan_,soBuoiConLai_,gioiHanBuoi_);
        if(trangThai()=="Dong bang") previous.dongBang();
        else if(trangThai()=="Het han") previous.datTrangThai(taoTrangThaiHetHan());
    }
    ++(*this);
    return previous;
}
}
