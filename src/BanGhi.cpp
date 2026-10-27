#include "BanGhi.h"
#include <stdexcept>
#include <utility>
namespace crm {
DangKyGoi::DangKyGoi(std::string ma, std::string hv, std::string goi, std::string ten, std::string loai, Date ngay, Date tu, Date den,
                     double giaGoc, double phi, std::string nv, std::string km, std::string hd)
    : ma_(std::move(ma)), maHoiVien_(std::move(hv)), maGoi_(std::move(goi)), tenGoi_(std::move(ten)), loai_(std::move(loai)), maNhanVien_(std::move(nv)),
      maKhuyenMai_(std::move(km)), maHoaDon_(std::move(hd)), ngay_(ngay), tuNgay_(tu), denNgay_(den), giaGoc_(giaGoc), phi_(phi) {
    if (ma_.empty() || maHoiVien_.empty() || loai_.empty() || !ngay_.ok() || !denNgay_.ok() || giaGoc_ < 0 || phi_ < 0) throw std::invalid_argument("Bản ghi đăng ký gói không hợp lệ.");
}
const std::string& DangKyGoi::ma() const noexcept { return ma_; } const std::string& DangKyGoi::maHoiVien() const noexcept { return maHoiVien_; }
const std::string& DangKyGoi::maGoi() const noexcept { return maGoi_; } const std::string& DangKyGoi::tenGoi() const noexcept { return tenGoi_; }
const std::string& DangKyGoi::loai() const noexcept { return loai_; } const std::string& DangKyGoi::maNhanVien() const noexcept { return maNhanVien_; }
const std::string& DangKyGoi::maKhuyenMai() const noexcept { return maKhuyenMai_; } const std::string& DangKyGoi::maHoaDon() const noexcept { return maHoaDon_; }
Date DangKyGoi::ngay() const noexcept { return ngay_; } Date DangKyGoi::tuNgay() const noexcept { return tuNgay_; } Date DangKyGoi::denNgay() const noexcept { return denNgay_; }
double DangKyGoi::giaGoc() const noexcept { return giaGoc_; } double DangKyGoi::phi() const noexcept { return phi_; }

LichSuBaoTri::LichSuBaoTri(std::string tb, Date ngay, std::string nd, double cp, std::string nguoi)
    : maThietBi_(std::move(tb)), noiDung_(std::move(nd)), nguoiThucHien_(std::move(nguoi)), ngay_(ngay), chiPhi_(cp) {
    if (maThietBi_.empty() || noiDung_.empty() || !ngay_.ok() || chiPhi_ < 0) throw std::invalid_argument("Phiếu bảo trì không hợp lệ.");
}
const std::string& LichSuBaoTri::maThietBi() const noexcept { return maThietBi_; } const std::string& LichSuBaoTri::noiDung() const noexcept { return noiDung_; }
const std::string& LichSuBaoTri::nguoiThucHien() const noexcept { return nguoiThucHien_; } Date LichSuBaoTri::ngay() const noexcept { return ngay_; } double LichSuBaoTri::chiPhi() const noexcept { return chiPhi_; }

ChamCong::ChamCong(std::string nv, DateTime vao, DateTime ra) : maNhanVien_(std::move(nv)), vao_(vao), ra_(ra) {
    if (maNhanVien_.empty()) throw std::invalid_argument("Mã nhân viên rỗng.");
    if (daRa() && ra_ < vao_) throw std::invalid_argument("Giờ ra phải sau giờ vào.");
}
const std::string& ChamCong::maNhanVien() const noexcept { return maNhanVien_; }
DateTime ChamCong::vao() const noexcept { return vao_; } DateTime ChamCong::ra() const noexcept { return ra_; }
bool ChamCong::daRa() const noexcept { return ra_.time_since_epoch().count() != 0; }
Date ChamCong::ngay() const noexcept { return ngayCua(vao_); }
double ChamCong::soGio() const { const auto end = daRa() ? ra_ : now(); return end < vao_ ? 0.0 : std::chrono::duration<double, std::ratio<3600>>(end - vao_).count(); }
void ChamCong::ketThuc(DateTime luc) { if (daRa()) throw std::logic_error("Ca làm này đã chấm công ra."); if (luc < vao_) throw std::invalid_argument("Giờ ra phải sau giờ vào."); ra_ = luc; }

PhanHoi::PhanHoi(std::string ma, std::string hv, std::string dt, int diem, std::string nd, Date ngay)
    : ma_(std::move(ma)), maHoiVien_(std::move(hv)), maDoiTuong_(std::move(dt)), noiDung_(std::move(nd)), diem_(diem), ngay_(ngay) {
    if (ma_.empty() || maHoiVien_.empty() || diem_ < 1 || diem_ > 5 || !ngay_.ok()) throw std::invalid_argument("Phản hồi không hợp lệ (điểm từ 1 đến 5).");
    if (maDoiTuong_.empty()) maDoiTuong_ = "PHONG_TAP";
}
const std::string& PhanHoi::ma() const noexcept { return ma_; } const std::string& PhanHoi::maHoiVien() const noexcept { return maHoiVien_; }
const std::string& PhanHoi::maDoiTuong() const noexcept { return maDoiTuong_; } const std::string& PhanHoi::noiDung() const noexcept { return noiDung_; }
const std::string& PhanHoi::trangThai() const noexcept { return trangThai_; } const std::string& PhanHoi::xuLy() const noexcept { return xuLy_; }
int PhanHoi::diem() const noexcept { return diem_; } Date PhanHoi::ngay() const noexcept { return ngay_; }
void PhanHoi::danhDauDaXuLy(std::string kq) { if (trangThai_ == "Đã xử lý") throw std::logic_error("Phản hồi đã được xử lý."); trangThai_ = "Đã xử lý"; xuLy_ = std::move(kq); }
void PhanHoi::napTrangThai(std::string tt, std::string xl) { trangThai_ = tt.empty() ? "Mới" : std::move(tt); xuLy_ = std::move(xl); }

PhieuInBody::PhieuInBody(std::string hv, Date ngay, InBody cs) : maHoiVien_(std::move(hv)), ngay_(ngay), chiSo_(cs) {
    if (maHoiVien_.empty() || !ngay_.ok()) throw std::invalid_argument("Phiếu InBody không hợp lệ.");
}
const std::string& PhieuInBody::maHoiVien() const noexcept { return maHoiVien_; } Date PhieuInBody::ngay() const noexcept { return ngay_; } const InBody& PhieuInBody::chiSo() const noexcept { return chiSo_; }

NhatKyTap::NhatKyTap(std::string hv, Date ngay, std::string nd, int phut, std::string pt)
    : maHoiVien_(std::move(hv)), maPT_(std::move(pt)), noiDung_(std::move(nd)), ngay_(ngay), thoiLuongPhut_(phut) {
    if (maHoiVien_.empty() || noiDung_.empty() || !ngay_.ok() || thoiLuongPhut_ <= 0) throw std::invalid_argument("Nhật ký tập không hợp lệ.");
}
const std::string& NhatKyTap::maHoiVien() const noexcept { return maHoiVien_; } const std::string& NhatKyTap::maPT() const noexcept { return maPT_; }
const std::string& NhatKyTap::noiDung() const noexcept { return noiDung_; } Date NhatKyTap::ngay() const noexcept { return ngay_; } int NhatKyTap::thoiLuongPhut() const noexcept { return thoiLuongPhut_; }

GhiChuCSKH::GhiChuCSKH(std::string hv, std::string loai, std::string nd, Date ngay, std::string nguoi)
    : maHoiVien_(std::move(hv)), loai_(std::move(loai)), noiDung_(std::move(nd)), nguoiGhi_(std::move(nguoi)), ngay_(ngay) {
    if (maHoiVien_.empty() || loai_.empty() || noiDung_.empty() || !ngay_.ok()) throw std::invalid_argument("Ghi chú CSKH không hợp lệ.");
}
const std::string& GhiChuCSKH::maHoiVien() const noexcept { return maHoiVien_; } const std::string& GhiChuCSKH::loai() const noexcept { return loai_; }
const std::string& GhiChuCSKH::noiDung() const noexcept { return noiDung_; } const std::string& GhiChuCSKH::nguoiGhi() const noexcept { return nguoiGhi_; }
Date GhiChuCSKH::ngay() const noexcept { return ngay_; }
}
