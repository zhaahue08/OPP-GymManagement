#pragma once
#include <iosfwd>
#include <string>

namespace crm {
// Bộ chỉ số InBody của một lần đo. Ràng buộc dữ liệu kiểm tra ngay trong constructor.
class InBody {
    double chieuCaoCm_{}, canNangKg_{}, phanTramMo_{}, khoiLuongCoKg_{};
public:
    InBody() = default;
    InBody(double cao, double nang, double mo, double co);
    double chieuCaoCm() const noexcept;
    double canNangKg() const noexcept;
    double phanTramMo() const noexcept;
    double khoiLuongCoKg() const noexcept;
    double bmi() const;
    std::string phanLoaiBMI() const;  // theo ngưỡng châu Á - Thái Bình Dương
    friend std::ostream& operator<<(std::ostream& out, const InBody& data);
    friend std::istream& operator>>(std::istream& in, InBody& data);
};
}
