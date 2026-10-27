#include "PT.h"
#include <stdexcept>
#include <utility>
namespace crm {
PT::PT(std::string ma,std::string ten,std::string mon):NhanVien(std::move(ma),std::move(ten),"Huấn luyện viên"),chuyenMon_(std::move(mon)){if(chuyenMon_.empty())throw std::invalid_argument("Chuyên môn PT không được rỗng.");}
PT::PT(std::string ma, std::string ten, std::string mon, std::string sdt, double luong, double donGia, double tyLe, int gioBD, int gioKT, double giaHV)
    : NhanVien(std::move(ma), std::move(ten), "Huấn luyện viên", std::move(sdt), luong, tyLe) {
    capNhatPT(Nguoi::ten(), std::move(mon), NhanVien::sdt(), luong, donGia, tyLe, gioBD, gioKT, giaHV);
}
PT::~PT()=default; std::string PT::vaiTro()const{return "Huấn luyện viên";} const std::string& PT::chuyenMon()const noexcept{return chuyenMon_;}
void PT::ghiNhanBuoiDay(double doanhThu){if(doanhThu<0)throw std::invalid_argument("Doanh thu không hợp lệ.");++soCaDaDay_;doanhThuCaNhan_+=doanhThu;}
void PT::chamCong(){++soBuoiChamCong_;} double PT::tinhHoaHong(double tyLe)const{if(tyLe<0||tyLe>1)throw std::invalid_argument("Tỷ lệ hoa hồng phải từ 0 đến 1.");return doanhThuCaNhan_*tyLe;}
int PT::soCaDaDay()const noexcept{return soCaDaDay_;} int PT::soBuoiChamCong()const noexcept{return soBuoiChamCong_;}
double PT::doanhThuCaNhan()const noexcept{return doanhThuCaNhan_;}
double PT::donGiaBuoi()const noexcept{return donGiaBuoi_;} double PT::giaBuoiChoHoiVien()const noexcept{return giaBuoiChoHoiVien_;}
int PT::gioBatDau()const noexcept{return gioBatDau_;} int PT::gioKetThuc()const noexcept{return gioKetThuc_;}
void PT::capNhatPT(std::string ten, std::string mon, std::string sdt, double luong, double donGia, double tyLe, int gioBD, int gioKT, double giaHV) {
    if (mon.empty()) throw std::invalid_argument("Chuyên môn PT không được rỗng.");
    if (donGia < 0 || giaHV < 0 || gioBD < 0 || gioKT > 24 || gioBD >= gioKT) throw std::invalid_argument("Đơn giá hoặc ca làm của PT không hợp lệ.");
    capNhat(std::move(ten), "Huấn luyện viên", std::move(sdt), luong, tyLe);
    chuyenMon_ = std::move(mon); donGiaBuoi_ = donGia; gioBatDau_ = gioBD; gioKetThuc_ = gioKT; giaBuoiChoHoiVien_ = giaHV;
}
void PT::napThongKe(int soCa, double doanhThu, int soChamCong) {
    if (soCa < 0 || doanhThu < 0 || soChamCong < 0) throw std::invalid_argument("Thống kê PT không hợp lệ.");
    soCaDaDay_ = soCa; doanhThuCaNhan_ = doanhThu; soBuoiChamCong_ = soChamCong;
}
bool PT::trongCaLam(int tu, int den) const noexcept { return tu >= gioBatDau_ * 60 && den <= gioKetThuc_ * 60 && tu < den; }
double PT::tinhLuong(const KyLuong& ky) const {
    // Lương cứng theo công + hoa hồng doanh số + thù lao từng buổi dạy + % doanh thu buổi PT.
    return NhanVien::tinhLuong(ky) + ky.soCaPT * donGiaBuoi_ + ky.doanhThuPT * tyLeHoaHong_;
}
}
