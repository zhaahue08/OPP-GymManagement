#pragma once
#include "DateUtils.h"
#include <chrono>
#include <cstddef>
#include <iosfwd>
#include <string>
#include <vector>

namespace crm {
class HoiVien;

// Lịch sử check-in của MỘT hội viên. Mỗi lượt quẹt thẻ = 1 buổi tập (cùng quy ước với HoiVien::operator++).
class LichSuCheckIn {
    std::string maHoiVien_;
    std::vector<Date> ngay_;  // luôn theo thứ tự thời gian tăng dần
public:
    explicit LichSuCheckIn(std::string maHoiVien);
    static LichSuCheckIn tuHoiVien(const HoiVien& hv);  // dựng từ HoiVien::lichSuCheckIn()

    const std::string& maHoiVien() const noexcept;
    const std::vector<Date>& cacNgay() const noexcept;
    std::size_t soBuoi() const noexcept;
    std::size_t soBuoiTrongThang(std::chrono::year y, std::chrono::month m) const noexcept;
    Date lanCuoi() const;  // ném logic_error nếu chưa có buổi nào

    void themNgay(Date ngay);  // kiểm tra hợp lệ, không ở tương lai, không lùi so với lần trước

    LichSuCheckIn& operator++();    // ghi 1 buổi tập vào hôm nay
    LichSuCheckIn operator++(int);  // trả bản trước khi tăng

    // Định dạng: MA SO_BUOI d1 d2 ... dn  (<< ghi ra, >> đọc lại được)
    friend std::ostream& operator<<(std::ostream& out, const LichSuCheckIn& h);
    friend std::istream& operator>>(std::istream& in, LichSuCheckIn& h);
};
}
