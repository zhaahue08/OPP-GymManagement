#include "LichHen.h"
#include <stdexcept>
#include <utility>
namespace crm {
LichHen::LichHen(std::string ma,std::string hv,std::string pt,std::chrono::sys_seconds start,std::chrono::sys_seconds end):ma_(std::move(ma)),maHoiVien_(std::move(hv)),maPT_(std::move(pt)),batDau_(start),ketThuc_(end){if(ma_.empty()||maHoiVien_.empty()||maPT_.empty()||end<=start)throw std::invalid_argument("Khung giờ lịch hẹn không hợp lệ.");}
const std::string& LichHen::ma()const noexcept{return ma_;} const std::string& LichHen::maHoiVien()const noexcept{return maHoiVien_;} const std::string& LichHen::maPT()const noexcept{return maPT_;}
std::chrono::sys_seconds LichHen::batDau()const noexcept{return batDau_;} std::chrono::sys_seconds LichHen::ketThuc()const noexcept{return ketThuc_;}
bool LichHen::trungLich(const LichHen& x)const noexcept{return dangHoatDong()&&x.dangHoatDong()&&maPT_==x.maPT_&&batDau_<x.ketThuc_&&x.batDau_<ketThuc_;}
bool LichHen::trungLichHoiVien(const LichHen& x)const noexcept{return dangHoatDong()&&x.dangHoatDong()&&maHoiVien_==x.maHoiVien_&&batDau_<x.ketThuc_&&x.batDau_<ketThuc_;}
int LichHen::thoiLuongPhut()const noexcept{return static_cast<int>(std::chrono::duration_cast<std::chrono::minutes>(ketThuc_-batDau_).count());}
const std::string& LichHen::trangThai()const noexcept{return trangThai_;} const std::string& LichHen::ghiChu()const noexcept{return ghiChu_;}
bool LichHen::dangHoatDong()const noexcept{return trangThai_!="Đã hủy";}
void LichHen::hoanThanh(){if(trangThai_!="Đã đặt")throw std::logic_error("Chỉ lịch đang đặt mới được hoàn thành.");trangThai_="Hoàn thành";}
void LichHen::huy(std::string lyDo){if(trangThai_!="Đã đặt")throw std::logic_error("Chỉ lịch đang đặt mới được hủy.");trangThai_="Đã hủy";ghiChu_=std::move(lyDo);}
void LichHen::danhDauVang(){if(trangThai_!="Đã đặt")throw std::logic_error("Chỉ lịch đang đặt mới được đánh dấu vắng.");trangThai_="Vắng mặt";}
void LichHen::napTrangThai(std::string trangThai,std::string ghiChu){if(trangThai!="Đã đặt"&&trangThai!="Hoàn thành"&&trangThai!="Đã hủy"&&trangThai!="Vắng mặt")throw std::invalid_argument("Trạng thái lịch hẹn không hợp lệ.");trangThai_=std::move(trangThai);ghiChu_=std::move(ghiChu);}
}
