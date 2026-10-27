#pragma once
#include <iosfwd>
namespace crm {
class InBody {
    double chieuCaoCm_{}, canNangKg_{}, phanTramMo_{}, khoiLuongCoKg_{};
public:
    InBody() = default;
    InBody(double cao, double nang, double mo, double co);
    double bmi() const;
    double chieuCao() const noexcept; double canNang() const noexcept; double phanTramMo() const noexcept; double khoiLuongCo() const noexcept;
    const char* danhGiaBMI() const;
    friend std::ostream& operator<<(std::ostream& out, const InBody& data);
    friend std::istream& operator>>(std::istream& in, InBody& data);
};
}
