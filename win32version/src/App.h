#pragma once
// Khai báo dùng chung cho giao diện Win32: chuỗi, DPI, màu, form nhập liệu, trang nghiệp vụ.
#ifndef NOMINMAX
#define NOMINMAX
#endif
#ifndef UNICODE
#define UNICODE
#endif
#ifndef _UNICODE
#define _UNICODE
#endif
#include <windows.h>
#include <commctrl.h>
#include "BaoCao.h"
#include "HethongCRM.h"
#include <filesystem>
#include <functional>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace gui {
// ===== Chuỗi & định dạng =====
std::wstring W(const std::string& utf8);
std::string U(const std::wstring& text);
std::wstring tien(double soTien);              // 1.234.000
std::wstring ngay(crm::Date d);                // 26/10/2026, rỗng thì "—"
std::wstring luc(crm::DateTime t);             // 26/10/2026 07:30
std::wstring gio(crm::DateTime t);             // 07:30
std::wstring chuThuong(std::wstring text);
std::string maTu(const std::wstring& luaChon); // "HV001 — Nguyễn A" -> "HV001"
std::wstring sao(int diem);                    // ★★★☆☆
double docSo(const std::wstring& text);        // "1.234.000" -> 1234000, "4,5" -> 4.5
std::wstring thapPhan(double v, int chuSo = 1); // 4.5 -> "4,5"

// ===== DPI, màu, phông =====
int S(int px);
void khoiTaoDPI();
namespace mau {
inline constexpr COLORREF nen = RGB(247, 248, 250), the = RGB(255, 255, 255), vien = RGB(216, 221, 228);
inline constexpr COLORREF sidebar = RGB(241, 243, 246), sidebarChon = RGB(224, 234, 249), sidebarChu = RGB(57, 67, 82);
inline constexpr COLORREF chinh = RGB(35, 87, 157), chinhNhat = RGB(228, 238, 252), chu = RGB(29, 37, 49), phu = RGB(82, 93, 110);
inline constexpr COLORREF xanh = RGB(37, 99, 235), xanhNhat = RGB(219, 234, 254), cam = RGB(217, 119, 6), camNhat = RGB(254, 243, 199);
inline constexpr COLORREF doDam = RGB(220, 38, 38), doNhat = RGB(254, 226, 226), xanhLa = RGB(22, 163, 74), xanhLaNhat = RGB(220, 252, 231);
inline constexpr COLORREF tim = RGB(124, 58, 237), timNhat = RGB(237, 233, 254), xam = RGB(156, 163, 175), dongChan = RGB(249, 250, 252);
}
HFONT font(int px, bool dam = false, const wchar_t* ten = L"Segoe UI");
HFONT fontIcon(int px);

// ===== Vẽ GDI =====
void toNen(HDC dc, RECT r, COLORREF mau);
void veKhung(HDC dc, RECT r, COLORREF nen, COLORREF vien, int bo);
void veChu(HDC dc, RECT r, const std::wstring& text, HFONT f, COLORREF mau, UINT flags = DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS);
void veKPI(HDC dc, RECT r, const std::wstring& nhan, const std::wstring& giaTri, const std::wstring& phu, COLORREF mauNhan, COLORREF mauNen);
void veBieuDoCot(HDC dc, RECT r, const std::wstring& tieuDe, const std::vector<std::wstring>& nhan, const std::vector<double>& giaTri, COLORREF mau, bool laTien);

// ===== Hộp thoại =====
enum class Kieu { Chu, So, Tien, Ngay, Gio, Chon, ChonSua, Check, NhieuDong, MatKhau, ChiDoc };
struct Truong {
    std::wstring nhan;
    Kieu kieu = Kieu::Chu;
    std::wstring giaTri;
    std::vector<std::wstring> luaChon;
    bool batBuoc = false;
    std::wstring goiY;
};
class KetQua {
    const std::vector<Truong>& t_;
public:
    explicit KetQua(const std::vector<Truong>& t) : t_(t) {}
    std::string chu(std::size_t i) const;
    std::wstring wchu(std::size_t i) const;
    bool rong(std::size_t i) const;
    int soNguyen(std::size_t i, int macDinh = 0) const;
    double so(std::size_t i, double macDinh = 0) const;
    crm::Date ngay(std::size_t i) const;
    int gioPhut(std::size_t i) const; // số phút từ 0h
    int chon(std::size_t i) const;
    std::string ma(std::size_t i) const;
    bool check(std::size_t i) const;
};
// Ném HuyBo trong hàm xử lý để giữ form mở mà không báo lỗi (ví dụ người dùng bấm "Không" ở bước xác nhận).
struct HuyBo {};
bool moForm(HWND owner, const std::wstring& tieuDe, std::vector<Truong>& truong, const std::function<void(const KetQua&)>& xuLy,
            const std::wstring& nutOK = L"Lưu", const std::wstring& moTa = {});
