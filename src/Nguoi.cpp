#include "Nguoi.h"
#include <stdexcept>
#include <utility>
namespace crm {
Nguoi::Nguoi(std::string ma,std::string ten):ma_(std::move(ma)),ten_(std::move(ten)){if(ma_.empty()||ten_.empty())throw std::invalid_argument("Ma va ten khong duoc rong.");}
Nguoi::~Nguoi()=default;
const std::string& Nguoi::ma()const noexcept{return ma_;} const std::string& Nguoi::ten()const noexcept{return ten_;}
}
