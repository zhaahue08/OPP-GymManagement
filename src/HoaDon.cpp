#include "HoaDon.h"
#include <iomanip>
#include <istream>
#include <ostream>
#include <stdexcept>
#include <utility>
namespace crm {
HoaDon::HoaDon(std::string ma, std::string hv, std::string ngay) : ma_(std::move(ma)), maHoiVien_(std::move(hv)), ngay_(std::move(ngay)) { if(ma_.empty()||maHoiVien_.empty()||ngay_.empty()) throw std::invalid_argument("Thong tin hoa don khong hop le."); }
void HoaDon::themHang(std::string ten, int sl, double gia) { dongHang_.emplace_back(std::move(ten),sl,gia); }
double HoaDon::tongTien() const noexcept { double total=0; for(const auto& d:dongHang_) total += d.soLuong*d.donGia; return total; }
const std::vector<DongHang>& HoaDon::dongHang() const noexcept { return dongHang_; }
const std::string& HoaDon::ma() const noexcept{return ma_;} const std::string& HoaDon::maHoiVien() const noexcept{return maHoiVien_;} const std::string& HoaDon::ngay() const noexcept{return ngay_;}
std::ostream& operator<<(std::ostream& out,const HoaDon& hd) { out<<"Hoa don "<<hd.ma_<<" | HV "<<hd.maHoiVien_<<" | "<<hd.ngay_<<'\n'; for(const auto& d:hd.dongHang_) out<<"- "<<d.ten<<" x"<<d.soLuong<<" @ "<<std::fixed<<std::setprecision(0)<<d.donGia<<" = "<<d.soLuong*d.donGia<<" VND\n"; return out<<"Tong: "<<std::fixed<<std::setprecision(0)<<hd.tongTien()<<" VND\n"; }
std::istream& operator>>(std::istream& in,HoaDon& hd) { std::string ma,hv,date,name; int n; double price; if(!(in>>ma>>hv>>date>>n) || n<0) { in.setstate(std::ios::failbit); return in; } HoaDon tmp(ma,hv,date); for(int i=0;i<n;++i) { if(!(in>>std::quoted(name)>>tmp.dongHang_.emplace_back().soLuong>>price)) { in.setstate(std::ios::failbit); return in; } auto& d=tmp.dongHang_.back(); d.ten=name; d.donGia=price; if(d.soLuong<=0||price<0) { in.setstate(std::ios::failbit); return in; } } hd=std::move(tmp); return in; }
}
