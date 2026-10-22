#include "PT.h"
#include <stdexcept>
#include <utility>
namespace crm {
PT::PT(std::string ma,std::string ten,std::string mon):Nguoi(std::move(ma),std::move(ten)),chuyenMon_(std::move(mon)){if(chuyenMon_.empty())throw std::invalid_argument("Chuyen mon PT khong duoc rong.");}
PT::~PT()=default; std::string PT::vaiTro()const{return "Huan luyen vien";} const std::string& PT::chuyenMon()const noexcept{return chuyenMon_;}
void PT::ghiNhanBuoiDay(double doanhThu){if(doanhThu<0)throw std::invalid_argument("Doanh thu khong hop le.");++soCaDaDay_;doanhThuCaNhan_+=doanhThu;}
void PT::chamCong(){++soBuoiChamCong_;} double PT::tinhHoaHong(double tyLe)const{if(tyLe<0||tyLe>1)throw std::invalid_argument("Ty le hoa hong phai tu 0 den 1.");return doanhThuCaNhan_*tyLe;}
int PT::soCaDaDay()const noexcept{return soCaDaDay_;} int PT::soBuoiChamCong()const noexcept{return soBuoiChamCong_;}
}