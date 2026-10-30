#include "DichVuCSKH.h"
#include "LichHen.h"
#include "Voucher.h"
#include <iomanip>
#include <sstream>
#include <stdexcept>
namespace crm {
bool DichVuCSKH::laSinhNhat(Date sinh,Date now)noexcept{return sameDayMonth(sinh,now);}
bool DichVuCSKH::canNhacGiaHan(Date expiry,int before,Date now){if(!expiry.ok()||!now.ok()||before<0)throw std::invalid_argument("Tham số nhắc gia hạn không hợp lệ.");auto left=std::chrono::sys_days{expiry}-std::chrono::sys_days{now};return left.count()>=0&&left.count()<=before;}
std::string DichVuCSKH::taoNhacLichTap(const LichHen& event){const auto day=std::chrono::floor<std::chrono::days>(event.batDau());const auto ymd=Date{day};const auto minutes=std::chrono::duration_cast<std::chrono::minutes>(event.batDau()-day).count();std::ostringstream out;out<<"Lịch "<<event.ma()<<" với PT "<<event.maPT()<<" lúc "<<formatDate(ymd)<<' '<<std::setfill('0')<<std::setw(2)<<minutes/60<<':'<<std::setw(2)<<minutes%60<<" (mã hội viên "<<event.maHoiVien()<<')';return out.str();}
std::string DichVuCSKH::taoThongBaoUuDai(const std::string& member,const std::string& campaign,const Voucher& voucher){if(member.empty()||campaign.empty())throw std::invalid_argument("Thông tin ưu đãi không hợp lệ.");std::ostringstream out;out<<"Hội viên "<<member<<": "<<campaign<<". Nhập mã "<<voucher.ma()<<" để được giảm ";if(voucher.loai()==LoaiGiamGia::PhanTram)out<<voucher.giaTri()<<'%';else out<<std::fixed<<std::setprecision(0)<<voucher.giaTri()<<" VND";return out.str();}
}
