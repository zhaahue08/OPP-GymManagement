#include "HethongCRM.h"
#include "DichVuCSKH.h"
#include <algorithm>
#include <cctype>
#include <cmath>
#include <iomanip>
#include <set>
#include <sstream>
#include <stdexcept>

namespace crm {
namespace {
template<class Range, class GetMa>
int soLonNhat(const Range& range, const std::string& prefix, GetMa getMa) {
    int best = 0;
    for (const auto& item : range) {
        const std::string& ma = getMa(item);
        if (ma.size() <= prefix.size() || ma.compare(0, prefix.size(), prefix) != 0) continue;
        const std::string tail = ma.substr(prefix.size());
        if (tail.empty() || !std::all_of(tail.begin(), tail.end(), [](unsigned char c) { return std::isdigit(c); })) continue;
        best = std::max(best, std::stoi(tail));
    }
    return best;
}
std::string tien(double value) { std::ostringstream out; out << std::fixed << std::setprecision(0) << value; return out.str(); }
}

HethongCRM::HethongCRM():chienLuocGia_(std::make_unique<GiaTieuChuan>()){khoiTaoGoiMacDinh();taiKhoans_.emplace_back("admin","admin123",VaiTro::Admin);}
HethongCRM::~HethongCRM()=default;

void HethongCRM::khoiTaoGoiMacDinh() {
    for (auto goi : {GoiTap::MotNgay, GoiTap::MotThang, GoiTap::BaThang, GoiTap::SauThang, GoiTap::MuoiBaThang}) {
        GoiDichVu item(goi); goiDichVus_.emplace(item.ma(), std::move(item));
    }
    goiDichVus_.emplace("GT06", GoiDichVu("GT06", "Gói 12 buổi (trong 60 ngày)", LoaiGoi::TheoBuoi, 0, 60, 12, 0, 450000));
    goiDichVus_.emplace("GT07", GoiDichVu("GT07", "1 tháng + 8 buổi PT", LoaiGoi::KemPT, 1, 0, 0, 8, 2490000));
    goiDichVus_.emplace("GT08", GoiDichVu("GT08", "3 tháng + 24 buổi PT", LoaiGoi::KemPT, 3, 0, 0, 24, 6490000));
}
void HethongCRM::xoaDuLieu() {
    hoiViens_.clear(); pts_.clear(); lichHens_.clear(); checkIns_.clear(); hoaDons_.clear(); chiPhis_.clear(); thietBis_.clear();
    kiemTraVeSinhs_.clear(); nhanViens_.clear(); goiDichVus_.clear(); khuyenMais_.clear(); dangKys_.clear(); thuKhacs_.clear();
    sanPhams_.clear(); phieuKhos_.clear(); baoTris_.clear(); tuDos_.clear(); chamCongs_.clear(); phanHois_.clear(); inBodys_.clear();
    nhatKyTaps_.clear(); ghiChuCSKHs_.clear(); taiKhoans_.clear(); caiDat_.clear(); canhBaoNap_.clear();
    chienLuocGia_ = std::make_unique<GiaTieuChuan>();
}

std::string HethongCRM::sinhMa(const std::string& p) const {
    int n = 0, width = 4;
    auto key = [](const auto& pair) -> const std::string& { return pair.first; };
    auto ma = [](const auto& item) -> const std::string& { return item.ma(); };
    if (p == "HV") { n = soLonNhat(hoiViens_, p, key); width = 3; }
    else if (p == "PT" || p == "NV") { n = std::max(soLonNhat(pts_, p, key), soLonNhat(nhanViens_, p, key)); width = 3; }
    else if (p == "HD") n = soLonNhat(hoaDons_, p, ma);
    else if (p == "CP") n = soLonNhat(chiPhis_, p, ma);
    else if (p == "TK") n = soLonNhat(thuKhacs_, p, ma);
    else if (p == "LH") n = soLonNhat(lichHens_, p, ma);
    else if (p == "DK") n = soLonNhat(dangKys_, p, ma);
    else if (p == "SP") { n = soLonNhat(sanPhams_, p, key); width = 3; }
    else if (p == "PK") n = soLonNhat(phieuKhos_, p, ma);
    else if (p == "TB") { n = soLonNhat(thietBis_, p, ma); width = 3; }
    else if (p == "PH") n = soLonNhat(phanHois_, p, ma);
    else if (p == "KM") n = soLonNhat(khuyenMais_, p, ma);
    else if (p == "GT") { n = soLonNhat(goiDichVus_, p, key); width = 2; }
    else throw std::invalid_argument("Tiền tố mã không hỗ trợ: " + p);
    std::ostringstream out; out << p << std::setw(width) << std::setfill('0') << n + 1;
    return out.str();
}

// ===== Hội viên =====
void HethongCRM::themHoiVien(std::shared_ptr<HoiVien> hv){if(!hv)throw std::invalid_argument("Hội viên rỗng.");const auto id=hv->ma();if(id==KHACH_LE)throw std::invalid_argument("Mã hội viên trùng mã dành riêng.");if(!hv->maThe().empty()&&timHoiVienTheoThe(hv->maThe()))throw std::invalid_argument("Mã thẻ đã được gán cho hội viên khác.");if(!hoiViens_.emplace(id,std::move(hv)).second)throw std::invalid_argument("Mã hội viên đã tồn tại.");}
void HethongCRM::themPT(std::unique_ptr<PT> pt){if(!pt)throw std::invalid_argument("PT rỗng.");auto id=pt->ma();if(nhanViens_.contains(id)||!pts_.emplace(id,std::move(pt)).second)throw std::invalid_argument("Mã PT/nhân viên đã tồn tại.");}
std::shared_ptr<HoiVien> HethongCRM::timHoiVien(const std::string& id)const{auto it=hoiViens_.find(id);if(it==hoiViens_.end())throw std::out_of_range("Không tìm thấy hội viên "+id+".");return it->second;}
std::shared_ptr<HoiVien> HethongCRM::timHoiVienTheoThe(const std::string& the)const{if(the.empty())return nullptr;for(const auto& [id,hv]:hoiViens_)if(hv->maThe()==the)return hv;return nullptr;}
PT& HethongCRM::timPT(const std::string& id)const{auto it=pts_.find(id);if(it==pts_.end())throw std::out_of_range("Không tìm thấy PT "+id+".");return *it->second;}
void HethongCRM::xoaHoiVien(const std::string& ma) {
    (void)timHoiVien(ma);
    if (dangTrongPhong(ma)) throw std::logic_error("Hội viên đang trong phòng tập, hãy check-out trước.");
    for (auto& tu : tuDos_) if (tu.maHoiVien() == ma && (tu.trangThai() == "Đang dùng" || tu.trangThai() == "Thuê tháng")) tu.traTu();
    for (auto& lich : lichHens_) if (lich.maHoiVien() == ma && lich.trangThai() == "Đã đặt") lich.huy("Hội viên bị xóa");
    hoiViens_.erase(ma);
}
void HethongCRM::ganThe(const std::string& maHV, const std::string& maThe, const std::string& loaiThe) {
    auto hv = timHoiVien(maHV);
    if (auto other = timHoiVienTheoThe(maThe); other && other->ma() != maHV) throw std::invalid_argument("Mã thẻ đã được gán cho " + other->ma() + ".");
    hv->ganThe(maThe, loaiThe);
}
void HethongCRM::phanCongPT(const std::string& maHV, const std::string& maPT) {
    auto hv = timHoiVien(maHV);
    if (!maPT.empty() && !timPT(maPT).dangLam()) throw std::logic_error("PT đã nghỉ việc.");
    hv->ganPT(maPT);
}
void HethongCRM::baoLuu(const std::string& maHV) {
    auto hv = timHoiVien(maHV);
    if (dangTrongPhong(maHV)) throw std::logic_error("Hội viên đang trong phòng tập.");
    hv->baoLuu();
    for (auto& lich : lichHens_) if (lich.maHoiVien() == maHV && lich.trangThai() == "Đã đặt" && lich.batDau() > now()) lich.huy("Bảo lưu thẻ");
}
int HethongCRM::moBaoLuu(const std::string& maHV) { return timHoiVien(maHV)->moBaoLuu(); }
bool HethongCRM::laHoiVienMoi(const std::string& maHV) const {
    return std::none_of(dangKys_.begin(), dangKys_.end(), [&](const DangKyGoi& d) { return d.maHoiVien() == maHV; });
}
void HethongCRM::chuyenNhuong(const std::string& tuMa, const std::string& denMa, double phi, const std::string& maNV) {
    if (tuMa == denMa) throw std::invalid_argument("Không thể chuyển nhượng cho chính mình.");
    auto from = timHoiVien(tuMa); auto to = timHoiVien(denMa);
    if (from->trangThai() != "Kích hoạt" || from->soNgayConLai() < 0) throw std::logic_error("Chỉ thẻ đang kích hoạt và còn hạn mới được chuyển nhượng.");
    if (to->trangThai() == "Kích hoạt" || to->trangThai() == "Bảo lưu") throw std::logic_error("Người nhận đang có gói tập còn hiệu lực.");
    if (phi < 0) throw std::invalid_argument("Phí chuyển nhượng không hợp lệ.");
    if (dangTrongPhong(tuMa)) throw std::logic_error("Người chuyển đang trong phòng tập.");
    const Date han = from->ngayHetHan();
    to->kichHoat(han, from->soBuoiConLai(), from->gioiHanBuoi());
    to->datSoBuoiPT(from->soBuoiPT());
    to->datGoiHienTai(from->goiHienTai());
    std::string maHD;
    if (phi > 0) {
        HoaDon hd(sinhMa("HD"), tuMa, formatDate(today()));
        hd.themHang("Phí chuyển nhượng gói cho " + denMa, 1, phi);
        hd.datLoai("Dịch vụ"); hd.datNhanVien(maNV);
        maHD = hd.ma(); themHoaDon(std::move(hd));
    }
    dangKys_.emplace_back(sinhMa("DK"), denMa, "", from->goiHienTai().empty() ? "Gói chuyển nhượng" : from->goiHienTai(), "Chuyển nhượng",
                          today(), today(), han, 0, phi, maNV, "", maHD);
    from->ketThucGoi();
    for (auto& lich : lichHens_) if (lich.maHoiVien() == tuMa && lich.trangThai() == "Đã đặt") lich.huy("Đã chuyển nhượng gói");
}
double HethongCRM::tongChiTieu(const std::string& maHV) const {
    double sum = 0; for (const auto& hd : hoaDons_) if (hd.maHoiVien() == maHV) sum += hd.tongTien(); return sum;
}
std::string HethongCRM::capNhatHang(const std::string& maHV) {
    auto hv = timHoiVien(maHV);
    const double spend = tongChiTieu(maHV);
    const std::vector<std::pair<double, std::string>> bac{{10000000, "Kim cương"}, {5000000, "Vàng"}, {2000000, "Bạc"}};
    auto rank = [](const std::string& h) { return h == "Kim cương" ? 3 : h == "Vàng" ? 2 : h == "Bạc" ? 1 : 0; };
    for (const auto& [nguong, ten] : bac)
        if (spend >= nguong) { if (rank(ten) > rank(hv->hang())) hv->datHang(ten); break; }
    return hv->hang();
}
int HethongCRM::capNhatTrangThaiHetHan() {
    int count = 0;
    for (auto& [id, hv] : hoiViens_) {
        (void)id;
        if (hv->trangThai() == "Kích hoạt" && hv->ngayHetHan().ok() && today() > hv->ngayHetHan()) { hv->datTrangThai(taoTrangThaiHetHan()); ++count; }
    }
    for (auto& tu : tuDos_) if (tu.trangThai() == "Thuê tháng" && tu.hetHanThue().ok() && today() > tu.hetHanThue()) tu.traTu();
    return count;
}
void HethongCRM::ghiInBody(const std::string& maHV, InBody chiSo, Date ngay) { (void)timHoiVien(maHV); inBodys_.emplace_back(maHV, ngay, chiSo); }
void HethongCRM::ghiNhatKyTap(NhatKyTap nk) { (void)timHoiVien(nk.maHoiVien()); if (!nk.maPT().empty()) (void)timPT(nk.maPT()); nhatKyTaps_.push_back(std::move(nk)); }

// ===== Gói tập & khuyến mãi =====
KetQuaDangKy HethongCRM::tinhPhi(const HoiVien& hv, const GoiDichVu& goi, const std::string& maKM, const KhuyenMai** kmApDung) const {
    if (!goi.dangBan()) throw std::logic_error("Gói " + goi.ten() + " đã ngừng bán.");
    const bool moi = laHoiVienMoi(hv.ma());
    const KhuyenMai* km = nullptr;
    double phi = goi.tinhPhiDichVu(hv, *chienLuocGia_);
    if (!maKM.empty()) {
        km = timKhuyenMai(maKM);
        if (!km) throw std::invalid_argument("Mã khuyến mãi " + maKM + " không tồn tại.");
        if (!km->apDungDuoc(goi.ma(), hv.ma(), moi)) throw std::logic_error("Mã " + maKM + " không áp dụng được (hết hạn, hết lượt hoặc sai điều kiện).");
        phi = goi.tinhPhiDichVu(hv, *chienLuocGia_, &km->voucher());
    } else {
        // Tự động chọn khuyến mãi tốt nhất đang chạy.
        for (const auto& item : khuyenMais_) {
            if (!item.tuDong() || !item.apDungDuoc(goi.ma(), hv.ma(), moi)) continue;
            const double gia = goi.tinhPhiDichVu(hv, *chienLuocGia_, &item.voucher());
            if (gia < phi) { phi = gia; km = &item; }
        }
    }
    KetQuaDangKy kq;
    kq.giaGoc = goi.giaGoc(); kq.phi = phi; kq.loai = moi ? "Đăng ký mới" : "Gia hạn";
    if (km) { kq.maKhuyenMai = km->ma(); kq.tenKhuyenMai = km->ten(); }
    Date start = today();
    if (hv.trangThai() == "Kích hoạt" && hv.ngayHetHan().ok() && hv.ngayHetHan() >= today()) start = addDays(hv.ngayHetHan(), 1);
    kq.tuNgay = start; kq.denNgay = goi.tinhNgayHetHan(start);
    if (kmApDung) *kmApDung = km;
    return kq;
}
KetQuaDangKy HethongCRM::thucHienDangKy(HoiVien& hv, const GoiDichVu& goi, const Voucher* voucher, const std::string& maKM,
                                        const std::string& maNV, double daTra, const std::string& phuongThuc) {
    if (hv.trangThai() == "Bảo lưu") throw std::logic_error("Thẻ đang bảo lưu, hãy mở bảo lưu trước khi gia hạn.");
    const KhuyenMai* km = nullptr;
    KetQuaDangKy kq = tinhPhi(hv, goi, voucher ? std::string{} : maKM, voucher ? nullptr : &km);
    if (voucher) { kq.phi = goi.tinhPhiDichVu(hv, *chienLuocGia_, voucher); kq.maKhuyenMai = voucher->ma(); kq.tenKhuyenMai = "Voucher nhập tay"; }
    if (daTra > kq.phi + 0.5) throw std::invalid_argument("Số tiền trả lớn hơn phí gói.");
    int sessions = goi.soBuoi();
    const bool limited = sessions > 0;
    if (limited && hv.gioiHanBuoi() && hv.trangThai() == "Kích hoạt") sessions += hv.soBuoiConLai();
    HoaDon invoice(sinhMa("HD"), hv.ma(), formatDate(today()));
    invoice.themHang(goi.ten().rfind("Gói", 0) == 0 ? goi.ten() : "Gói tập " + goi.ten(), 1, kq.phi);
    invoice.datLoai("Gói tập"); invoice.datNhanVien(maNV);
    if (!kq.maKhuyenMai.empty()) invoice.datGhiChu("Khuyến mãi " + kq.maKhuyenMai + " (giá gốc " + tien(kq.giaGoc) + ")");
    invoice.ghiNhanThanhToan(daTra < 0 ? invoice.tongTien() : daTra, phuongThuc);
    // Mọi kiểm tra đã xong, bắt đầu ghi thay đổi.
    hv.kichHoat(kq.denNgay, sessions, limited);
    hv.napBuoiPT(goi.soBuoiPT());
    hv.datGoiHienTai(goi.ten());
    kq.maHoaDon = invoice.ma();
    themHoaDon(std::move(invoice));
    if (km) if (auto* sua = timKhuyenMaiSua(km->ma())) sua->suDung();
    dangKys_.emplace_back(sinhMa("DK"), hv.ma(), goi.ma(), goi.ten(), kq.loai == "Đăng ký mới" ? "Mới" : "Gia hạn", today(), kq.tuNgay, kq.denNgay,
                          kq.giaGoc, kq.phi, maNV, kq.maKhuyenMai, kq.maHoaDon);
    capNhatHang(hv.ma());
    return kq;
}
double HethongCRM::dangKyGoi(const std::string& id,GoiTap goi,const Voucher* voucher){
    auto hv=timHoiVien(id);
    auto it=goiDichVus_.find(maGoiChuan(goi));
    const GoiDichVu service=it!=goiDichVus_.end()?it->second:GoiDichVu(goi);
    return thucHienDangKy(*hv,service,voucher,{},{},-1,"Tiền mặt").phi;
}
KetQuaDangKy HethongCRM::dangKyGoi(const std::string& maHV, const std::string& maGoi, const std::string& maKM, const std::string& maNV, double daTra, const std::string& phuongThuc) {
    auto hv = timHoiVien(maHV);
    if (!maNV.empty() && !timNhanSu(maNV)) throw std::out_of_range("Không tìm thấy nhân viên " + maNV + ".");
    return thucHienDangKy(*hv, goi(maGoi), nullptr, maKM, maNV, daTra, phuongThuc);
}
KetQuaDangKy HethongCRM::xemTruocPhi(const std::string& maHV, const std::string& maGoi, const std::string& maKM) const {
    auto it = goiDichVus_.find(maGoi);
    if (it == goiDichVus_.end()) throw std::out_of_range("Không tìm thấy gói " + maGoi + ".");
    return tinhPhi(*timHoiVien(maHV), it->second, maKM, nullptr);
}
void HethongCRM::themGoi(GoiDichVu g) { const auto ma = g.ma(); if (!goiDichVus_.emplace(ma, std::move(g)).second) throw std::invalid_argument("Mã gói đã tồn tại."); }
GoiDichVu& HethongCRM::goi(const std::string& ma) { auto it = goiDichVus_.find(ma); if (it == goiDichVus_.end()) throw std::out_of_range("Không tìm thấy gói " + ma + "."); return it->second; }
void HethongCRM::xoaGoi(const std::string& ma) {
    auto& g = goi(ma);
    if (std::any_of(dangKys_.begin(), dangKys_.end(), [&](const DangKyGoi& d) { return d.maGoi() == ma; })) { g.datDangBan(false); throw std::logic_error("Gói đã có người đăng ký nên chỉ chuyển sang ngừng bán."); }
    goiDichVus_.erase(ma);
}
void HethongCRM::themKhuyenMai(KhuyenMai km) {
    if (timKhuyenMai(km.ma())) throw std::invalid_argument("Mã khuyến mãi đã tồn tại.");
    if (!km.maGoi().empty() && !goiDichVus_.contains(km.maGoi())) throw std::out_of_range("Không tìm thấy gói " + km.maGoi() + ".");
    khuyenMais_.push_back(std::move(km));
}
void HethongCRM::xoaKhuyenMai(const std::string& ma) {
    const auto old = khuyenMais_.size();
    std::erase_if(khuyenMais_, [&](const KhuyenMai& k) { return k.ma() == ma; });
    if (old == khuyenMais_.size()) throw std::out_of_range("Không tìm thấy khuyến mãi " + ma + ".");
}
const KhuyenMai* HethongCRM::timKhuyenMai(const std::string& ma) const { for (const auto& k : khuyenMais_) if (k.ma() == ma) return &k; return nullptr; }
KhuyenMai* HethongCRM::timKhuyenMaiSua(const std::string& ma) { for (auto& k : khuyenMais_) if (k.ma() == ma) return &k; return nullptr; }
void HethongCRM::datUuDaiTheoHang(bool bat) {
    if (bat) chienLuocGia_ = std::make_unique<GiaTheoHangThanhVien>(); else chienLuocGia_ = std::make_unique<GiaTieuChuan>();
    caiDat_["uu_dai_theo_hang"] = bat ? "1" : "0";
}
bool HethongCRM::uuDaiTheoHang() const { return dynamic_cast<const GiaTheoHangThanhVien*>(chienLuocGia_.get()) != nullptr; }
const ChienLuocGia& HethongCRM::chienLuocGia() const noexcept { return *chienLuocGia_; }

// ===== PT & lịch =====
void HethongCRM::datLich(LichHen item){
    (void)timHoiVien(item.maHoiVien());(void)timPT(item.maPT());
    for(const auto& prior:lichHens_){
        if(prior.ma()==item.ma())throw std::invalid_argument("Mã lịch hẹn đã tồn tại.");
        if(item.trungLich(prior))throw std::logic_error("PT đã có lịch trong khoảng giờ này ("+prior.ma()+").");
        if(item.trungLichHoiVien(prior))throw std::logic_error("Hội viên đã có lịch khác trong khoảng giờ này ("+prior.ma()+").");
    }
    lichHens_.push_back(std::move(item));
}
std::string HethongCRM::datLichPT(const std::string& maHV, const std::string& maPT, DateTime batDau, int soPhut, const std::string& ghiChu) {
    auto hv = timHoiVien(maHV); auto& pt = timPT(maPT);
    if (!hv->duocCheckIn()) throw std::logic_error("Thẻ hội viên chưa kích hoạt (" + hv->trangThai() + ").");
    if (!pt.dangLam()) throw std::logic_error("PT đã nghỉ việc.");
    if (soPhut < 15 || soPhut > 240) throw std::invalid_argument("Thời lượng buổi tập từ 15 đến 240 phút.");
    if (batDau < now()) throw std::logic_error("Không thể đặt lịch trong quá khứ.");
    if (hv->ngayHetHan().ok() && ngayCua(batDau) > hv->ngayHetHan()) throw std::logic_error("Ngày tập vượt quá hạn thẻ của hội viên.");
    const int start = static_cast<int>(std::chrono::duration_cast<std::chrono::minutes>(batDau - std::chrono::floor<std::chrono::days>(batDau)).count());
    if (!pt.trongCaLam(start, start + soPhut)) throw std::logic_error("Ngoài ca làm của PT (" + std::to_string(pt.gioBatDau()) + "h-" + std::to_string(pt.gioKetThuc()) + "h).");
    LichHen lich(sinhMa("LH"), maHV, maPT, batDau, batDau + std::chrono::minutes{soPhut});
    if (!ghiChu.empty()) lich.napTrangThai("Đã đặt", ghiChu);
    const auto ma = lich.ma();
    datLich(std::move(lich));
    return ma;
}
LichHen& HethongCRM::timLich(const std::string& ma) { for (auto& l : lichHens_) if (l.ma() == ma) return l; throw std::out_of_range("Không tìm thấy lịch " + ma + "."); }
void HethongCRM::hoanThanhLich(const std::string& ma) {
    auto& lich = timLich(ma);
    auto hv = timHoiVien(lich.maHoiVien()); auto& pt = timPT(lich.maPT());
    if (lich.trangThai() != "Đã đặt") throw std::logic_error("Chỉ lịch đang đặt mới được hoàn thành.");
    const bool dungBuoiGoi = hv->soBuoiPT() > 0;
    if (!dungBuoiGoi && pt.giaBuoiChoHoiVien() > 0) {
        // Hội viên không còn buổi PT trong gói: lập hóa đơn buổi lẻ, ghi công nợ để lễ tân thu.
        HoaDon hd(sinhMa("HD"), hv->ma(), formatDate(today()));
        hd.themHang("Buổi tập PT lẻ với " + pt.ten(), 1, pt.giaBuoiChoHoiVien());
        hd.datLoai("Dịch vụ"); hd.datNhanVien(pt.ma()); hd.ghiNhanThanhToan(0, "Tiền mặt");
        themHoaDon(std::move(hd));
    }
    lich.hoanThanh();
    if (dungBuoiGoi) hv->truBuoiPT();
    pt.ghiNhanBuoiDay(pt.giaBuoiChoHoiVien());
    nhatKyTaps_.emplace_back(hv->ma(), ngayCua(lich.batDau()), "Buổi tập cùng PT " + pt.ten() + (lich.ghiChu().empty() ? "" : ": " + lich.ghiChu()), lich.thoiLuongPhut(), pt.ma());
}
void HethongCRM::huyLich(const std::string& ma, const std::string& lyDo) { timLich(ma).huy(lyDo); }
void HethongCRM::vangLich(const std::string& ma) { timLich(ma).danhDauVang(); }
std::vector<DateTime> HethongCRM::khungGioTrong(const std::string& maPT, Date ngay, int soPhut) const {
    const auto& pt = timPT(maPT);
    if (soPhut <= 0) throw std::invalid_argument("Thời lượng không hợp lệ.");
    std::vector<DateTime> result;
    const DateTime luc = now();
    for (int m = pt.gioBatDau() * 60; m + soPhut <= pt.gioKetThuc() * 60; m += 30) {
        const DateTime start = taoThoiDiem(ngay, m / 60, m % 60);
        if (start < luc) continue;
        LichHen thu("tmp", "tmp", maPT, start, start + std::chrono::minutes{soPhut});
        if (std::none_of(lichHens_.begin(), lichHens_.end(), [&](const LichHen& l) { return thu.trungLich(l); })) result.push_back(start);
    }
    return result;
}

// ===== Check-in =====
CheckIn* HethongCRM::luotDangMo(const std::string& maHV) {
    for (auto it = checkIns_.rbegin(); it != checkIns_.rend(); ++it) if (it->maHoiVien() == maHV && !it->daRa() && ngayCua(it->vao()) == today()) return &*it;
    return nullptr;
}
bool HethongCRM::dangTrongPhong(const std::string& maHV) const {
    return std::any_of(checkIns_.begin(), checkIns_.end(), [&](const CheckIn& c) { return c.maHoiVien() == maHV && !c.daRa() && ngayCua(c.vao()) == today(); });
}
int HethongCRM::soNguoiTrongPhong() const {
    return static_cast<int>(std::count_if(checkIns_.begin(), checkIns_.end(), [](const CheckIn& c) { return !c.daRa() && ngayCua(c.vao()) == today(); }));
}
int HethongCRM::soLuotHomNay() const {
    return static_cast<int>(std::count_if(checkIns_.begin(), checkIns_.end(), [](const CheckIn& c) { return ngayCua(c.vao()) == today(); }));
}
void HethongCRM::checkIn(const std::string& id){(void)checkIn(id,"Thủ công",false);}
const CheckIn& HethongCRM::checkIn(const std::string& id, const std::string& cach, bool ganTuTuDong) {
    auto hv = timHoiVien(id);
    if (dangTrongPhong(id)) throw std::logic_error("Hội viên " + id + " đang ở trong phòng tập (chưa check-out).");
    ++(*hv);
    CheckIn luot(id, now(), cach);
    if (ganTuTuDong) {
        const std::string thue = tuCuaHoiVien(id);
        if (!thue.empty()) luot.ganTu(thue);
        else {
            // Ưu tiên tủ đúng khu vực theo giới tính, sau đó khu chung.
            auto chon = [&](const std::string& khu) -> TuDo* { for (auto& t : tuDos_) if (t.trong() && (khu.empty() || t.khuVuc() == khu)) return &t; return nullptr; };
            TuDo* tu = hv->gioiTinh().empty() ? nullptr : chon(hv->gioiTinh());
            if (!tu) tu = chon("Chung");
            if (!tu) tu = chon({});
            if (tu) { tu->ganTheoLuot(id); luot.ganTu(tu->ma()); }
        }
    }
    checkIns_.push_back(std::move(luot));
    return checkIns_.back();
}
int HethongCRM::checkOut(const std::string& maHV) {
    CheckIn* luot = luotDangMo(maHV);
    if (!luot) throw std::logic_error("Hội viên " + maHV + " chưa check-in hôm nay.");
    luot->checkOut();
    if (!luot->maTu().empty()) for (auto& tu : tuDos_) if (tu.ma() == luot->maTu() && tu.trangThai() == "Đang dùng" && tu.maHoiVien() == maHV) tu.traTu();
    return luot->soPhut();
}
std::string HethongCRM::quetThe(const std::string& ma, const std::string& cach) {
    auto hv = timHoiVienTheoThe(ma);
    if (!hv) { auto it = hoiViens_.find(ma); if (it == hoiViens_.end()) throw std::out_of_range("Không nhận diện được thẻ/mã: " + ma + "."); hv = it->second; }
    if (dangTrongPhong(hv->ma())) {
        const int phut = checkOut(hv->ma());
        return "Tạm biệt " + hv->ten() + "! Check-out lúc " + formatTime(now()) + ", thời gian tập " + std::to_string(phut) + " phút.";
    }
    const auto& luot = checkIn(hv->ma(), cach, true);
    std::string msg = "Xin chào " + hv->ten() + "! Check-in lúc " + formatTime(luot.vao()) + ".";
    if (!luot.maTu().empty()) msg += " Tủ đồ: " + luot.maTu() + ".";
    if (hv->gioiHanBuoi()) msg += " Còn " + std::to_string(hv->soBuoiConLai()) + " buổi.";
    const int left = hv->soNgayConLai();
    if (left <= 7) msg += " Thẻ còn " + std::to_string(left) + " ngày, nhắc gia hạn!";
    if (DichVuCSKH::laSinhNhat(hv->ngaySinh(), today())) msg += " Chúc mừng sinh nhật!";
    return msg;
}

// ===== Nhân sự =====
void HethongCRM::themNhanVien(std::unique_ptr<NhanVien> nv) {
    if (!nv) throw std::invalid_argument("Nhân viên rỗng.");
    if (auto* pt = dynamic_cast<PT*>(nv.get())) { nv.release(); themPT(std::unique_ptr<PT>(pt)); return; }
    const auto id = nv->ma();
    if (pts_.contains(id) || !nhanViens_.emplace(id, std::move(nv)).second) throw std::invalid_argument("Mã nhân viên đã tồn tại.");
}
NhanVien* HethongCRM::timNhanSu(const std::string& ma) const {
    if (auto it = pts_.find(ma); it != pts_.end()) return it->second.get();
    if (auto it = nhanViens_.find(ma); it != nhanViens_.end()) return it->second.get();
    return nullptr;
}
std::vector<NhanVien*> HethongCRM::danhSachNhanSu() const {
    std::vector<NhanVien*> result;
    for (const auto& [id, nv] : nhanViens_) { (void)id; result.push_back(nv.get()); }
    for (const auto& [id, pt] : pts_) { (void)id; result.push_back(pt.get()); }
    std::sort(result.begin(), result.end(), [](const NhanVien* a, const NhanVien* b) { return a->ma() < b->ma(); });
    return result;
}
bool HethongCRM::dangTrongCa(const std::string& maNV) const {
    return std::any_of(chamCongs_.begin(), chamCongs_.end(), [&](const ChamCong& c) { return c.maNhanVien() == maNV && !c.daRa() && c.ngay() == today(); });
}
void HethongCRM::chamCongVao(const std::string& maNV) {
    NhanVien* nv = timNhanSu(maNV);
    if (!nv) throw std::out_of_range("Không tìm thấy nhân viên " + maNV + ".");
    if (!nv->dangLam()) throw std::logic_error("Nhân viên đã nghỉ việc.");
    if (dangTrongCa(maNV)) throw std::logic_error(nv->ten() + " đã chấm công vào, chưa chấm công ra.");
    chamCongs_.emplace_back(maNV, now());
    if (auto* pt = dynamic_cast<PT*>(nv)) pt->chamCong();
}
void HethongCRM::chamCongRa(const std::string& maNV) {
    for (auto it = chamCongs_.rbegin(); it != chamCongs_.rend(); ++it)
        if (it->maNhanVien() == maNV && !it->daRa()) { it->ketThuc(); return; }
    throw std::logic_error("Nhân viên " + maNV + " chưa chấm công vào.");
}
std::vector<DongLuong> HethongCRM::bangLuong(int nam, unsigned thang) const {
    const Date dau{std::chrono::year{nam}, std::chrono::month{thang}, std::chrono::day{1}};
    if (!dau.ok()) throw std::invalid_argument("Tháng không hợp lệ.");
    const Date cuoi = addDays(addMonths(dau, 1), -1);
    auto trongKy = [&](Date d) { return d.ok() && dau <= d && d <= cuoi; };
    int congChuan = 26;
    try { congChuan = std::stoi(caiDat("cong_chuan", "26")); } catch (...) {}
    std::vector<DongLuong> rows;
    for (NhanVien* nv : danhSachNhanSu()) {
        KyLuong ky; ky.congChuan = congChuan;
        std::set<std::string> days; double hours = 0;
        for (const auto& c : chamCongs_) if (c.maNhanVien() == nv->ma() && trongKy(c.ngay())) { days.insert(formatDate(c.ngay())); hours += c.soGio(); }
        ky.soCong = static_cast<int>(days.size());
        for (const auto& d : dangKys_) if (d.maNhanVien() == nv->ma() && trongKy(d.ngay())) ky.doanhSo += d.phi();
        for (const auto& hd : hoaDons_) if (hd.maNhanVien() == nv->ma() && hd.loai() == "Bán lẻ" && trongKy(parseDate(hd.ngay()))) ky.doanhSo += hd.tongTien();
        if (auto* pt = dynamic_cast<PT*>(nv)) {
            for (const auto& l : lichHens_) if (l.maPT() == pt->ma() && l.trangThai() == "Hoàn thành" && trongKy(ngayCua(l.batDau()))) { ++ky.soCaPT; ky.doanhThuPT += pt->giaBuoiChoHoiVien(); }
        }
        if (!nv->dangLam() && ky.soCong == 0 && ky.doanhSo == 0 && ky.soCaPT == 0) continue;
        DongLuong row;
        row.maNhanVien = nv->ma(); row.ten = nv->ten(); row.chucVu = nv->chucVu();
        row.soCong = ky.soCong; row.soGio = hours; row.soCaPT = ky.soCaPT; row.doanhSo = ky.doanhSo + ky.doanhThuPT;
        row.hoaHong = nv->hoaHong(ky) + ky.doanhThuPT * nv->tyLeHoaHong();
        row.luong = nv->tinhLuong(ky);
        rows.push_back(std::move(row));
    }
    return rows;
}

// ===== Bán hàng, kho, tài chính =====
void HethongCRM::themHoaDon(HoaDon hd) {
    if (std::any_of(hoaDons_.begin(), hoaDons_.end(), [&](const HoaDon& x) { return x.ma() == hd.ma(); })) throw std::invalid_argument("Mã hóa đơn đã tồn tại.");
    const auto hv = hd.maHoiVien();
    hoaDons_.push_back(std::move(hd));
    if (hv != KHACH_LE && hoiViens_.contains(hv)) capNhatHang(hv);
}
void HethongCRM::banHang(HoaDon invoice){if(invoice.maHoiVien()!=KHACH_LE)(void)timHoiVien(invoice.maHoiVien());if(invoice.dongHang().empty())throw std::invalid_argument("Hóa đơn chưa có mặt hàng.");themHoaDon(std::move(invoice));}
std::string HethongCRM::banLe(const std::string& maHV, const std::vector<std::pair<std::string,int>>& gio, double giamGia, double daTra, const std::string& cach, const std::string& maNV) {
    const std::string khach = maHV.empty() ? std::string{KHACH_LE} : maHV;
    if (khach != KHACH_LE) (void)timHoiVien(khach);
    if (gio.empty()) throw std::invalid_argument("Giỏ hàng trống.");
    std::map<std::string, int> tong;
    for (const auto& [ma, sl] : gio) { if (sl <= 0) throw std::invalid_argument("Số lượng phải lớn hơn 0."); tong[ma] += sl; }
    for (const auto& [ma, sl] : tong) {
        const auto& sp = sanPham(ma);
        if (!sp.dangBan()) throw std::logic_error(sp.ten() + " đã ngừng bán.");
        if (sp.tonKho() < sl) throw std::logic_error("Không đủ hàng: " + sp.ten() + " (còn " + std::to_string(sp.tonKho()) + ").");
    }
    HoaDon hd(sinhMa("HD"), khach, formatDate(today()));
    for (const auto& [ma, sl] : tong) { const auto& sp = sanPham(ma); hd.themHang(sp.ten(), sl, sp.giaBan(), ma); }
    hd.datLoai("Bán lẻ"); hd.datNhanVien(maNV); hd.datGiamGia(giamGia);
    hd.ghiNhanThanhToan(daTra < 0 ? hd.tongTien() : daTra, cach);
    if (khach == KHACH_LE && hd.coCongNo()) throw std::logic_error("Khách lẻ phải thanh toán đủ.");
    for (const auto& [ma, sl] : tong) {
        sanPham(ma).xuatKho(sl);
        phieuKhos_.emplace_back(sinhMa("PK"), ma, today(), "Xuất bán", sl, sanPham(ma).giaBan(), "Hóa đơn " + hd.ma());
    }
    const auto code = hd.ma();
    themHoaDon(std::move(hd));
    return code;
}
void HethongCRM::thuNo(const std::string& ma, double soTien, const std::string& cach) {
    for (auto& hd : hoaDons_) if (hd.ma() == ma) { hd.thuNo(soTien, cach); return; }
    throw std::out_of_range("Không tìm thấy hóa đơn " + ma + ".");
}
const HoaDon& HethongCRM::timHoaDon(const std::string& ma) const {
    for (const auto& hd : hoaDons_) if (hd.ma() == ma) return hd;
    throw std::out_of_range("Không tìm thấy hóa đơn " + ma + ".");
}
void HethongCRM::ghiNhanChi(GiaoDich expense){if(expense.loai()!=LoaiGiaoDich::Chi)throw std::invalid_argument("Giao dịch phải là loại chi.");for(const auto& item:chiPhis_)if(item.ma()==expense.ma())throw std::invalid_argument("Mã giao dịch đã tồn tại.");chiPhis_.push_back(std::move(expense));}
void HethongCRM::ghiNhanThu(GiaoDich thu){if(thu.loai()!=LoaiGiaoDich::Thu)throw std::invalid_argument("Giao dịch phải là loại thu.");for(const auto& item:thuKhacs_)if(item.ma()==thu.ma())throw std::invalid_argument("Mã giao dịch đã tồn tại.");thuKhacs_.push_back(std::move(thu));}
void HethongCRM::xoaChiPhi(const std::string& ma) {
    const auto old = chiPhis_.size() + thuKhacs_.size();
    std::erase_if(chiPhis_, [&](const GiaoDich& g) { return g.ma() == ma; });
    std::erase_if(thuKhacs_, [&](const GiaoDich& g) { return g.ma() == ma; });
    if (old == chiPhis_.size() + thuKhacs_.size()) throw std::out_of_range("Không tìm thấy giao dịch " + ma + ".");
}
void HethongCRM::themSanPham(SanPham sp) { const auto ma = sp.ma(); if (!sanPhams_.emplace(ma, std::move(sp)).second) throw std::invalid_argument("Mã sản phẩm đã tồn tại."); }
SanPham& HethongCRM::sanPham(const std::string& ma) { auto it = sanPhams_.find(ma); if (it == sanPhams_.end()) throw std::out_of_range("Không tìm thấy sản phẩm " + ma + "."); return it->second; }
void HethongCRM::xoaSanPham(const std::string& ma) {
    auto& sp = sanPham(ma);
    if (std::any_of(phieuKhos_.begin(), phieuKhos_.end(), [&](const PhieuKho& p) { return p.maSanPham() == ma; })) { sp.datDangBan(false); throw std::logic_error("Sản phẩm đã có giao dịch kho nên chỉ chuyển sang ngừng bán."); }
    sanPhams_.erase(ma);
}
std::string HethongCRM::nhapHang(const std::string& maSP, int sl, double gia, const std::string& ncc, bool ghiChi) {
    auto& sp = sanPham(maSP);
    sp.nhapKho(sl, gia);
    const auto ma = sinhMa("PK");
    phieuKhos_.emplace_back(ma, maSP, today(), "Nhập", sl, gia, ncc.empty() ? std::string{} : "NCC: " + ncc);
    if (ghiChi && sl * gia > 0) chiPhis_.emplace_back(sinhMa("CP"), "Nhập " + std::to_string(sl) + " " + sp.donVi() + " " + sp.ten(), today(), LoaiGiaoDich::Chi, sl * gia, "Nhập hàng");
    return ma;
}
int HethongCRM::kiemKe(const std::string& maSP, int thucTe, const std::string& ghiChu) {
    auto& sp = sanPham(maSP);
    const int diff = sp.dieuChinhTon(thucTe);
    phieuKhos_.emplace_back(sinhMa("PK"), maSP, today(), "Kiểm kê", diff, sp.giaNhap(), ghiChu.empty() ? "Tồn thực tế " + std::to_string(thucTe) : ghiChu);
    return diff;
}

// ===== Thiết bị, vệ sinh, tủ đồ =====
void HethongCRM::themThietBi(ThietBi item){for(const auto& old:thietBis_)if(old.ma()==item.ma())throw std::invalid_argument("Mã thiết bị đã tồn tại.");thietBis_.push_back(std::move(item));}
ThietBi& HethongCRM::thietBi(const std::string& ma) { for (auto& t : thietBis_) if (t.ma() == ma) return t; throw std::out_of_range("Không tìm thấy thiết bị " + ma + "."); }
void HethongCRM::baoTriThietBi(const std::string& maTB, const std::string& noiDung, double chiPhi, const std::string& nguoi, Date tiepTheo) {
    auto& tb = thietBi(maTB);
    if (tiepTheo.ok() && tiepTheo <= today()) throw std::invalid_argument("Ngày bảo trì tiếp theo phải sau hôm nay.");
    baoTris_.emplace_back(maTB, today(), noiDung, chiPhi, nguoi);
    if (chiPhi > 0) chiPhis_.emplace_back(sinhMa("CP"), "Bảo trì " + tb.ten() + ": " + noiDung, today(), LoaiGiaoDich::Chi, chiPhi, "Bảo trì");
    if (tiepTheo.ok()) tb.lenLichBaoTri(tiepTheo);
    tb.capNhatTrangThai("Hoạt động");
}
void HethongCRM::ghiNhanVeSinh(KiemTraVeSinh report){kiemTraVeSinhs_.push_back(std::move(report));}
void HethongCRM::themTuDo(TuDo tu) { for (const auto& t : tuDos_) if (t.ma() == tu.ma()) throw std::invalid_argument("Mã tủ đã tồn tại."); tuDos_.push_back(std::move(tu)); }
int HethongCRM::themNhieuTuDo(const std::string& prefix, int soLuong, const std::string& khu) {
    if (prefix.empty() || soLuong <= 0 || soLuong > 500) throw std::invalid_argument("Tiền tố hoặc số lượng tủ không hợp lệ.");
    int added = 0, n = soLonNhat(tuDos_, prefix, [](const TuDo& t) -> const std::string& { return t.ma(); });
    while (added < soLuong) {
        std::ostringstream ma; ma << prefix << std::setw(2) << std::setfill('0') << ++n;
        tuDos_.emplace_back(ma.str(), khu); ++added;
    }
    return added;
}
TuDo& HethongCRM::tuDo(const std::string& ma) { for (auto& t : tuDos_) if (t.ma() == ma) return t; throw std::out_of_range("Không tìm thấy tủ " + ma + "."); }
void HethongCRM::xoaTuDo(const std::string& ma) {
    const auto& t = tuDo(ma);
    if (t.trangThai() == "Đang dùng" || t.trangThai() == "Thuê tháng") throw std::logic_error("Tủ đang có người dùng.");
    std::erase_if(tuDos_, [&](const TuDo& x) { return x.ma() == ma; });
}
void HethongCRM::ganTu(const std::string& maTu, const std::string& maHV) {
    (void)timHoiVien(maHV);
    tuDo(maTu).ganTheoLuot(maHV);
    if (CheckIn* luot = luotDangMo(maHV)) luot->ganTu(maTu);
}
void HethongCRM::traTu(const std::string& maTu) { tuDo(maTu).traTu(); }
std::string HethongCRM::thueTuThang(const std::string& maTu, const std::string& maHV, int soThang, const std::string& cach) {
    (void)timHoiVien(maHV);
    if (soThang <= 0 || soThang > 24) throw std::invalid_argument("Số tháng thuê từ 1 đến 24.");
    double gia = 100000;
    try { gia = std::stod(caiDat("gia_thue_tu", "100000")); } catch (...) {}
    auto& tu = tuDo(maTu);
    const Date base = (tu.trangThai() == "Thuê tháng" && tu.maHoiVien() == maHV && tu.hetHanThue() >= today()) ? addDays(tu.hetHanThue(), 1) : today();
    const Date het = addDays(addMonths(base, soThang), -1);
    HoaDon hd(sinhMa("HD"), maHV, formatDate(today()));
    hd.themHang("Thuê tủ " + maTu + " " + std::to_string(soThang) + " tháng", soThang, gia);
    hd.datLoai("Dịch vụ"); hd.ghiNhanThanhToan(hd.tongTien(), cach);
    tu.choThue(maHV, het);
    const auto ma = hd.ma();
    themHoaDon(std::move(hd));
    return ma;
}
std::string HethongCRM::tuCuaHoiVien(const std::string& maHV) const {
    for (const auto& t : tuDos_) if (t.maHoiVien() == maHV && (t.trangThai() == "Thuê tháng" || t.trangThai() == "Đang dùng")) return t.ma();
    return {};
}

// ===== CSKH =====
std::vector<std::string> HethongCRM::nhacSinhNhat(Date d)const{std::vector<std::string> result;for(const auto& [id,hv]:hoiViens_)if(DichVuCSKH::laSinhNhat(hv->ngaySinh(),d))result.push_back(id+" - "+hv->ten());return result;}
std::vector<std::string> HethongCRM::nhacGiaHan(int days)const{std::vector<std::string> result;for(const auto& [id,hv]:hoiViens_)if(hv->trangThai()=="Kích hoạt"&&hv->ngayHetHan().ok()&&DichVuCSKH::canNhacGiaHan(hv->ngayHetHan(),days))result.push_back(id+" - "+hv->ten()+" ("+formatDate(hv->ngayHetHan())+")");return result;}
std::vector<std::string> HethongCRM::nhacLichTap(int days)const{if(days<0)throw std::invalid_argument("Số ngày nhắc không hợp lệ.");const auto from=now();const auto end=from+std::chrono::days{days};std::vector<std::string> result;for(const auto& event:lichHens_)if(event.trangThai()=="Đã đặt"&&event.batDau()>=from&&event.batDau()<=end)result.push_back(DichVuCSKH::taoNhacLichTap(event));return result;}
std::vector<std::pair<std::string,int>> HethongCRM::hoiVienLauKhongDen(int soNgay) const {
    std::vector<std::pair<std::string,int>> result;
    for (const auto& [id, hv] : hoiViens_) {
        if (hv->trangThai() != "Kích hoạt") continue;
        const auto last = hv->lanCheckInCuoi();
        const int vang = last.empty() ? soNgayGiua(hv->ngayThamGia(), today()) : soNgayGiua(parseDate(last), today());
        if (vang >= soNgay) result.emplace_back(id, vang);
    }
    std::sort(result.begin(), result.end(), [](const auto& a, const auto& b) { return a.second > b.second; });
    return result;
}
std::string HethongCRM::tangQuaSinhNhat(const std::string& maHV, double phanTram, int soNgay, const std::string& nguoi) {
    auto hv = timHoiVien(maHV);
    if (soNgay <= 0) throw std::invalid_argument("Số ngày hiệu lực phải lớn hơn 0.");
    std::string ma = "SN" + std::to_string(int(today().year())) + maHV;
    if (timKhuyenMai(ma)) throw std::logic_error("Hội viên đã nhận quà sinh nhật năm nay (" + ma + ").");
    themKhuyenMai(KhuyenMai(ma, "Quà sinh nhật " + hv->ten(), LoaiGiamGia::PhanTram, phanTram, today(), addDays(today(), soNgay), false, {}, 1, maHV));
    ghiChuCSKHs_.emplace_back(maHV, "Sinh nhật", "Gửi lời chúc và tặng voucher " + ma + " giảm " + tien(phanTram) + "%", today(), nguoi);
    return ma;
}
void HethongCRM::ghiChuCSKH(GhiChuCSKH g) { (void)timHoiVien(g.maHoiVien()); ghiChuCSKHs_.push_back(std::move(g)); }
std::string HethongCRM::themPhanHoi(const std::string& maHV, const std::string& maDT, int diem, const std::string& noiDung) {
    (void)timHoiVien(maHV);
    if (!maDT.empty() && maDT != "PHONG_TAP" && !timNhanSu(maDT)) throw std::out_of_range("Không tìm thấy nhân sự " + maDT + ".");
    const auto ma = sinhMa("PH");
    phanHois_.emplace_back(ma, maHV, maDT, diem, noiDung);
    return ma;
}
void HethongCRM::xuLyPhanHoi(const std::string& ma, const std::string& kq) {
    for (auto& p : phanHois_) if (p.ma() == ma) { p.danhDauDaXuLy(kq); return; }
    throw std::out_of_range("Không tìm thấy phản hồi " + ma + ".");
}
std::pair<double,int> HethongCRM::diemDanhGia(const std::string& maDT) const {
    double sum = 0; int n = 0;
    for (const auto& p : phanHois_) if (p.maDoiTuong() == maDT) { sum += p.diem(); ++n; }
    return {n ? sum / n : 0.0, n};
}
std::vector<CanhBao> HethongCRM::canhBao() const {
    std::vector<CanhBao> out;
    const Date d = today();
    for (const auto& [id, hv] : hoiViens_) {
        if (hv->trangThai() == "Kích hoạt" && hv->ngayHetHan().ok()) {
            const int left = hv->soNgayConLai();
            if (left >= 0 && left <= 7) out.push_back({"Sắp hết hạn", id + " - " + hv->ten(), "Thẻ còn " + std::to_string(left) + " ngày (" + formatDate(hv->ngayHetHan()) + ")", formatDate(hv->ngayHetHan()), left <= 2 ? 2 : 1});
        }
        if (hv->gioiHanBuoi() && hv->trangThai() == "Kích hoạt" && hv->soBuoiConLai() <= 2)
            out.push_back({"Sắp hết buổi", id + " - " + hv->ten(), "Còn " + std::to_string(hv->soBuoiConLai()) + " buổi tập", formatDate(d), 1});
        if (DichVuCSKH::laSinhNhat(hv->ngaySinh(), d)) out.push_back({"Sinh nhật", id + " - " + hv->ten(), "Sinh nhật hôm nay, " + std::to_string(hv->tuoi()) + " tuổi", formatDate(d), 0});
    }
    for (const auto& text : nhacLichTap(1)) out.push_back({"Lịch PT", "", text, formatDate(d), 0});
    for (const auto& [ma, sp] : sanPhams_) if (sp.sapHet()) out.push_back({"Sắp hết hàng", ma + " - " + sp.ten(), "Tồn " + std::to_string(sp.tonKho()) + " " + sp.donVi() + " (tối thiểu " + std::to_string(sp.mucToiThieu()) + ")", formatDate(d), sp.tonKho() == 0 ? 2 : 1});
    for (const auto& tb : thietBis_) if (tb.canBaoTri(d)) out.push_back({"Bảo trì", tb.ma() + " - " + tb.ten(), tb.trangThai() == "Hỏng" ? "Thiết bị đang hỏng" : "Đến hạn bảo trì " + formatDate(tb.ngayBaoTri()), formatDate(tb.ngayBaoTri()), tb.trangThai() == "Hỏng" ? 2 : 1});
    for (const auto& hd : hoaDons_) if (hd.conNo() > 0) out.push_back({"Công nợ", hd.ma() + " - " + hd.maHoiVien(), "Còn nợ " + tien(hd.conNo()) + " VND", hd.ngay(), soNgayGiua(parseDate(hd.ngay()), d) > 14 ? 2 : 1});
    for (const auto& p : phanHois_) if (p.trangThai() == "Mới") out.push_back({"Phản hồi mới", p.ma() + " - " + p.maHoiVien(), std::to_string(p.diem()) + "★: " + p.noiDung(), formatDate(p.ngay()), p.diem() <= 2 ? 2 : 0});
    for (const auto& t : tuDos_) if (t.trangThai() == "Thuê tháng" && t.hetHanThue().ok() && soNgayGiua(d, t.hetHanThue()) <= 3) out.push_back({"Tủ sắp hết hạn", t.ma() + " - " + t.maHoiVien(), "Hết hạn thuê " + formatDate(t.hetHanThue()), formatDate(t.hetHanThue()), 0});
    std::stable_sort(out.begin(), out.end(), [](const CanhBao& a, const CanhBao& b) { return a.mucDo > b.mucDo; });
    return out;
}

// ===== Tài khoản & cài đặt =====
void HethongCRM::themTaiKhoan(TaiKhoan tk) {
    for (const auto& t : taiKhoans_) if (t.tenDangNhap() == tk.tenDangNhap()) throw std::invalid_argument("Tên đăng nhập đã tồn tại.");
    if (!tk.maNhanVien().empty() && !timNhanSu(tk.maNhanVien())) throw std::out_of_range("Không tìm thấy nhân viên " + tk.maNhanVien() + ".");
    taiKhoans_.push_back(std::move(tk));
}
const TaiKhoan* HethongCRM::dangNhap(const std::string& ten, const std::string& mk) const {
    for (const auto& t : taiKhoans_) if (t.tenDangNhap() == ten) return t.kiemTra(mk) ? &t : nullptr;
    return nullptr;
}
TaiKhoan& HethongCRM::taiKhoan(const std::string& ten) { for (auto& t : taiKhoans_) if (t.tenDangNhap() == ten) return t; throw std::out_of_range("Không tìm thấy tài khoản " + ten + "."); }
void HethongCRM::xoaTaiKhoan(const std::string& ten) {
    const auto& tk = taiKhoan(ten);
    const auto admins = std::count_if(taiKhoans_.begin(), taiKhoans_.end(), [](const TaiKhoan& t) { return t.vaiTro() == VaiTro::Admin && !t.khoa(); });
    if (tk.vaiTro() == VaiTro::Admin && admins <= 1) throw std::logic_error("Không thể xóa tài khoản quản trị cuối cùng.");
    std::erase_if(taiKhoans_, [&](const TaiKhoan& t) { return t.tenDangNhap() == ten; });
}
std::string HethongCRM::caiDat(const std::string& khoa, const std::string& macDinh) const { auto it = caiDat_.find(khoa); return it == caiDat_.end() ? macDinh : it->second; }
void HethongCRM::datCaiDat(const std::string& khoa, const std::string& giaTri) {
    if (khoa.empty()) throw std::invalid_argument("Khóa cài đặt rỗng.");
    if (khoa == "uu_dai_theo_hang") { datUuDaiTheoHang(giaTri == "1"); return; }
    caiDat_[khoa] = giaTri;
}

double HethongCRM::tongDoanhThu()const noexcept{double sum=0;for(const auto& invoice:hoaDons_)sum+=invoice.tongTien();for(const auto& t:thuKhacs_)sum+=t.soTien();return sum;}
double HethongCRM::tongChi()const noexcept{double sum=0;for(const auto& expense:chiPhis_)sum+=expense.soTien();return sum;}
double HethongCRM::soDu()const noexcept{return tongDoanhThu()-tongChi();}
double HethongCRM::tongCongNo()const noexcept{double sum=0;for(const auto& hd:hoaDons_)sum+=hd.conNo();return sum;}
double HethongCRM::tongDaThu()const noexcept{double sum=0;for(const auto& hd:hoaDons_)sum+=hd.daThanhToan();for(const auto& t:thuKhacs_)sum+=t.soTien();return sum;}
}
