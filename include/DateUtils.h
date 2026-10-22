#pragma once
#include <chrono>
#include <string>

namespace crm {
using Date = std::chrono::year_month_day;
Date parseDate(const std::string& text);
std::string formatDate(const Date& date);
Date today();
Date addMonths(Date date, int months);
bool sameDayMonth(const Date& a, const Date& b) noexcept;
}