#include "PhieuInBody.h"
#include <istream>
#include <ostream>
#include <sstream>
#include <stdexcept>
#include <utility>

namespace crm {
PhieuInBody::PhieuInBody(std::string maHoiVien, Date ngayDo, InBody chiSo)
    : maHoiVien_(std::move(maHoiVien)), ngayDo_(ngayDo), chiSo_(chiSo) {
    if (maHoiVien_.empty()) throw std::invalid_argument("Ma hoi vien rong.");
    if (!ngayDo_.ok()) throw std::invalid_argument("Ngay do khong hop le.");
    if (ngayDo_ > today()) throw std::invalid_argument("Ngay do khong duoc o tuong lai.");
}

const std::string& PhieuInBody::maHoiVien() const noexcept { return maHoiVien_; }
Date PhieuInBody::ngayDo() const noexcept { return ngayDo_; }
const InBody& PhieuInBody::chiSo() const noexcept { return chiSo_; }
double PhieuInBody::bmi() const { return chiSo_.bmi(); }
std::string PhieuInBody::phanLoaiBMI() const { return chiSo_.phanLoaiBMI(); }

std::string PhieuInBody::tomTat() const {
    std::ostringstream out;
    out << maHoiVien_ << " | " << formatDate(ngayDo_) << " | " << chiSo_.canNangKg() << " kg | mo "
        << chiSo_.phanTramMo() << "% | co " << chiSo_.khoiLuongCoKg() << " kg | BMI "
        << chiSo_.bmi() << " (" << chiSo_.phanLoaiBMI() << ")";
    return out.str();
}

PhieuInBody::ChenhLech PhieuInBody::operator-(const PhieuInBody& truoc) const {
    if (maHoiVien_ != truoc.maHoiVien_) throw std::invalid_argument("Hai phieu InBody khong cung hoi vien.");
    if (ngayDo_ < truoc.ngayDo_) throw std::invalid_argument("Phieu truoc phai co ngay do som hon.");
    return {chiSo_.canNangKg() - truoc.chiSo_.canNangKg(),
            chiSo_.phanTramMo() - truoc.chiSo_.phanTramMo(),
            chiSo_.khoiLuongCoKg() - truoc.chiSo_.khoiLuongCoKg()};
}

bool PhieuInBody::operator<(const PhieuInBody& other) const noexcept { return ngayDo_ < other.ngayDo_; }

std::ostream& operator<<(std::ostream& out, const PhieuInBody& p) {
    return out << p.maHoiVien_ << ' ' << formatDate(p.ngayDo_) << ' ' << p.chiSo_.chieuCaoCm() << ' '
               << p.chiSo_.canNangKg() << ' ' << p.chiSo_.phanTramMo() << ' ' << p.chiSo_.khoiLuongCoKg();
}

std::istream& operator>>(std::istream& in, PhieuInBody& p) {
    std::string ma, ngay;
    InBody chiSo;
    if (in >> ma >> ngay >> chiSo) {
        try { p = PhieuInBody(ma, parseDate(ngay), chiSo); }
        catch (const std::invalid_argument&) { in.setstate(std::ios::failbit); }
    }
    return in;
}
}
