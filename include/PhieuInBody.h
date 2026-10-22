#pragma once
#include "DateUtils.h"
#include "InBody.h"
#include <iosfwd>
#include <string>

namespace crm {
// Phiếu InBody = chỉ số InBody + mã hội viên + ngày đo (composition với InBody).
class PhieuInBody {
    std::string maHoiVien_;
    Date ngayDo_{};
    InBody chiSo_;
public:
    struct ChenhLech { double canNangKg, phanTramMo, khoiLuongCoKg; };

    PhieuInBody() = default;  // cần cho operator>>
    PhieuInBody(std::string maHoiVien, Date ngayDo, InBody chiSo);

    const std::string& maHoiVien() const noexcept;
    Date ngayDo() const noexcept;
    const InBody& chiSo() const noexcept;
    double bmi() const;
    std::string phanLoaiBMI() const;
    std::string tomTat() const;  // dòng mô tả cho người đọc

    // Chênh lệch so với phiếu trước đó của cùng hội viên (dương = tăng).
    ChenhLech operator-(const PhieuInBody& truoc) const;
    bool operator<(const PhieuInBody& other) const noexcept;  // sắp xếp theo ngày đo

    // Định dạng một dòng: MA YYYY-MM-DD cao nang mo co  (<< ghi ra, >> đọc lại được)
    friend std::ostream& operator<<(std::ostream& out, const PhieuInBody& p);
    friend std::istream& operator>>(std::istream& in, PhieuInBody& p);
};
}
