#pragma once
#include "DateUtils.h"
#include <string>
namespace crm {
class KiemTraVeSinh {
    std::string khuVuc_, nguoiKiemTra_, ghiChu_;
    Date ngayKiemTra_{};
    bool datYeuCau_{};
public:
    KiemTraVeSinh(std::string khuVuc,std::string nguoiKiemTra,Date ngay,bool datYeuCau,std::string ghiChu={});
    bool datYeuCau() const noexcept; Date ngayKiemTra() const noexcept;
    const std::string& khuVuc() const noexcept; const std::string& nguoiKiemTra() const noexcept;
    const std::string& ghiChu() const noexcept;
};
}
