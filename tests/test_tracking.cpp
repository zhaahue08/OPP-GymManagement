// Kiểm thử module Check-in & InBody (TV4). Chạy: build/test_tracking  -> exit code 0 nếu đạt.
#include "HoiVien.h"
#include "LichSuCheckIn.h"
#include "PhieuInBody.h"
#include <cmath>
#include <iostream>
#include <sstream>

using namespace crm;
using namespace std::chrono;

static int dem = 0, loi = 0;
static Date ngayMai() { return Date{sys_days{today()} + days{1}}; }
#define CHECK(cond) do { ++dem; if (!(cond)) { ++loi; std::cout << "FAIL dong " << __LINE__ << ": " #cond "\n"; } } while (0)
#define CHECK_THROWS(type, expr) do { ++dem; bool b = false; try { (void)(expr); } catch (const type&) { b = true; } \
    if (!b) { ++loi; std::cout << "FAIL dong " << __LINE__ << ": khong nem " #type "\n"; } } while (0)

int main() {
    // ---- InBody: ràng buộc dữ liệu ----
    CHECK_THROWS(std::invalid_argument, InBody(0, 70, 20, 30));      // cao = 0
    CHECK_THROWS(std::invalid_argument, InBody(170, -5, 20, 30));    // cân âm
    CHECK_THROWS(std::invalid_argument, InBody(170, 70, 101, 30));   // mỡ > 100%
    CHECK_THROWS(std::invalid_argument, InBody(170, 70, 20, 80));    // cơ > cân nặng
    CHECK_THROWS(std::invalid_argument, InBody(std::nan(""), 70, 20, 30));
    CHECK_THROWS(std::logic_error, InBody().bmi());                  // chưa có dữ liệu

    const InBody ok(170, 65, 20, 30);
    CHECK(std::abs(ok.bmi() - 22.49) < 0.01);
    CHECK(ok.phanLoaiBMI() == "Binh thuong");
    CHECK(InBody(170, 50, 15, 25).phanLoaiBMI() == "Thieu can");
    CHECK(InBody(170, 70, 25, 30).phanLoaiBMI() == "Thua can");
    CHECK(InBody(170, 90, 30, 35).phanLoaiBMI() == "Beo phi");

    // ---- InBody: toán tử >> ----
    { std::istringstream in("170 65 20 30"); InBody x; CHECK(static_cast<bool>(in >> x)); CHECK(x.canNangKg() == 65); }
    { std::istringstream in("abc 65 20 30"); InBody x; CHECK(!(in >> x)); }                 // sai kiểu
    { std::istringstream in("170 65 200 30"); InBody x(160, 50, 10, 20); CHECK(!(in >> x)); CHECK(x.canNangKg() == 50); } // sai ràng buộc, giữ nguyên x
    { std::ostringstream out; out << ok; CHECK(out.str().find("BMI") != std::string::npos); }

    // ---- PhieuInBody ----
    const Date d1 = parseDate("2026-01-05"), d2 = parseDate("2026-03-05");
    PhieuInBody p1("HV001", d1, InBody(170, 70, 25, 28));
    PhieuInBody p2("HV001", d2, InBody(170, 66, 20, 30));
    CHECK(p1 < p2);
    const auto cl = p2 - p1;
    CHECK(cl.canNangKg == -4 && cl.phanTramMo == -5 && cl.khoiLuongCoKg == 2);
    CHECK_THROWS(std::invalid_argument, p1 - p2);                                           // phiếu trước phải sớm hơn
    CHECK_THROWS(std::invalid_argument, PhieuInBody("HV002", d1, ok) - p1);                 // khác hội viên
    CHECK_THROWS(std::invalid_argument, PhieuInBody("", d1, ok));
    CHECK_THROWS(std::invalid_argument, PhieuInBody("HV001", ngayMai(), ok));       // tương lai
    { // round-trip << rồi >>
        std::ostringstream out; out << p2;
        std::istringstream in(out.str()); PhieuInBody q; CHECK(static_cast<bool>(in >> q));
        CHECK(q.maHoiVien() == "HV001" && q.ngayDo() == d2 && q.chiSo().canNangKg() == 66);
    }
    { std::istringstream in("HV001 2026-13-40 170 65 20 30"); PhieuInBody q; CHECK(!(in >> q)); }  // ngày sai

    // ---- LichSuCheckIn ----
    CHECK_THROWS(std::invalid_argument, LichSuCheckIn(""));
    LichSuCheckIn h("HV001");
    CHECK(h.soBuoi() == 0);
    CHECK_THROWS(std::logic_error, h.lanCuoi());
    h.themNgay(parseDate("2026-01-03"));
    h.themNgay(parseDate("2026-01-10"));
    h.themNgay(parseDate("2026-02-02"));
    CHECK(h.soBuoi() == 3);
    CHECK(h.soBuoiTrongThang(year{2026}, January) == 2);
    CHECK(h.soBuoiTrongThang(year{2026}, February) == 1);
    CHECK(h.lanCuoi() == parseDate("2026-02-02"));
    CHECK_THROWS(std::invalid_argument, h.themNgay(parseDate("2026-01-01")));               // lùi ngày
    CHECK_THROWS(std::invalid_argument, h.themNgay(ngayMai()));                     // tương lai
    CHECK(h.soBuoi() == 3);                                                                 // lỗi không làm hỏng dữ liệu

    const auto truoc = h++;                                                                 // hậu tố trả bản cũ
    CHECK(truoc.soBuoi() == 3 && h.soBuoi() == 4 && h.lanCuoi() == today());
    ++h; CHECK(h.soBuoi() == 5);

    { // round-trip << >>
        std::ostringstream out; out << truoc;
        std::istringstream in(out.str()); LichSuCheckIn r("TAM"); CHECK(static_cast<bool>(in >> r));
        CHECK(r.maHoiVien() == "HV001" && r.soBuoi() == 3 && r.lanCuoi() == parseDate("2026-02-02"));
    }
    { std::istringstream in("HV001 2 2026-01-10 2026-01-03"); LichSuCheckIn r("TAM"); CHECK(!(in >> r)); CHECK(r.maHoiVien() == "TAM"); } // ngày giảm dần
    { std::istringstream in("HV001 3 2026-01-03"); LichSuCheckIn r("TAM"); CHECK(!(in >> r)); }                                          // thiếu dữ liệu

    // ---- Tích hợp với HoiVien (check-in thật qua ++) ----
    HoiVien hv("HV900", "Test", "0900000000", parseDate("2000-01-01"), false);
    hv.kichHoat(addMonths(today(), 1), 5, true);
    ++hv; ++hv;
    const auto ls = LichSuCheckIn::tuHoiVien(hv);
    CHECK(ls.soBuoi() == 2 && ls.maHoiVien() == "HV900" && ls.lanCuoi() == today());
    CHECK(hv.soBuoiConLai() == 3);

    std::cout << (dem - loi) << "/" << dem << " kiem tra dat\n";
    return loi == 0 ? 0 : 1;
}
