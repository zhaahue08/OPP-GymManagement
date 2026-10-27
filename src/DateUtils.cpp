#include "DateUtils.h"
#include <algorithm>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <stdexcept>

namespace crm {
Date parseDate(const std::string& text) {
    int y{}, m{}, d{}; char a{}, b{}; std::istringstream in(text);
    if (!(in >> y >> a >> m >> b >> d) || a != '-' || b != '-' || in.peek() != std::char_traits<char>::eof())
        throw std::invalid_argument("Ngày phải theo định dạng YYYY-MM-DD.");
    if (m < 1 || m > 12 || d < 1 || d > 31) throw std::invalid_argument("Ngày không hợp lệ.");
    Date result{std::chrono::year{y}, std::chrono::month{static_cast<unsigned>(m)}, std::chrono::day{static_cast<unsigned>(d)}};
    if (!result.ok()) throw std::invalid_argument("Ngày không hợp lệ.");
    return result;
}
std::string formatDate(const Date& date) {
    if (!date.ok()) throw std::invalid_argument("Ngày không hợp lệ.");
    std::ostringstream out;
    out << std::setfill('0') << std::setw(4) << int(date.year()) << '-' << std::setw(2) << unsigned(date.month()) << '-' << std::setw(2) << unsigned(date.day());
    return out.str();
}
DateTime now() {
    // system_clock là giờ UTC; quy đổi về giờ địa phương để "hôm nay" không lệch múi giờ (VN là UTC+7).
    const std::time_t t = std::time(nullptr);
    std::tm local{};
#ifdef _WIN32
    localtime_s(&local, &t);
#else
    localtime_r(&t, &local);
#endif
    const Date day{std::chrono::year{local.tm_year + 1900}, std::chrono::month{static_cast<unsigned>(local.tm_mon + 1)}, std::chrono::day{static_cast<unsigned>(local.tm_mday)}};
    return std::chrono::sys_days{day} + std::chrono::hours{local.tm_hour} + std::chrono::minutes{local.tm_min} + std::chrono::seconds{local.tm_sec};
}
Date today() { return ngayCua(now()); }
Date addMonths(Date date, int months) {
    if (!date.ok() || months < 0) throw std::invalid_argument("Ngày hoặc số tháng không hợp lệ.");
    const Date first{date.year(), date.month(), std::chrono::day{1}};
    const auto target = first.year()/first.month() + std::chrono::months{months};
    const std::chrono::year_month_day_last last{target.year(), std::chrono::month_day_last{target.month()}};
    const auto lastDay = unsigned(last.day());
    return {target.year(), target.month(), std::chrono::day{std::min(unsigned(date.day()), lastDay)}};
}
Date addDays(Date date, int days) {
    if (!date.ok()) throw std::invalid_argument("Ngày không hợp lệ.");
    return Date{std::chrono::sys_days{date} + std::chrono::days{days}};
}
bool sameDayMonth(const Date& a, const Date& b) noexcept { return a.ok() && b.ok() && a.month() == b.month() && a.day() == b.day(); }
int soNgayGiua(Date tu, Date den) {
    if (!tu.ok() || !den.ok()) throw std::invalid_argument("Ngày không hợp lệ.");
    return static_cast<int>((std::chrono::sys_days{den} - std::chrono::sys_days{tu}).count());
}
int tinhTuoi(Date sinh, Date ngay) {
    if (!sinh.ok() || !ngay.ok()) return 0;
    int age = int(ngay.year()) - int(sinh.year());
    if (ngay.month() < sinh.month() || (ngay.month() == sinh.month() && ngay.day() < sinh.day())) --age;
    return std::max(age, 0);
}
int thuTrongTuan(Date ngay) { return static_cast<int>(std::chrono::weekday{std::chrono::sys_days{ngay}}.c_encoding()); }
DateTime parseDateTime(const std::string& text) {
    const auto space = text.find(' ');
    const Date day = parseDate(text.substr(0, space));
    int h = 0, m = 0, s = 0;
    if (space != std::string::npos) {
        char c1{}, c2{}; std::istringstream in(text.substr(space + 1));
        if (!(in >> h >> c1 >> m) || c1 != ':') throw std::invalid_argument("Giờ phải theo định dạng HH:MM.");
        if (in >> c2 && c2 == ':') in >> s;
        if (h < 0 || h > 23 || m < 0 || m > 59 || s < 0 || s > 59) throw std::invalid_argument("Giờ không hợp lệ.");
    }
    return std::chrono::sys_days{day} + std::chrono::hours{h} + std::chrono::minutes{m} + std::chrono::seconds{s};
}
std::string formatTime(DateTime time) {
    const auto day = std::chrono::floor<std::chrono::days>(time);
    const auto minutes = std::chrono::duration_cast<std::chrono::minutes>(time - day).count();
    std::ostringstream out;
    out << std::setfill('0') << std::setw(2) << minutes / 60 << ':' << std::setw(2) << minutes % 60;
    return out.str();
}
std::string formatDateTime(DateTime time) { return formatDate(ngayCua(time)) + ' ' + formatTime(time); }
DateTime taoThoiDiem(Date ngay, int gio, int phut) {
    if (!ngay.ok() || gio < 0 || gio > 23 || phut < 0 || phut > 59) throw std::invalid_argument("Thời điểm không hợp lệ.");
    return std::chrono::sys_days{ngay} + std::chrono::hours{gio} + std::chrono::minutes{phut};
}
Date ngayCua(DateTime time) { return Date{std::chrono::floor<std::chrono::days>(time)}; }
int gioCua(DateTime time) {
    const auto day = std::chrono::floor<std::chrono::days>(time);
    return static_cast<int>(std::chrono::duration_cast<std::chrono::hours>(time - day).count());
}
std::string khoaKy(Date ngay, KyBaoCao ky) {
    std::ostringstream out; out << std::setfill('0');
    switch (ky) {
        case KyBaoCao::Ngay: return formatDate(ngay);
        case KyBaoCao::Tuan: {
            // Tuần theo ISO 8601: tuần chứa ngày thứ Năm quyết định năm.
            const std::chrono::sys_days d{ngay};
            const int iso = static_cast<int>(std::chrono::weekday{d}.iso_encoding());
            const Date thursday{d + std::chrono::days{4 - iso}};
            const std::chrono::sys_days jan1{thursday.year() / std::chrono::January / 1};
            const int week = static_cast<int>((std::chrono::sys_days{thursday} - jan1).count() / 7 + 1);
            out << int(thursday.year()) << "-T" << std::setw(2) << week; return out.str();
        }
        case KyBaoCao::Thang: out << int(ngay.year()) << '-' << std::setw(2) << unsigned(ngay.month()); return out.str();
        case KyBaoCao::Quy: out << int(ngay.year()) << "-Q" << (unsigned(ngay.month()) - 1) / 3 + 1; return out.str();
        case KyBaoCao::Nam: out << int(ngay.year()); return out.str();
    }
    return formatDate(ngay);
}
std::string tenKy(KyBaoCao ky) {
    switch (ky) { case KyBaoCao::Ngay: return "Ngày"; case KyBaoCao::Tuan: return "Tuần"; case KyBaoCao::Thang: return "Tháng"; case KyBaoCao::Quy: return "Quý"; case KyBaoCao::Nam: return "Năm"; }
    return "Kỳ";
}
}
