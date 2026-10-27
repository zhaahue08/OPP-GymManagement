#include "GoiDichVu.h"
#include "HoiVien.h"
#include <stdexcept>
#include <utility>
namespace crm {
std::string tenLoaiGoi(LoaiGoi loai) {
    switch (loai) { case LoaiGoi::TheoNgay: return "Theo ngày"; case LoaiGoi::TheoThang: return "Theo tháng"; case LoaiGoi::TheoBuoi: return "Theo số buổi"; case LoaiGoi::KemPT: return "Kèm PT"; }
    return "Khác";
}
LoaiGoi loaiGoiTuTen(const std::string& ten) {
    if (ten == "Theo ngày") return LoaiGoi::TheoNgay;
    if (ten == "Theo số buổi") return LoaiGoi::TheoBuoi;
    if (ten == "Kèm PT") return LoaiGoi::KemPT;
    return LoaiGoi::TheoThang;
}
GoiDichVu::GoiDichVu(GoiTap loai) : loai_(loai) {
    switch (loai_) {
        case GoiTap::MotNgay: kieu_=LoaiGoi::TheoNgay; soThang_=0; soBuoi_=1; gia_=36000; break;
        case GoiTap::MotThang: soThang_=1; soBuoi_=0; gia_=339000; giaSV_=240000; break;
        case GoiTap::BaThang: soThang_=3; soBuoi_=0; gia_=899000; break;
        case GoiTap::SauThang: soThang_=6; soBuoi_=0; gia_=1499000; break;
        case GoiTap::MuoiBaThang: soThang_=13; soBuoi_=0; gia_=2999000; break;
        default: throw std::invalid_argument("Gói tập không hợp lệ.");
    }
    ma_ = maGoiChuan(loai_); ten_ = "Gói tập " + tenGoi(loai_);
}
GoiDichVu::GoiDichVu(std::string ma, std::string ten, LoaiGoi kieu, int soThang, int soNgay, int soBuoi, int soBuoiPT, double gia, double giaSV) : ma_(std::move(ma)) {
    if (ma_.empty()) throw std::invalid_argument("Mã gói không được rỗng.");
    capNhat(std::move(ten), kieu, soThang, soNgay, soBuoi, soBuoiPT, gia, giaSV);
}
void GoiDichVu::capNhat(std::string ten, LoaiGoi kieu, int soThang, int soNgay, int soBuoi, int soBuoiPT, double gia, double giaSV) {
    if (ten.empty() || soThang < 0 || soNgay < 0 || soBuoi < 0 || soBuoiPT < 0 || gia < 0 || giaSV < 0) throw std::invalid_argument("Thông tin gói tập không hợp lệ.");
    if (kieu == LoaiGoi::TheoBuoi && soBuoi == 0) throw std::invalid_argument("Gói theo số buổi cần số buổi > 0.");
    if (kieu == LoaiGoi::KemPT && soBuoiPT == 0) throw std::invalid_argument("Gói kèm PT cần số buổi PT > 0.");
    if (kieu != LoaiGoi::TheoNgay && soThang == 0 && soNgay == 0) throw std::invalid_argument("Gói cần thời hạn (số tháng hoặc số ngày).");
    ten_ = std::move(ten); kieu_ = kieu; soThang_ = soThang; soNgay_ = soNgay; soBuoi_ = soBuoi; soBuoiPT_ = soBuoiPT; gia_ = gia; giaSV_ = giaSV;
}
GoiTap GoiDichVu::loai() const noexcept { return loai_; } int GoiDichVu::soThang() const noexcept { return soThang_; } int GoiDichVu::soBuoi() const noexcept { return soBuoi_; } double GoiDichVu::giaGoc() const noexcept { return gia_; }
const std::string& GoiDichVu::ma() const noexcept { return ma_; } const std::string& GoiDichVu::ten() const noexcept { return ten_; } LoaiGoi GoiDichVu::kieu() const noexcept { return kieu_; }
int GoiDichVu::soNgay() const noexcept { return soNgay_; } int GoiDichVu::soBuoiPT() const noexcept { return soBuoiPT_; } double GoiDichVu::giaSinhVien() const noexcept { return giaSV_; }
bool GoiDichVu::dangBan() const noexcept { return dangBan_; } void GoiDichVu::datDangBan(bool value) noexcept { dangBan_ = value; }
std::string GoiDichVu::moTaThoiHan() const {
    std::string text;
    if (soThang_ > 0) text += std::to_string(soThang_) + " tháng";
    if (soNgay_ > 0) text += (text.empty() ? "" : " + ") + std::to_string(soNgay_) + " ngày";
    if (text.empty()) text = "Trong ngày";
    if (soBuoi_ > 0) text += ", " + std::to_string(soBuoi_) + " buổi";
    if (soBuoiPT_ > 0) text += ", " + std::to_string(soBuoiPT_) + " buổi PT";
    return text;
}
double GoiDichVu::tinhPhiDichVu(const HoiVien& hv, const ChienLuocGia& strategy, const Voucher* voucher) const { return strategy.tinhPhiDichVu(hv, *this, voucher); }
Date GoiDichVu::tinhNgayHetHan(Date batDau) const {
    if (!batDau.ok()) throw std::invalid_argument("Ngày bắt đầu không hợp lệ.");
    if (soThang_ == 0 && soNgay_ == 0) return batDau; // vé ngày: dùng trong ngày bắt đầu
    // Hạn thẻ tính trọn ngày: gói 1 tháng từ 12/10 dùng đến hết 11/11.
    return addDays(addMonths(batDau, soThang_), soNgay_ - 1);
}
double ChienLuocGia::tinhPhiDichVu(const HoiVien& hv, GoiTap goi, const Voucher* voucher) const { return tinhPhiDichVu(hv, GoiDichVu(goi), voucher); }
std::string GiaTieuChuan::ten() const { return "Giá tiêu chuẩn"; }
double GiaTieuChuan::tinhPhiDichVu(const HoiVien& hv, const GoiDichVu& goi, const Voucher* voucher) const {
    const double base = (hv.laSinhVien() && goi.giaSinhVien() > 0) ? goi.giaSinhVien() : goi.giaGoc();
    return voucher ? voucher->apDung(base) : base;
}
std::string GiaTheoHangThanhVien::ten() const { return "Ưu đãi theo hạng thành viên"; }
double GiaTheoHangThanhVien::tyLeGiamTheoHang(const std::string& hang) noexcept {
    if (hang == "Kim cương") return 0.10;
    if (hang == "Vàng") return 0.05;
    if (hang == "Bạc") return 0.03;
    return 0;
}
double GiaTheoHangThanhVien::tinhPhiDichVu(const HoiVien& hv, const GoiDichVu& goi, const Voucher* voucher) const {
    const double base = GiaTieuChuan{}.tinhPhiDichVu(hv, goi) * (1.0 - tyLeGiamTheoHang(hv.hang()));
    return voucher ? voucher->apDung(base) : base;
}
std::string tenGoi(GoiTap goi) { switch(goi) { case GoiTap::MotNgay:return "1 ngày"; case GoiTap::MotThang:return "1 tháng"; case GoiTap::BaThang:return "3 tháng"; case GoiTap::SauThang:return "6 tháng (5+1)"; case GoiTap::MuoiBaThang:return "13 tháng (12+1)"; } return "Không rõ"; }
std::string maGoiChuan(GoiTap goi) { switch(goi) { case GoiTap::MotNgay:return "GT01"; case GoiTap::MotThang:return "GT02"; case GoiTap::BaThang:return "GT03"; case GoiTap::SauThang:return "GT04"; case GoiTap::MuoiBaThang:return "GT05"; } return "GT00"; }
}
