// Cài đặt phần dữ liệu dùng chung cho hội viên và nhân viên, gồm mã và họ tên.
// Nguoi là lớp cơ sở trừu tượng. Lớp con cung cấp vaiTro() theo kiểu người cụ thể.

#include "Nguoi.h"
#include <stdexcept>
#include <utility>
namespace crm {
Nguoi::Nguoi(std::string ma,std::string ten):ma_(std::move(ma)),ten_(std::move(ten)){if(ma_.empty()||ten_.empty())throw std::invalid_argument("Mã và tên không được rỗng.");}
Nguoi::~Nguoi()=default;
void Nguoi::doiTen(std::string ten){if(ten.empty())throw std::invalid_argument("Tên không được rỗng.");ten_=std::move(ten);}
const std::string& Nguoi::ma()const noexcept{return ma_;} const std::string& Nguoi::ten()const noexcept{return ten_;}
}
