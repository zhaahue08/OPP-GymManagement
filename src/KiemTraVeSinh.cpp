#include "KiemTraVeSinh.h"
#include <stdexcept>
#include <utility>
namespace crm {
KiemTraVeSinh::KiemTraVeSinh(std::string area,std::string checker,Date day,bool ok,std::string note):khuVuc_(std::move(area)),nguoiKiemTra_(std::move(checker)),ghiChu_(std::move(note)),ngayKiemTra_(day),datYeuCau_(ok){if(khuVuc_.empty()||nguoiKiemTra_.empty()||!day.ok())throw std::invalid_argument("Biên bản vệ sinh không hợp lệ.");}
bool KiemTraVeSinh::datYeuCau()const noexcept{return datYeuCau_;} Date KiemTraVeSinh::ngayKiemTra()const noexcept{return ngayKiemTra_;} const std::string& KiemTraVeSinh::khuVuc()const noexcept{return khuVuc_;} const std::string& KiemTraVeSinh::nguoiKiemTra()const noexcept{return nguoiKiemTra_;} const std::string& KiemTraVeSinh::ghiChu()const noexcept{return ghiChu_;}
}
