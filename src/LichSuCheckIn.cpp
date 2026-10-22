#include "LichSuCheckIn.h"
#include "HoiVien.h"
#include <algorithm>
#include <istream>
#include <ostream>
#include <stdexcept>
#include <utility>

namespace crm {
namespace { constexpr std::size_t kToiDaBuoi = 100000; }

LichSuCheckIn::LichSuCheckIn(std::string maHoiVien) : maHoiVien_(std::move(maHoiVien)) {
    if (maHoiVien_.empty()) throw std::invalid_argument("Ma hoi vien rong.");
}

LichSuCheckIn LichSuCheckIn::tuHoiVien(const HoiVien& hv) {
    LichSuCheckIn h(hv.ma());
    for (const auto& text : hv.lichSuCheckIn()) h.themNgay(parseDate(text));
    return h;
}

const std::string& LichSuCheckIn::maHoiVien() const noexcept { return maHoiVien_; }
const std::vector<Date>& LichSuCheckIn::cacNgay() const noexcept { return ngay_; }
std::size_t LichSuCheckIn::soBuoi() const noexcept { return ngay_.size(); }

std::size_t LichSuCheckIn::soBuoiTrongThang(std::chrono::year y, std::chrono::month m) const noexcept {
    return static_cast<std::size_t>(std::count_if(ngay_.begin(), ngay_.end(),
        [&](const Date& d) { return d.year() == y && d.month() == m; }));
}

Date LichSuCheckIn::lanCuoi() const {
    if (ngay_.empty()) throw std::logic_error("Chua co buoi check-in nao.");
    return ngay_.back();
}

void LichSuCheckIn::themNgay(Date ngay) {
    if (!ngay.ok()) throw std::invalid_argument("Ngay check-in khong hop le.");
    if (ngay > today()) throw std::invalid_argument("Ngay check-in khong duoc o tuong lai.");
    if (!ngay_.empty() && ngay < ngay_.back()) throw std::invalid_argument("Ngay check-in khong duoc som hon lan truoc.");
    ngay_.push_back(ngay);
}

LichSuCheckIn& LichSuCheckIn::operator++() { themNgay(today()); return *this; }

LichSuCheckIn LichSuCheckIn::operator++(int) { LichSuCheckIn truoc = *this; ++*this; return truoc; }

std::ostream& operator<<(std::ostream& out, const LichSuCheckIn& h) {
    out << h.maHoiVien_ << ' ' << h.ngay_.size();
    for (const auto& d : h.ngay_) out << ' ' << formatDate(d);
    return out;
}

std::istream& operator>>(std::istream& in, LichSuCheckIn& h) {
    std::string ma;
    std::size_t n{};
    if (!(in >> ma >> n)) return in;
    if (n > kToiDaBuoi) { in.setstate(std::ios::failbit); return in; }
    try {
        LichSuCheckIn tam(ma);
        for (std::size_t i = 0; i < n; ++i) {
            std::string text;
            if (!(in >> text)) return in;  // thiếu dữ liệu: stream đã ở trạng thái lỗi
            tam.themNgay(parseDate(text));
        }
        h = std::move(tam);
    } catch (const std::invalid_argument&) {
        in.setstate(std::ios::failbit);
    }
    return in;
}
}
