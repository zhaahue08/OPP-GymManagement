#pragma once
#include "DateUtils.h"
#include <string>
namespace crm {
// Tủ đồ thông minh: gán theo lượt khi check-in hoặc cho thuê theo tháng.
class TuDo {
    std::string ma_, khuVuc_, trangThai_{"Trống"}, maHoiVien_;
    DateTime batDau_{};
    Date hetHanThue_{};
public:
    TuDo(std::string ma, std::string khuVuc);
    const std::string& ma() const noexcept; const std::string& khuVuc() const noexcept;
    const std::string& trangThai() const noexcept; const std::string& maHoiVien() const noexcept;
    DateTime batDau() const noexcept; Date hetHanThue() const noexcept;
    bool trong() const noexcept;
    void ganTheoLuot(std::string maHoiVien, DateTime luc = now());
    void choThue(std::string maHoiVien, Date hetHan);
    void traTu();
    void baoHong(); void suaXong();
    void napTrangThai(std::string trangThai, std::string maHoiVien, DateTime batDau, Date hetHan);
};
}
