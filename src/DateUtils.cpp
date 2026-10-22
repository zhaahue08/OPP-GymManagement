#include "DateUtils.h"
#include <algorithm>
#include <iomanip>
#include <sstream>
#include <stdexcept>

namespace crm {
Date parseDate(const std::string& text) {
    int y{}, m{}, d{}; char a{}, b{}; std::istringstream in(text);
    if (!(in >> y >> a >> m >> b >> d) || a != '-' || b != '-' || in.peek() != std::char_traits<char>::eof())
        throw std::invalid_argument("Ngay phai theo dinh dang YYYY-MM-DD.");
    Date result{std::chrono::year{y}, std::chrono::month{static_cast<unsigned>(m)}, std::chrono::day{static_cast<unsigned>(d)}};
    if (!result.ok()) throw std::invalid_argument("Ngay khong hop le.");
    return result;
}
std::string formatDate(const Date& date) {
    if (!date.ok()) throw std::invalid_argument("Ngay khong hop le.");
    std::ostringstream out;
    out << std::setfill('0') << std::setw(4) << int(date.year()) << '-' << std::setw(2) << unsigned(date.month()) << '-' << std::setw(2) << unsigned(date.day());
    return out.str();
}
Date today() { return std::chrono::year_month_day{std::chrono::floor<std::chrono::days>(std::chrono::system_clock::now())}; }
Date addMonths(Date date, int months) {
    if (!date.ok() || months < 0) throw std::invalid_argument("Ngay hoac so thang khong hop le.");
    const Date first{date.year(), date.month(), std::chrono::day{1}};
    const auto target = first.year()/first.month() + std::chrono::months{months};
    const std::chrono::year_month_day_last last{target.year(), std::chrono::month_day_last{target.month()}};
    const auto lastDay = unsigned(last.day());
    return {target.year(), target.month(), std::chrono::day{std::min(unsigned(date.day()), lastDay)}};
}
bool sameDayMonth(const Date& a, const Date& b) noexcept { return a.ok() && b.ok() && a.month() == b.month() && a.day() == b.day(); }
}