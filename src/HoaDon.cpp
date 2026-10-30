#include "HoaDon.h"
#include <algorithm>
#include <iomanip>
#include <istream>
#include <ostream>
#include <stdexcept>
#include <utility>
namespace crm {
HoaDon::HoaDon(std::string ma, std::string hv, std::string ngay) : ma_(std::move(ma)), maHoiVien_(std::move(hv)), ngay_(std::move(ngay)) { if(ma_.empty()||maHoiVien_.empty()||ngay_.empty()) throw std::invalid_argument("Thông tin hóa đơn không hợp lệ."); }
void HoaDon::themHang(std::string ten, int sl, double gia, std::string sp) { dongHang_.emplace_back(std::move(ten),sl,gia,std::move(sp)); }
double HoaDon::tamTinh() const noexcept { double total=0; for(const auto& d:dongHang_) total += d.thanhTien(); return total; }
double HoaDon::tongTien() const noexcept { return std::max(0.0, tamTinh() - giamGia_); }
const std::vector<DongHang>& HoaDon::dongHang() const noexcept { return dongHang_; }
const std::string& HoaDon::ma() const noexcept{return ma_;} const std::string& HoaDon::maHoiVien() const noexcept{return maHoiVien_;} const std::string& HoaDon::ngay() const noexcept{return ngay_;}
const std::string& HoaDon::loai() const noexcept{return loai_;} void HoaDon::datLoai(std::string loai){if(loai.empty())throw std::invalid_argument("Loại hóa đơn rỗng.");loai_=std::move(loai);}
const std::string& HoaDon::phuongThuc() const noexcept{return phuongThuc_;}
const std::string& HoaDon::maNhanVien() const noexcept{return maNhanVien_;} void HoaDon::datNhanVien(std::string ma){maNhanVien_=std::move(ma);}
const std::string& HoaDon::ghiChu() const noexcept{return ghiChu_;} void HoaDon::datGhiChu(std::string text){ghiChu_=std::move(text);}
double HoaDon::giamGia() const noexcept{return giamGia_;}
void HoaDon::datGiamGia(double soTien){if(soTien<0||soTien>tamTinh())throw std::invalid_argument("Số tiền giảm giá không hợp lệ.");giamGia_=soTien;}
void HoaDon::ghiNhanThanhToan(double daTra, std::string cach) {
    if (daTra < 0 || daTra > tongTien() + 0.5) throw std::invalid_argument("Số tiền thanh toán phải từ 0 đến tổng hóa đơn.");
    if (!cach.empty()) phuongThuc_ = std::move(cach);
    daThanhToan_ = std::min(daTra, tongTien()); coCongNo_ = daThanhToan_ + 0.5 < tongTien();
}
void HoaDon::thuNo(double soTien, std::string cach) {
    if (!coCongNo_) throw std::logic_error("Hóa đơn này không còn công nợ.");
    if (soTien <= 0 || soTien > conNo() + 0.5) throw std::invalid_argument("Số tiền thu nợ không hợp lệ.");
    daThanhToan_ = std::min(tongTien(), daThanhToan_ + soTien);
    if (!cach.empty()) phuongThuc_ = std::move(cach);
    coCongNo_ = daThanhToan_ + 0.5 < tongTien();
}
double HoaDon::daThanhToan() const noexcept { return coCongNo_ ? daThanhToan_ : tongTien(); }
double HoaDon::conNo() const noexcept { return coCongNo_ ? std::max(0.0, tongTien() - daThanhToan_) : 0.0; }
bool HoaDon::coCongNo() const noexcept { return coCongNo_; }
std::ostream& operator<<(std::ostream& out,const HoaDon& hd) {
    out<<"Hóa đơn "<<hd.ma_<<" | HV "<<hd.maHoiVien_<<" | "<<hd.ngay_<<'\n';
    for(const auto& d:hd.dongHang_) out<<"- "<<d.ten<<" x"<<d.soLuong<<" @ "<<std::fixed<<std::setprecision(0)<<d.donGia<<" = "<<d.thanhTien()<<" VND\n";
    if(hd.giamGia_>0) out<<"Giảm giá: "<<std::fixed<<std::setprecision(0)<<hd.giamGia_<<" VND\n";
    out<<"Tổng: "<<std::fixed<<std::setprecision(0)<<hd.tongTien()<<" VND\n";
    out<<"Đã thanh toán: "<<hd.daThanhToan()<<" VND ("<<hd.phuongThuc_<<")\n";
    if(hd.conNo()>0) out<<"Còn nợ: "<<hd.conNo()<<" VND\n";
    return out;
}
std::istream& operator>>(std::istream& in,HoaDon& hd) { std::string ma,hv,date,name; int n; double price; if(!(in>>ma>>hv>>date>>n) || n<0) { in.setstate(std::ios::failbit); return in; } HoaDon tmp(ma,hv,date); for(int i=0;i<n;++i) { if(!(in>>std::quoted(name)>>tmp.dongHang_.emplace_back().soLuong>>price)) { in.setstate(std::ios::failbit); return in; } auto& d=tmp.dongHang_.back(); d.ten=name; d.donGia=price; if(d.soLuong<=0||price<0) { in.setstate(std::ios::failbit); return in; } } hd=std::move(tmp); return in; }
}
