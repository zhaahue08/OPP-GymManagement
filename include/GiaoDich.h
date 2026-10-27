#pragma once
#include "DateUtils.h"
#include <string>
namespace crm {
enum class LoaiGiaoDich { Thu, Chi };
class GiaoDich {
    std::string ma_, noiDung_;
    Date ngay_{};
    LoaiGiaoDich loai_;
    double soTien_;
    std::string danhMuc_;
public:
    GiaoDich(std::string ma,std::string noiDung,Date ngay,LoaiGiaoDich loai,double soTien,std::string danhMuc = "Khác");
    const std::string& ma() const noexcept; const std::string& noiDung() const noexcept;
    Date ngay() const noexcept; LoaiGiaoDich loai() const noexcept; double soTien() const noexcept;
    const std::string& danhMuc() const noexcept;
};
}
