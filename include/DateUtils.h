#pragma once
#include <chrono>
#include <string>

namespace crm {
using Date = std::chrono::year_month_day;
// Thời điểm theo giờ địa phương (giờ trên đồng hồ máy), lưu dưới dạng sys_seconds.
using DateTime = std::chrono::sys_seconds;
enum class KyBaoCao { Ngay, Tuan, Thang, Quy, Nam };

Date parseDate(const std::string& text);
std::string formatDate(const Date& date);
Date today();
Date addMonths(Date date, int months);
Date addDays(Date date, int days);
bool sameDayMonth(const Date& a, const Date& b) noexcept;
int soNgayGiua(Date tu, Date den);
int tinhTuoi(Date ngaySinh, Date ngay = today());
int thuTrongTuan(Date ngay); // 0 = Chủ nhật ... 6 = Thứ bảy

DateTime now();
DateTime parseDateTime(const std::string& text);
std::string formatDateTime(DateTime time);
std::string formatTime(DateTime time);
DateTime taoThoiDiem(Date ngay, int gio, int phut);
Date ngayCua(DateTime time);
int gioCua(DateTime time);
std::string khoaKy(Date ngay, KyBaoCao ky);
std::string tenKy(KyBaoCao ky);
}
