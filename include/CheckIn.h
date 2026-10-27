#pragma once
#include "DateUtils.h"
#include <chrono>
#include <string>
namespace crm {
// Một lượt ra/vào phòng tập (kiểm soát ra vào bằng thẻ từ, mã vạch, vân tay, khuôn mặt hoặc thủ công).
class CheckIn {
    std::string maHoiVien_;
    DateTime vao_, ra_{};
    std::string phuongThuc_, maTu_;
public:
    explicit CheckIn(std::string maHoiVien);
    CheckIn(std::string maHoiVien, DateTime vao, std::string phuongThuc, DateTime ra = {}, std::string maTu = {});
    const std::string& maHoiVien() const noexcept;
    std::chrono::system_clock::time_point thoiGian() const noexcept;
    DateTime vao() const noexcept; DateTime ra() const noexcept; bool daRa() const noexcept;
    int soPhut(DateTime denLuc = now()) const;
    const std::string& phuongThuc() const noexcept;
    const std::string& maTu() const noexcept; void ganTu(std::string maTu);
    void checkOut(DateTime luc = now());
};
}
