#include "GiaoDich.h"
#include <stdexcept>
#include <utility>
namespace crm {
GiaoDich::GiaoDich(std::string ma,std::string text,Date date,LoaiGiaoDich type,double amount,std::string group):ma_(std::move(ma)),noiDung_(std::move(text)),ngay_(date),loai_(type),soTien_(amount),danhMuc_(std::move(group)){if(ma_.empty()||noiDung_.empty()||!date.ok()||amount<=0)throw std::invalid_argument("Giao dịch tài chính không hợp lệ.");if(danhMuc_.empty())danhMuc_="Khác";}
const std::string& GiaoDich::ma()const noexcept{return ma_;} const std::string& GiaoDich::noiDung()const noexcept{return noiDung_;} Date GiaoDich::ngay()const noexcept{return ngay_;} LoaiGiaoDich GiaoDich::loai()const noexcept{return loai_;} double GiaoDich::soTien()const noexcept{return soTien_;}
const std::string& GiaoDich::danhMuc()const noexcept{return danhMuc_;}
}
