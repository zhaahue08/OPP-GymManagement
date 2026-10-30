#include "InBody.h"
#include <istream>
#include <ostream>
#include <stdexcept>
namespace crm {
InBody::InBody(double cao,double nang,double mo,double co):chieuCaoCm_(cao),canNangKg_(nang),phanTramMo_(mo),khoiLuongCoKg_(co){if(cao<=0||nang<=0||mo<0||mo>100||co<0||co>nang)throw std::invalid_argument("Chỉ số InBody không hợp lệ.");}
double InBody::bmi() const { if(chieuCaoCm_<=0)throw std::logic_error("Chưa có chiều cao."); const double m=chieuCaoCm_/100; return canNangKg_/(m*m); }
double InBody::chieuCao()const noexcept{return chieuCaoCm_;} double InBody::canNang()const noexcept{return canNangKg_;} double InBody::phanTramMo()const noexcept{return phanTramMo_;} double InBody::khoiLuongCo()const noexcept{return khoiLuongCoKg_;}
const char* InBody::danhGiaBMI() const { const double v=bmi(); if(v<18.5)return "Thiếu cân"; if(v<23)return "Bình thường"; if(v<25)return "Thừa cân"; return "Béo phì"; }
std::ostream& operator<<(std::ostream& out,const InBody& x){return out<<"Cao(cm) Can(kg) Mo(%) Co(kg)\n"<<x.chieuCaoCm_<<' '<<x.canNangKg_<<' '<<x.phanTramMo_<<' '<<x.khoiLuongCoKg_<<"\nBMI: "<<x.bmi();}
std::istream& operator>>(std::istream& in,InBody& x){double a,b,c,d;if(in>>a>>b>>c>>d){try{x=InBody(a,b,c,d);}catch(...){in.setstate(std::ios::failbit);}}return in;}
}
