#pragma once
#include "DateUtils.h"
#include <string>
namespace crm {
class ThietBi {
    std::string ma_, ten_, trangThai_;
    Date ngayBaoTri_{};
    std::string khuVuc_, nhaCungCap_;
    Date ngayNhap_{}, hanBaoHanh_{};
    double giaTri_{};
public:
    ThietBi(std::string ma,std::string ten,Date ngayBaoTri);
    ThietBi(std::string ma,std::string ten,std::string khuVuc,Date ngayNhap,Date hanBaoHanh,Date ngayBaoTri,double giaTri,std::string nhaCungCap = {});
    void lenLichBaoTri(Date ngay); void capNhatTrangThai(std::string trangThai);
    void capNhat(std::string ten,std::string khuVuc,Date ngayNhap,Date hanBaoHanh,double giaTri,std::string nhaCungCap);
    bool canBaoTri(Date ngay = today()) const;
    bool conBaoHanh(Date ngay = today()) const;
    Date ngayBaoTri() const noexcept;
    const std::string& ma() const noexcept; const std::string& ten() const noexcept; const std::string& trangThai() const noexcept;
    const std::string& khuVuc() const noexcept; const std::string& nhaCungCap() const noexcept;
    Date ngayNhap() const noexcept; Date hanBaoHanh() const noexcept; double giaTri() const noexcept;
};
}
