#include "HoiVien.h"
#include <algorithm>
#include <stdexcept>
#include <utility>

namespace crm {
HoiVien::HoiVien(std::string ma, std::string ten, std::string sdt, Date ngaySinh, bool sinhVien)
    : Nguoi(std::move(ma), std::move(ten)), sdt_(std::move(sdt)), ngaySinh_(ngaySinh), sinhVien_(sinhVien), trangThai_(taoTrangThaiMoi()), ngayThamGia_(today()) {
    if (sdt_.empty() || !ngaySinh_.ok()) throw std::invalid_argument("Thông tin hội viên không hợp lệ (cần số điện thoại và ngày sinh).");
}
HoiVien::~HoiVien() = default;
const std::string& HoiVien::ma() const noexcept { return Nguoi::ma(); } const std::string& HoiVien::ten() const noexcept { return Nguoi::ten(); } const std::string& HoiVien::sdt() const noexcept { return sdt_; }
std::string HoiVien::vaiTro() const { return "Hội viên"; }
bool HoiVien::laSinhVien() const noexcept { return sinhVien_; } int HoiVien::soBuoiConLai() const noexcept { return soBuoiConLai_; } bool HoiVien::gioiHanBuoi() const noexcept { return gioiHanBuoi_; } Date HoiVien::ngayHetHan() const noexcept { return ngayHetHan_; }
Date HoiVien::ngaySinh() const noexcept { return ngaySinh_; }
std::string HoiVien::trangThai() const { return trangThai_->ten(); } const std::vector<std::string>& HoiVien::lichSuCheckIn() const noexcept { return lichSuCheckIn_; }
bool HoiVien::duocCheckIn() const noexcept { return trangThai_->duocCheckIn(); }
void HoiVien::napLichSuCheckIn(std::vector<std::string> history) { for(const auto& date:history) (void)parseDate(date); lichSuCheckIn_=std::move(history); }
void HoiVien::datTrangThai(std::unique_ptr<TrangThaiThe> state) { if (!state) throw std::invalid_argument("Trạng thái thẻ rỗng."); trangThai_ = std::move(state); }
void HoiVien::kichHoat(Date hetHan, int soBuoi, bool gioiHanBuoi) { if (!hetHan.ok() || soBuoi < 0) throw std::invalid_argument("Hạn thẻ/số buổi không hợp lệ."); trangThai_->kichHoat(*this); ngayHetHan_ = hetHan; soBuoiConLai_ = soBuoi; gioiHanBuoi_ = gioiHanBuoi; }
void HoiVien::dongBang() { baoLuu(); }
void HoiVien::moBang() { (void)moBaoLuu(); }
void HoiVien::capNhatHetHan(Date ngay) { if (!ngay.ok()) throw std::invalid_argument("Hạn thẻ không hợp lệ."); ngayHetHan_ = ngay; }
HoiVien& HoiVien::operator++() {
    if (!trangThai_->duocCheckIn()) throw std::logic_error("Thẻ hội viên không ở trạng thái kích hoạt (" + trangThai() + ").");
    // Thẻ còn hiệu lực đến hết ngày hết hạn.
    if (ngayHetHan_.ok() && today() > ngayHetHan_) { trangThai_->danhDauHetHan(*this); throw std::logic_error("Thẻ hội viên đã hết hạn."); }
    if (gioiHanBuoi_ && soBuoiConLai_ <= 0) throw std::logic_error("Hội viên đã sử dụng hết số buổi.");
    if (gioiHanBuoi_) --soBuoiConLai_;
    lichSuCheckIn_.push_back(formatDate(today())); return *this;
}
HoiVien HoiVien::operator++(int) {
    HoiVien previous(ma(),ten(),sdt_,ngaySinh_,sinhVien_);
    previous.lichSuCheckIn_=lichSuCheckIn_;
    previous.gioiTinh_=gioiTinh_; previous.ngheNghiep_=ngheNghiep_; previous.email_=email_; previous.hang_=hang_;
    previous.maThe_=maThe_; previous.loaiThe_=loaiThe_; previous.maPT_=maPT_; previous.goiHienTai_=goiHienTai_; previous.ghiChu_=ghiChu_;
    previous.ngayThamGia_=ngayThamGia_; previous.ngayBaoLuu_=ngayBaoLuu_; previous.soBuoiPT_=soBuoiPT_;
    previous.ngayHetHan_=ngayHetHan_; previous.soBuoiConLai_=soBuoiConLai_; previous.gioiHanBuoi_=gioiHanBuoi_;
    previous.datTrangThai(taoTrangThaiTheoTen(trangThai()));
    ++(*this);
    return previous;
}
void HoiVien::capNhatThongTin(std::string ten, std::string sdt, Date ngaySinh, bool sinhVien, std::string gioiTinh, std::string ngheNghiep, std::string email) {
    if (sdt.empty() || !ngaySinh.ok()) throw std::invalid_argument("Cần số điện thoại và ngày sinh hợp lệ.");
    if (!email.empty() && email.find('@') == std::string::npos) throw std::invalid_argument("Email không hợp lệ.");
    doiTen(std::move(ten)); sdt_ = std::move(sdt); ngaySinh_ = ngaySinh; sinhVien_ = sinhVien;
    gioiTinh_ = std::move(gioiTinh); ngheNghiep_ = std::move(ngheNghiep); email_ = std::move(email);
}
const std::string& HoiVien::gioiTinh() const noexcept { return gioiTinh_; }
const std::string& HoiVien::ngheNghiep() const noexcept { return ngheNghiep_; }
const std::string& HoiVien::email() const noexcept { return email_; }
int HoiVien::tuoi(Date ngay) const { return tinhTuoi(ngaySinh_, ngay); }
const std::string& HoiVien::hang() const noexcept { return hang_; }
void HoiVien::datHang(std::string hang) { if (hang.empty()) throw std::invalid_argument("Hạng thành viên rỗng."); hang_ = std::move(hang); }
const std::string& HoiVien::maThe() const noexcept { return maThe_; }
const std::string& HoiVien::loaiThe() const noexcept { return loaiThe_; }
void HoiVien::ganThe(std::string maThe, std::string loaiThe) { maThe_ = std::move(maThe); loaiThe_ = maThe_.empty() ? std::string{} : std::move(loaiThe); }
const std::string& HoiVien::maPT() const noexcept { return maPT_; }
void HoiVien::ganPT(std::string maPT) { maPT_ = std::move(maPT); }
const std::string& HoiVien::goiHienTai() const noexcept { return goiHienTai_; }
void HoiVien::datGoiHienTai(std::string ten) { goiHienTai_ = std::move(ten); }
const std::string& HoiVien::ghiChu() const noexcept { return ghiChu_; }
void HoiVien::datGhiChu(std::string ghiChu) { ghiChu_ = std::move(ghiChu); }
Date HoiVien::ngayThamGia() const noexcept { return ngayThamGia_; }
void HoiVien::datNgayThamGia(Date ngay) { if (!ngay.ok()) throw std::invalid_argument("Ngày tham gia không hợp lệ."); ngayThamGia_ = ngay; }
Date HoiVien::ngayBaoLuu() const noexcept { return ngayBaoLuu_; }
void HoiVien::baoLuu(Date tuNgay) {
    if (!tuNgay.ok()) throw std::invalid_argument("Ngày bảo lưu không hợp lệ.");
    trangThai_->dongBang(*this);
    ngayBaoLuu_ = tuNgay;
}
int HoiVien::moBaoLuu(Date ngay) {
    trangThai_->moBang(*this);
    int days = 0;
    if (ngayBaoLuu_.ok() && ngay.ok()) days = std::max(0, soNgayGiua(ngayBaoLuu_, ngay));
    if (ngayHetHan_.ok() && days > 0) ngayHetHan_ = addDays(ngayHetHan_, days);
    ngayBaoLuu_ = Date{};
    return days;
}
int HoiVien::soBuoiPT() const noexcept { return soBuoiPT_; }
void HoiVien::napBuoiPT(int soBuoi) { if (soBuoi < 0) throw std::invalid_argument("Số buổi PT không hợp lệ."); soBuoiPT_ += soBuoi; }
void HoiVien::datSoBuoiPT(int soBuoi) { if (soBuoi < 0) throw std::invalid_argument("Số buổi PT không hợp lệ."); soBuoiPT_ = soBuoi; }
void HoiVien::truBuoiPT() { if (soBuoiPT_ <= 0) throw std::logic_error("Hội viên đã hết buổi tập cùng PT."); --soBuoiPT_; }
std::string HoiVien::lanCheckInCuoi() const { return lichSuCheckIn_.empty() ? std::string{} : lichSuCheckIn_.back(); }
int HoiVien::soNgayConLai(Date ngay) const { return ngayHetHan_.ok() ? soNgayGiua(ngay, ngayHetHan_) : 0; }
void HoiVien::ketThucGoi() {
    if (trangThai() == "Bảo lưu") (void)moBaoLuu();
    if (trangThai() == "Kích hoạt") trangThai_->danhDauHetHan(*this);
    ngayHetHan_ = addDays(today(), -1); soBuoiConLai_ = 0; gioiHanBuoi_ = false; soBuoiPT_ = 0; goiHienTai_.clear();
}
}
