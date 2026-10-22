#include "LichHen.h"
#include <stdexcept>
#include <utility>
namespace crm {
LichHen::LichHen(std::string ma,std::string hv,std::string pt,std::chrono::sys_seconds start,std::chrono::sys_seconds end):ma_(std::move(ma)),maHoiVien_(std::move(hv)),maPT_(std::move(pt)),batDau_(start),ketThuc_(end){if(ma_.empty()||maHoiVien_.empty()||maPT_.empty()||end<=start)throw std::invalid_argument("Khung gio lich hen khong hop le.");}
const std::string& LichHen::ma()const noexcept{return ma_;} const std::string& LichHen::maHoiVien()const noexcept{return maHoiVien_;} const std::string& LichHen::maPT()const noexcept{return maPT_;}
std::chrono::sys_seconds LichHen::batDau()const noexcept{return batDau_;} std::chrono::sys_seconds LichHen::ketThuc()const noexcept{return ketThuc_;}
bool LichHen::trungLich(const LichHen& x)const noexcept{return maPT_==x.maPT_&&batDau_<x.ketThuc_&&x.batDau_<ketThuc_;}
}