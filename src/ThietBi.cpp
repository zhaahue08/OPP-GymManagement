#include "ThietBi.h"
#include <stdexcept>
#include <utility>
namespace crm {
ThietBi::ThietBi(std::string ma,std::string ten,Date date):ma_(std::move(ma)),ten_(std::move(ten)),trangThai_("Hoạt động"),ngayBaoTri_(date),khuVuc_("Khu tập chung"),ngayNhap_(today()){if(ma_.empty()||ten_.empty()||!date.ok())throw std::invalid_argument("Thông tin thiết bị không hợp lệ.");}
ThietBi::ThietBi(std::string ma,std::string ten,std::string khuVuc,Date nhap,Date baoHanh,Date baoTri,double giaTri,std::string ncc):ThietBi(std::move(ma),std::move(ten),baoTri){capNhat(ten_,std::move(khuVuc),nhap,baoHanh,giaTri,std::move(ncc));}
void ThietBi::lenLichBaoTri(Date date){if(!date.ok())throw std::invalid_argument("Ngày bảo trì không hợp lệ.");ngayBaoTri_=date;}
void ThietBi::capNhatTrangThai(std::string value){if(value.empty())throw std::invalid_argument("Trạng thái rỗng.");trangThai_=std::move(value);}
void ThietBi::capNhat(std::string ten,std::string khuVuc,Date nhap,Date baoHanh,double giaTri,std::string ncc){if(ten.empty()||!nhap.ok()||giaTri<0||(baoHanh.ok()&&baoHanh<nhap))throw std::invalid_argument("Thông tin thiết bị không hợp lệ.");ten_=std::move(ten);khuVuc_=khuVuc.empty()?"Khu tập chung":std::move(khuVuc);ngayNhap_=nhap;hanBaoHanh_=baoHanh;giaTri_=giaTri;nhaCungCap_=std::move(ncc);}
bool ThietBi::canBaoTri(Date date)const{return date.ok()&&trangThai_!="Thanh lý"&&(ngayBaoTri_<=date||trangThai_=="Hỏng");}
bool ThietBi::conBaoHanh(Date date)const{return hanBaoHanh_.ok()&&date<=hanBaoHanh_;}
Date ThietBi::ngayBaoTri()const noexcept{return ngayBaoTri_;} const std::string& ThietBi::ma()const noexcept{return ma_;} const std::string& ThietBi::ten()const noexcept{return ten_;} const std::string& ThietBi::trangThai()const noexcept{return trangThai_;}
const std::string& ThietBi::khuVuc()const noexcept{return khuVuc_;} const std::string& ThietBi::nhaCungCap()const noexcept{return nhaCungCap_;}
Date ThietBi::ngayNhap()const noexcept{return ngayNhap_;} Date ThietBi::hanBaoHanh()const noexcept{return hanBaoHanh_;} double ThietBi::giaTri()const noexcept{return giaTri_;}
}