void xemVanBan(HWND owner, const std::wstring& tieuDe, const std::wstring& noiDung, const std::wstring& tenFile = {});
void thongBao(const std::wstring& text, const std::wstring& tieuDe = L"Thông báo");
void loi(const std::wstring& text);
bool hoi(const std::wstring& text, const std::wstring& tieuDe = L"Xác nhận");
bool chonFileLuu(HWND owner, std::wstring& path, const wchar_t* filter, const wchar_t* ext, const std::wstring& tenMacDinh);
bool chonFileMo(HWND owner, std::wstring& path, const wchar_t* filter);
void ghiFileUtf8(const std::wstring& path, const std::wstring& noiDung);
void saoChep(HWND owner, const std::wstring& text);

// ===== Trang nghiệp vụ =====
enum Quyen : unsigned { Q_ADMIN = 1, Q_QUANLY = 2, Q_LETAN = 4, Q_SALE = 8, Q_PT = 16, Q_TATCA = 31 };
struct Cot { std::wstring ten; int rong; bool so = false; };
using Dong = std::vector<std::wstring>;
struct ThaoTac {
    std::wstring nhan;
    std::function<void()> chay;
    std::vector<int> boLoc;  // rỗng = hiện ở mọi bộ lọc
    bool chiQuanLy = false;
    bool chinh = false;      // nút nhấn mạnh
};
struct Trang {
    std::wstring ten, bieuTuong, moTa;
    unsigned quyen = Q_TATCA;
    std::vector<std::wstring> boLoc;
    std::function<std::vector<Cot>(int)> cot;
    std::function<std::vector<Dong>(int)> dong;
    std::function<COLORREF(const Dong&, int)> mauChu;
    std::function<std::wstring(int)> phuDe;
    std::vector<ThaoTac> thaoTac;
    std::function<int(int)> chieuCaoVe;
    std::function<void(HDC, RECT, int)> ve;
    std::function<void(int)> moDong; // nhấp đúp
};

struct UngDung {
    crm::HethongCRM crm;
    std::string tenDangNhap, maNhanVien;
    crm::VaiTro vaiTro = crm::VaiTro::LeTan;
    std::filesystem::path thuMuc;
    HWND chinh = nullptr;
    bool thayDoi = false;
    std::vector<std::pair<std::string, int>> gioHang;
};
UngDung& app();
crm::HethongCRM& C();
std::string chon();                    // khóa (cột đầu) của dòng đang chọn, ném lỗi nếu chưa chọn
std::string chonHoacRong();
int boLocHienTai();
const std::vector<Dong>& dongHienTai();
void lamMoi();
void daThayDoi();
bool laQuanLy();
unsigned quyenHienTai();
void luuDuLieu(bool imLang);

std::vector<std::wstring> dsHoiVien(bool chiKichHoat = false);
std::vector<std::wstring> dsPT(bool coRong = false);
std::vector<std::wstring> dsNhanSu(bool coRong = false);
std::vector<std::wstring> dsGoi();
std::vector<std::wstring> dsSanPham();
std::wstring timTrongDs(const std::vector<std::wstring>& ds, const std::string& ma);
std::wstring vanBanHoaDon(const crm::HoaDon& hd);
std::wstring tenHoiVien(const std::string& ma);
std::wstring tenNhanSu(const std::string& ma);

void themTrangTongQuan(std::vector<Trang>& ds);
void themTrangHoiVien(std::vector<Trang>& ds);
void themTrangDichVu(std::vector<Trang>& ds);
void themTrangNhanSu(std::vector<Trang>& ds);
void themTrangTaiChinh(std::vector<Trang>& ds);
void themTrangVanHanh(std::vector<Trang>& ds);
void themTrangBaoCao(std::vector<Trang>& ds);
void themTrangCSKH(std::vector<Trang>& ds);
void themTrangHeThong(std::vector<Trang>& ds);
inline std::vector<std::wstring> dsThanhToan() { return {L"Tiền mặt", L"Chuyển khoản", L"Thẻ ngân hàng", L"Ví điện tử"}; }
inline std::vector<std::wstring> dsLoaiThe() { return {L"(Chưa gán)", L"Thẻ từ", L"Mã vạch", L"Vân tay", L"Khuôn mặt"}; }
// Mở hộp quét thẻ (dùng ở Tổng quan và Check-in).
void moQuetThe();
// Mở form đăng ký/gia hạn gói cho hội viên.
void moDangKyGoi(const std::string& maHV);
}
