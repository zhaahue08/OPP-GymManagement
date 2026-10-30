#include "DongHang.h"
#include <stdexcept>
#include <utility>
namespace crm {
DongHang::DongHang(std::string name,int count,double price,std::string sp):ten(std::move(name)),soLuong(count),donGia(price),maSanPham(std::move(sp)){if(ten.empty()||count<=0||price<0)throw std::invalid_argument("Dòng hàng không hợp lệ.");}
double DongHang::thanhTien()const noexcept{return soLuong*donGia;}
}
