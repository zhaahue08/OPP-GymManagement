#include "InBody.h"
#include <istream>
#include <ostream>
#include <stdexcept>

namespace crm {
InBody::InBody(double cao, double nang, double mo, double co)
    : chieuCaoCm_(cao), canNangKg_(nang), phanTramMo_(mo), khoiLuongCoKg_(co) {
    // Viết dạng !(a && b) để các giá trị NaN cũng bị từ chối.
    if (!(cao >= 50 && cao <= 250))  throw std::invalid_argument("Chieu cao phai trong khoang 50-250 cm.");
    if (!(nang >= 10 && nang <= 300)) throw std::invalid_argument("Can nang phai trong khoang 10-300 kg.");
    if (!(mo >= 0 && mo <= 100))     throw std::invalid_argument("Phan tram mo phai trong khoang 0-100.");
    if (!(co >= 0 && co <= nang))    throw std::invalid_argument("Khoi luong co phai tu 0 den can nang.");
}

double InBody::chieuCaoCm() const noexcept { return chieuCaoCm_; }
double InBody::canNangKg() const noexcept { return canNangKg_; }
double InBody::phanTramMo() const noexcept { return phanTramMo_; }
double InBody::khoiLuongCoKg() const noexcept { return khoiLuongCoKg_; }

double InBody::bmi() const {
    if (chieuCaoCm_ <= 0) throw std::logic_error("Chua co chieu cao.");
    const double m = chieuCaoCm_ / 100;
    return canNangKg_ / (m * m);
}

std::string InBody::phanLoaiBMI() const {
    const double v = bmi();
    if (v < 18.5) return "Thieu can";
    if (v < 23.0) return "Binh thuong";
    if (v < 25.0) return "Thua can";
    return "Beo phi";
}

std::ostream& operator<<(std::ostream& out, const InBody& x) {
    if (x.chieuCaoCm_ <= 0) return out << "(chua co du lieu InBody)";
    return out << "Cao(cm) Can(kg) Mo(%) Co(kg)\n"
               << x.chieuCaoCm_ << ' ' << x.canNangKg_ << ' ' << x.phanTramMo_ << ' ' << x.khoiLuongCoKg_
               << "\nBMI: " << x.bmi();
}

// Chỉ ghi vào x khi cả 4 số hợp lệ; sai kiểu hoặc sai ràng buộc -> failbit, x giữ nguyên.
std::istream& operator>>(std::istream& in, InBody& x) {
    double cao, nang, mo, co;
    if (in >> cao >> nang >> mo >> co) {
        try { x = InBody(cao, nang, mo, co); }
        catch (const std::invalid_argument&) { in.setstate(std::ios::failbit); }
    }
    return in;
}
}
