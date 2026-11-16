// Tiện ích giao diện: chuyển chuỗi, DPI, phông, vẽ GDI, FormDialog dựng control bằng CreateWindowExW,
// hộp xem văn bản, hộp chọn file (comdlg32) và clipboard.
#include "App.h"
#include <commdlg.h>
#include <algorithm>
#include <cmath>
#include <cwctype>
#include <fstream>
#include <map>
#include <sstream>
#include <tuple>

namespace gui {
namespace { int g_dpi = 96; }

std::wstring W(const std::string& s) {
    if (s.empty()) return {};
    const int n = MultiByteToWideChar(CP_UTF8, 0, s.data(), static_cast<int>(s.size()), nullptr, 0);
    std::wstring out(static_cast<std::size_t>(n), L'\0');
    MultiByteToWideChar(CP_UTF8, 0, s.data(), static_cast<int>(s.size()), out.data(), n);
    return out;
}
std::string U(const std::wstring& s) {
    if (s.empty()) return {};
    const int n = WideCharToMultiByte(CP_UTF8, 0, s.data(), static_cast<int>(s.size()), nullptr, 0, nullptr, nullptr);
    std::string out(static_cast<std::size_t>(n), '\0');
    WideCharToMultiByte(CP_UTF8, 0, s.data(), static_cast<int>(s.size()), out.data(), n, nullptr, nullptr);
    return out;
}
std::wstring tien(double v) {
    const bool am = v < 0;
    long long n = std::llround(std::fabs(v));
    std::wstring digits = std::to_wstring(n), out;
    int count = 0;
    for (auto it = digits.rbegin(); it != digits.rend(); ++it) { if (count && count % 3 == 0) out.insert(out.begin(), L'.'); out.insert(out.begin(), *it); ++count; }
    return am ? L"-" + out : out;
}
std::wstring ngay(crm::Date d) {
    if (!d.ok()) return L"—";
    wchar_t buf[16]; swprintf(buf, 16, L"%02u/%02u/%04d", unsigned(d.day()), unsigned(d.month()), int(d.year()));
    return buf;
}
std::wstring gio(crm::DateTime t) { return t.time_since_epoch().count() == 0 ? L"—" : W(crm::formatTime(t)); }
std::wstring luc(crm::DateTime t) { return t.time_since_epoch().count() == 0 ? L"—" : ngay(crm::ngayCua(t)) + L" " + gio(t); }
std::wstring chuThuong(std::wstring s) { if (!s.empty()) CharLowerBuffW(s.data(), static_cast<DWORD>(s.size())); return s; }
std::string maTu(const std::wstring& s) { const auto p = s.find(L' '); return U(p == std::wstring::npos ? s : s.substr(0, p)); }
std::wstring sao(int d) { std::wstring s; for (int i = 1; i <= 5; ++i) s += i <= d ? L'★' : L'☆'; return s; }

void khoiTaoDPI() { HDC dc = GetDC(nullptr); g_dpi = GetDeviceCaps(dc, LOGPIXELSY); ReleaseDC(nullptr, dc); if (g_dpi <= 0) g_dpi = 96; }
int S(int px) { return MulDiv(px, g_dpi, 96); }
HFONT font(int px, bool dam, const wchar_t* ten) {
    static std::map<std::tuple<int, bool, std::wstring>, HFONT> cache;
    auto key = std::make_tuple(px, dam, std::wstring(ten));
    if (auto it = cache.find(key); it != cache.end()) return it->second;
    HFONT f = CreateFontW(-S(px), 0, 0, 0, dam ? FW_SEMIBOLD : FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH, ten);
    cache.emplace(key, f);
    return f;
}
HFONT fontIcon(int px) { return font(px, false, L"Segoe MDL2 Assets"); }

// ===== Vẽ =====
void toNen(HDC dc, RECT r, COLORREF c) { HBRUSH b = CreateSolidBrush(c); FillRect(dc, &r, b); DeleteObject(b); }
void veKhung(HDC dc, RECT r, COLORREF nen, COLORREF vien, int bo) {
    HPEN p = CreatePen(PS_SOLID, 1, vien); HBRUSH b = CreateSolidBrush(nen);
    auto op = SelectObject(dc, p); auto ob = SelectObject(dc, b);
    RoundRect(dc, r.left, r.top, r.right, r.bottom, bo, bo);
    SelectObject(dc, op); SelectObject(dc, ob); DeleteObject(p); DeleteObject(b);
}
void veChu(HDC dc, RECT r, const std::wstring& text, HFONT f, COLORREF c, UINT flags) {
    SetBkMode(dc, TRANSPARENT); SetTextColor(dc, c);
    auto old = SelectObject(dc, f);
    DrawTextW(dc, text.c_str(), static_cast<int>(text.size()), &r, flags | DT_NOPREFIX);
    SelectObject(dc, old);
}
void veKPI(HDC dc, RECT r, const std::wstring& nhan, const std::wstring& giaTri, const std::wstring& phu, COLORREF mauNhan, COLORREF mauNen) {
    veKhung(dc, r, mau::the, mau::vien, S(4));
    toNen(dc, {r.left + 1, r.top + S(8), r.left + S(4), r.bottom - S(8)}, mauNen);
    const int x = r.left + S(14);
    veChu(dc, {x, r.top + S(12), r.right - S(10), r.top + S(30)}, nhan, font(12, true), mau::phu);
    // Số dài (VD doanh thu hàng trăm triệu) thì thu nhỏ cỡ chữ thay vì cắt bớt.
    int co = 21;
    for (; co > 13; co -= 2) {
        SIZE sz{}; auto old = SelectObject(dc, font(co, true));
        GetTextExtentPoint32W(dc, giaTri.c_str(), static_cast<int>(giaTri.size()), &sz); SelectObject(dc, old);
        if (sz.cx <= r.right - S(10) - x) break;
    }
    veChu(dc, {x, r.top + S(30), r.right - S(10), r.top + S(58)}, giaTri, font(co, true), mau::chu);
    veChu(dc, {x, r.top + S(58), r.right - S(10), r.bottom - S(6)}, phu, font(11), mauNhan);
}
namespace {
std::wstring gonTien(double v) {
    wchar_t buf[32];
    const double a = std::fabs(v);
    if (a >= 1e9) swprintf(buf, 32, L"%.1f tỷ", v / 1e9);
    else if (a >= 1e6) swprintf(buf, 32, L"%.1ftr", v / 1e6);
    else if (a >= 1e3) swprintf(buf, 32, L"%.0fk", v / 1e3);
    else swprintf(buf, 32, L"%.0f", v);
    return buf;
}
}
void veBieuDoCot(HDC dc, RECT r, const std::wstring& tieuDe, const std::vector<std::wstring>& nhan, const std::vector<double>& giaTri, COLORREF c, bool laTien) {
    veKhung(dc, r, mau::the, mau::vien, S(14));
    veChu(dc, {r.left + S(18), r.top + S(10), r.right - S(18), r.top + S(34)}, tieuDe, font(13, true), mau::chu);
    RECT plot{r.left + S(52), r.top + S(46), r.right - S(18), r.bottom - S(34)};
    if (plot.right - plot.left < S(40) || plot.bottom - plot.top < S(30)) return;
    double maxV = 0; for (double v : giaTri) maxV = std::max(maxV, v);
    if (giaTri.empty() || maxV <= 0) { veChu(dc, plot, L"Chưa có dữ liệu", font(13), mau::xam, DT_CENTER | DT_VCENTER | DT_SINGLELINE); return; }
    HPEN grid = CreatePen(PS_SOLID, 1, mau::vien); auto op = SelectObject(dc, grid);
    for (int i = 0; i <= 4; ++i) {
        const int y = plot.bottom - (plot.bottom - plot.top) * i / 4;
        MoveToEx(dc, plot.left, y, nullptr); LineTo(dc, plot.right, y);
        const double v = maxV * i / 4;
        veChu(dc, {r.left + S(4), y - S(8), plot.left - S(6), y + S(8)}, laTien ? gonTien(v) : W(std::to_string(static_cast<long long>(std::llround(v)))), font(10), mau::phu, DT_RIGHT | DT_VCENTER | DT_SINGLELINE);
    }
    SelectObject(dc, op); DeleteObject(grid);
    const int n = static_cast<int>(giaTri.size());
    const double slot = double(plot.right - plot.left) / n;
    const int barW = std::max(S(4), std::min(S(46), static_cast<int>(slot * 0.62)));
    HBRUSH b = CreateSolidBrush(c); HPEN none = CreatePen(PS_NULL, 0, 0);
    auto ob = SelectObject(dc, b); op = SelectObject(dc, none);
    const int labelStep = std::max(1, static_cast<int>(std::ceil(S(44) / slot)));
    for (int i = 0; i < n; ++i) {
        const int cx = plot.left + static_cast<int>(slot * i + slot / 2);
        const int h = static_cast<int>((plot.bottom - plot.top) * giaTri[static_cast<std::size_t>(i)] / maxV);
        if (h > 0) RoundRect(dc, cx - barW / 2, plot.bottom - h, cx + barW / 2, plot.bottom + S(3), S(6), S(6));
        if (slot >= S(30) && giaTri[static_cast<std::size_t>(i)] > 0)
            veChu(dc, {cx - S(40), plot.bottom - h - S(17), cx + S(40), plot.bottom - h - S(1)}, laTien ? gonTien(giaTri[static_cast<std::size_t>(i)]) : W(std::to_string(static_cast<long long>(std::llround(giaTri[static_cast<std::size_t>(i)])))), font(10, true), mau::chu, DT_CENTER | DT_BOTTOM | DT_SINGLELINE);
        if (i % labelStep == 0 && static_cast<std::size_t>(i) < nhan.size())
            veChu(dc, {cx - static_cast<int>(slot * labelStep / 2), plot.bottom + S(6), cx + static_cast<int>(slot * labelStep / 2), r.bottom - S(8)}, nhan[static_cast<std::size_t>(i)], font(10), mau::phu, DT_CENTER | DT_TOP | DT_SINGLELINE | DT_END_ELLIPSIS);
    }
    SelectObject(dc, ob); SelectObject(dc, op); DeleteObject(b); DeleteObject(none);
}

// ===== KetQua =====
namespace {
std::wstring catKhoang(const std::wstring& s) {
    const auto a = s.find_first_not_of(L" \t\r\n"); if (a == std::wstring::npos) return {};
    const auto b = s.find_last_not_of(L" \t\r\n"); return s.substr(a, b - a + 1);
}
}
std::string KetQua::chu(std::size_t i) const { return U(wchu(i)); }
std::wstring KetQua::wchu(std::size_t i) const { return t_.at(i).giaTri; }
bool KetQua::rong(std::size_t i) const { return catKhoang(t_.at(i).giaTri).empty(); }
int KetQua::soNguyen(std::size_t i, int macDinh) const { return static_cast<int>(std::llround(so(i, macDinh))); }
double KetQua::so(std::size_t i, double macDinh) const {
    std::wstring s = catKhoang(t_.at(i).giaTri);
    if (s.empty()) return macDinh;
    std::wstring clean;
    if (t_.at(i).kieu == Kieu::Tien) { for (wchar_t c : s) if (iswdigit(c) || c == L'-') clean += c; }
    else { for (wchar_t c : s) if (iswdigit(c) || c == L'-' || c == L'.' || c == L',') clean += (c == L',' ? L'.' : c); }
    try { std::size_t used = 0; double v = std::stod(clean, &used); if (used != clean.size() || !std::isfinite(v)) throw 1; return v; }
    catch (...) { throw std::invalid_argument(U(L"Trường \"" + t_.at(i).nhan + L"\" phải là số.")); }
}
crm::Date KetQua::ngay(std::size_t i) const { return rong(i) ? crm::Date{} : crm::parseDate(chu(i)); }
int KetQua::gioPhut(std::size_t i) const { const auto t = chu(i); if (t.size() < 4) return 0; return std::stoi(t.substr(0, 2)) * 60 + std::stoi(t.substr(3, 2)); }
int KetQua::chon(std::size_t i) const {
    const auto& f = t_.at(i);
    for (std::size_t k = 0; k < f.luaChon.size(); ++k) if (f.luaChon[k] == f.giaTri) return static_cast<int>(k);
    return -1;
}
std::string KetQua::ma(std::size_t i) const { return maTu(t_.at(i).giaTri); }
bool KetQua::check(std::size_t i) const { return t_.at(i).giaTri == L"1"; }

// ===== Thông báo =====
void thongBao(const std::wstring& text, const std::wstring& tieuDe) { MessageBoxW(GetActiveWindow(), text.c_str(), tieuDe.c_str(), MB_OK | MB_ICONINFORMATION); }
void loi(const std::wstring& text) { MessageBoxW(GetActiveWindow(), text.c_str(), L"Không thực hiện được", MB_OK | MB_ICONWARNING); }
bool hoi(const std::wstring& text, const std::wstring& tieuDe) { return MessageBoxW(GetActiveWindow(), text.c_str(), tieuDe.c_str(), MB_YESNO | MB_ICONQUESTION) == IDYES; }

// ===== FormDialog =====
namespace {
constexpr int ID_CTL = 1000;
struct FormCtx {
    std::vector<Truong>* truong{};
    const std::function<void(const KetQua&)>* xuLy{};
    std::vector<HWND> ctl;
    HWND owner{};
    std::wstring tieuDe, moTa;
    int headerH{};
    bool ok{}, xong{};
};
std::wstring dongWindows(const std::wstring& s) { std::wstring out; for (wchar_t c : s) { if (c == L'\n') out += L'\r'; out += c; } return out; }
std::wstring layText(HWND h) { const int n = GetWindowTextLengthW(h); std::wstring s(static_cast<std::size_t>(n) + 1, L'\0'); GetWindowTextW(h, s.data(), n + 1); s.resize(static_cast<std::size_t>(n)); return s; }
void dongForm(HWND hwnd, FormCtx* ctx) { ctx->xong = true; if (ctx->owner) EnableWindow(ctx->owner, TRUE); DestroyWindow(hwnd); }
void docGiaTri(FormCtx* ctx) {
    auto& t = *ctx->truong;
    for (std::size_t i = 0; i < t.size(); ++i) {
        HWND h = ctx->ctl[i];
        switch (t[i].kieu) {
            case Kieu::Ngay: {
                SYSTEMTIME st{};
                if (SendMessageW(h, DTM_GETSYSTEMTIME, 0, reinterpret_cast<LPARAM>(&st)) == GDT_VALID) {
                    wchar_t buf[16]; swprintf(buf, 16, L"%04u-%02u-%02u", st.wYear, st.wMonth, st.wDay); t[i].giaTri = buf;
                } else t[i].giaTri.clear();
                break;
            }
            case Kieu::Gio: {
                SYSTEMTIME st{}; SendMessageW(h, DTM_GETSYSTEMTIME, 0, reinterpret_cast<LPARAM>(&st));
                wchar_t buf[8]; swprintf(buf, 8, L"%02u:%02u", st.wHour, st.wMinute); t[i].giaTri = buf; break;
            }
            case Kieu::Check: t[i].giaTri = SendMessageW(h, BM_GETCHECK, 0, 0) == BST_CHECKED ? L"1" : L"0"; break;
            case Kieu::Chon: { const auto idx = SendMessageW(h, CB_GETCURSEL, 0, 0); t[i].giaTri = idx >= 0 && static_cast<std::size_t>(idx) < t[i].luaChon.size() ? t[i].luaChon[static_cast<std::size_t>(idx)] : L""; break; }
            case Kieu::ChiDoc: break;
            default: t[i].giaTri = catKhoang(layText(h)); break;
        }
    }
}
void xacNhan(HWND hwnd, FormCtx* ctx) {
    docGiaTri(ctx);
    auto& t = *ctx->truong;
    KetQua kq(t);
    for (std::size_t i = 0; i < t.size(); ++i) {
        if (t[i].batBuoc && t[i].kieu != Kieu::ChiDoc && t[i].kieu != Kieu::Check && kq.rong(i)) {
            MessageBoxW(hwnd, (L"Vui lòng nhập \"" + t[i].nhan + L"\".").c_str(), ctx->tieuDe.c_str(), MB_OK | MB_ICONWARNING);
            SetFocus(ctx->ctl[i]); return;
        }
        if ((t[i].kieu == Kieu::So || t[i].kieu == Kieu::Tien) && !kq.rong(i)) {
            try { (void)kq.so(i); } catch (const std::exception& e) { MessageBoxW(hwnd, W(e.what()).c_str(), ctx->tieuDe.c_str(), MB_OK | MB_ICONWARNING); SetFocus(ctx->ctl[i]); return; }
        }
    }
    try {
        (*ctx->xuLy)(kq);
        ctx->ok = true;
        dongForm(hwnd, ctx);
    } catch (const HuyBo&) {
    } catch (const std::exception& e) {
        MessageBoxW(hwnd, W(e.what()).c_str(), L"Không thực hiện được", MB_OK | MB_ICONWARNING);
    }
}
LRESULT CALLBACK formProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    auto* ctx = reinterpret_cast<FormCtx*>(GetWindowLongPtrW(hwnd, GWLP_USERDATA));
    switch (msg) {
        case WM_NCCREATE: SetWindowLongPtrW(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(reinterpret_cast<CREATESTRUCTW*>(lp)->lpCreateParams)); break;
        case WM_COMMAND:
            if (!ctx) break;
            if (LOWORD(wp) == IDOK) { xacNhan(hwnd, ctx); return 0; }
            if (LOWORD(wp) == IDCANCEL) { dongForm(hwnd, ctx); return 0; }
            break;
        case DM_GETDEFID: return MAKELRESULT(IDOK, DC_HASDEFID);
        case WM_CLOSE: if (ctx) dongForm(hwnd, ctx); return 0;
        case WM_CTLCOLORSTATIC: {
            HDC dc = reinterpret_cast<HDC>(wp); SetBkColor(dc, mau::the); SetTextColor(dc, mau::chu);
            static HBRUSH white = CreateSolidBrush(mau::the); return reinterpret_cast<LRESULT>(white);
        }
        case WM_ERASEBKGND: return 1;
        case WM_PAINT: {
            PAINTSTRUCT ps; HDC dc = BeginPaint(hwnd, &ps);
            RECT c; GetClientRect(hwnd, &c);
            toNen(dc, c, mau::the);
            if (ctx) {
                const int titleBottom = ctx->moTa.empty() ? ctx->headerH : S(38);
                veChu(dc, {S(22), S(8), c.right - S(16), titleBottom}, ctx->tieuDe, font(17, true), mau::chu);
                if (!ctx->moTa.empty()) {
                    veChu(dc, {S(22), S(38), c.right - S(16), ctx->headerH - S(6)}, ctx->moTa, font(12), mau::phu);
                }
                toNen(dc, {S(22), ctx->headerH - 1, c.right - S(22), ctx->headerH}, mau::vien);
                RECT line{0, c.bottom - S(58), c.right, c.bottom - S(57)}; toNen(dc, line, mau::vien);
            }
            EndPaint(hwnd, &ps); return 0;
        }
    }
    return DefWindowProcW(hwnd, msg, wp, lp);
}
void dangKyLop(const wchar_t* ten, WNDPROC proc) {
    WNDCLASSW wc{}; wc.lpfnWndProc = proc; wc.hInstance = GetModuleHandleW(nullptr); wc.lpszClassName = ten;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW); wc.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
    wc.hIcon = LoadIconW(GetModuleHandleW(nullptr), MAKEINTRESOURCEW(1));
    RegisterClassW(&wc);
}
void datNgay(HWND h, const std::wstring& value) {
    SYSTEMTIME st{}; GetLocalTime(&st);
    if (!value.empty()) {
        try { const auto d = crm::parseDate(U(value)); st.wYear = static_cast<WORD>(int(d.year())); st.wMonth = static_cast<WORD>(unsigned(d.month())); st.wDay = static_cast<WORD>(unsigned(d.day())); }
        catch (...) {}
    }
    SendMessageW(h, DTM_SETSYSTEMTIME, GDT_VALID, reinterpret_cast<LPARAM>(&st));
}
HWND taoNut(HWND parent, int id, const wchar_t* text, int x, int y, int w, int h, bool macDinh) {
    HWND b = CreateWindowExW(0, L"BUTTON", text, WS_CHILD | WS_VISIBLE | WS_TABSTOP | (macDinh ? BS_DEFPUSHBUTTON : BS_PUSHBUTTON), x, y, w, h, parent, reinterpret_cast<HMENU>(static_cast<INT_PTR>(id)), GetModuleHandleW(nullptr), nullptr);
    SendMessageW(b, WM_SETFONT, reinterpret_cast<WPARAM>(font(13)), TRUE);
    return b;
}
void chayModal(HWND hwnd, HWND owner, const bool& xong) {
    MSG msg{};
    while (!xong) {
        const BOOL r = GetMessageW(&msg, nullptr, 0, 0);
        if (r <= 0) { if (r == 0) PostQuitMessage(static_cast<int>(msg.wParam)); break; }
        if (!IsWindow(hwnd) || !IsDialogMessageW(hwnd, &msg)) { TranslateMessage(&msg); DispatchMessageW(&msg); }
    }
    if (owner) { EnableWindow(owner, TRUE); SetForegroundWindow(owner); }
}
RECT viTriGiua(HWND owner, int w, int h) {
    RECT area{};
    if (owner && IsWindowVisible(owner)) GetWindowRect(owner, &area); else SystemParametersInfoW(SPI_GETWORKAREA, 0, &area, 0);
    RECT work{}; SystemParametersInfoW(SPI_GETWORKAREA, 0, &work, 0);
    int x = area.left + (area.right - area.left - w) / 2, y = area.top + (area.bottom - area.top - h) / 2;
    x = std::clamp(x, static_cast<int>(work.left), std::max(static_cast<int>(work.left), static_cast<int>(work.right) - w));
    y = std::clamp(y, static_cast<int>(work.top), std::max(static_cast<int>(work.top), static_cast<int>(work.bottom) - h));
    return {x, y, x + w, y + h};
}
}

bool moForm(HWND owner, const std::wstring& tieuDe, std::vector<Truong>& truong, const std::function<void(const KetQua&)>& xuLy, const std::wstring& nutOK, const std::wstring& moTa) {
    static bool registered = false;
    if (!registered) { dangKyLop(L"CRMForm", formProc); registered = true; }
    FormCtx ctx; ctx.truong = &truong; ctx.xuLy = &xuLy; ctx.owner = owner; ctx.tieuDe = tieuDe; ctx.moTa = moTa; ctx.headerH = S(moTa.empty() ? 46 : 62);
    const int labelW = S(168), ctlW = S(340), pad = S(22), gapY = S(10), colGap = S(26);
    auto caoTruong = [&](const Truong& t) {
        if (t.kieu == Kieu::NhieuDong) return S(74);
        if (t.kieu == Kieu::ChiDoc && t.giaTri.find(L'\n') != std::wstring::npos) return S(124);
        return S(28);
    };
    int total = 0; for (const auto& t : truong) total += caoTruong(t) + gapY;
    RECT work{}; SystemParametersInfoW(SPI_GETWORKAREA, 0, &work, 0);
    const int maxBody = (work.bottom - work.top) - ctx.headerH - S(150);
    const int cols = total > maxBody ? 2 : 1;
    std::vector<int> colOf(truong.size()), yOf(truong.size());
    int colH[2]{0, 0};
    {
        int col = 0, y = 0; const int limit = cols == 2 ? (total + 1) / 2 : total + 1;
        for (std::size_t i = 0; i < truong.size(); ++i) {
            const int h = caoTruong(truong[i]) + gapY;
            if (cols == 2 && col == 0 && y + h > limit && y > 0) { col = 1; y = 0; }
            colOf[i] = col; yOf[i] = y; y += h; colH[col] = y;
        }
    }
    const int clientW = pad * 2 + cols * (labelW + ctlW) + (cols - 1) * colGap;
    const int clientH = ctx.headerH + S(18) + std::max(colH[0], colH[1]) + S(64);
    const DWORD style = WS_POPUP | WS_CAPTION | WS_SYSMENU, ex = WS_EX_DLGMODALFRAME;
    RECT wr{0, 0, clientW, clientH}; AdjustWindowRectEx(&wr, style, FALSE, ex);
    const RECT pos = viTriGiua(owner, wr.right - wr.left, wr.bottom - wr.top);
    HWND hwnd = CreateWindowExW(ex, L"CRMForm", tieuDe.c_str(), style, pos.left, pos.top, pos.right - pos.left, pos.bottom - pos.top, owner, nullptr, GetModuleHandleW(nullptr), &ctx);
    if (!hwnd) return false;
    HINSTANCE inst = GetModuleHandleW(nullptr);
    HWND firstFocus = nullptr;
    for (std::size_t i = 0; i < truong.size(); ++i) {
        auto& t = truong[i];
        const int x = pad + colOf[i] * (labelW + ctlW + colGap), y = ctx.headerH + S(18) + yOf[i], h = caoTruong(t);
        const std::wstring label = t.nhan + (t.batBuoc ? L" *" : L"");
        HWND lb = CreateWindowExW(0, L"STATIC", label.c_str(), WS_CHILD | WS_VISIBLE | SS_LEFT | SS_NOPREFIX, x, y + S(5), labelW - S(10), S(20), hwnd, nullptr, inst, nullptr);
        SendMessageW(lb, WM_SETFONT, reinterpret_cast<WPARAM>(font(13)), TRUE);
        const int cx = x + labelW;
        HWND c = nullptr;
        const int id = ID_CTL + static_cast<int>(i);
        auto menu = reinterpret_cast<HMENU>(static_cast<INT_PTR>(id));
        switch (t.kieu) {
            case Kieu::Ngay:
                c = CreateWindowExW(0, DATETIMEPICK_CLASSW, L"", WS_CHILD | WS_VISIBLE | WS_TABSTOP | DTS_SHORTDATEFORMAT | (t.batBuoc ? 0 : DTS_SHOWNONE), cx, y, ctlW, h, hwnd, menu, inst, nullptr);
                SendMessageW(c, DTM_SETFORMATW, 0, reinterpret_cast<LPARAM>(L"dd/MM/yyyy"));
                datNgay(c, t.giaTri);
                if (!t.batBuoc && t.giaTri.empty()) SendMessageW(c, DTM_SETSYSTEMTIME, GDT_NONE, 0);
                break;
            case Kieu::Gio: {
                c = CreateWindowExW(0, DATETIMEPICK_CLASSW, L"", WS_CHILD | WS_VISIBLE | WS_TABSTOP | DTS_TIMEFORMAT | DTS_UPDOWN, cx, y, S(120), h, hwnd, menu, inst, nullptr);
                SendMessageW(c, DTM_SETFORMATW, 0, reinterpret_cast<LPARAM>(L"HH:mm"));
                SYSTEMTIME st{}; GetLocalTime(&st); st.wSecond = 0;
                if (t.giaTri.size() >= 5) { st.wHour = static_cast<WORD>(std::stoi(t.giaTri.substr(0, 2))); st.wMinute = static_cast<WORD>(std::stoi(t.giaTri.substr(3, 2))); }
                SendMessageW(c, DTM_SETSYSTEMTIME, GDT_VALID, reinterpret_cast<LPARAM>(&st));
                break;
            }
            case Kieu::Chon:
            case Kieu::ChonSua: {
                c = CreateWindowExW(0, L"COMBOBOX", L"", WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_VSCROLL | (t.kieu == Kieu::Chon ? CBS_DROPDOWNLIST : CBS_DROPDOWN | CBS_AUTOHSCROLL), cx, y, ctlW, S(320), hwnd, menu, inst, nullptr);
                int sel = -1;
                for (std::size_t k = 0; k < t.luaChon.size(); ++k) {
                    SendMessageW(c, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(t.luaChon[k].c_str()));
                    if (sel < 0 && !t.giaTri.empty() && (t.luaChon[k] == t.giaTri || maTu(t.luaChon[k]) == U(t.giaTri))) sel = static_cast<int>(k);
                }
                if (t.kieu == Kieu::Chon) SendMessageW(c, CB_SETCURSEL, sel < 0 ? 0 : sel, 0);
                else if (sel >= 0) SendMessageW(c, CB_SETCURSEL, sel, 0); else SetWindowTextW(c, t.giaTri.c_str());
                break;
            }
            case Kieu::Check:
                c = CreateWindowExW(0, L"BUTTON", L"Có", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_AUTOCHECKBOX, cx, y, ctlW, h, hwnd, menu, inst, nullptr);
                SendMessageW(c, BM_SETCHECK, t.giaTri == L"1" ? BST_CHECKED : BST_UNCHECKED, 0);
                break;
            case Kieu::NhieuDong:
                c = CreateWindowExW(WS_EX_CLIENTEDGE, L"EDIT", dongWindows(t.giaTri).c_str(), WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_MULTILINE | ES_WANTRETURN | ES_AUTOVSCROLL | WS_VSCROLL, cx, y, ctlW, h, hwnd, menu, inst, nullptr);
                break;
            case Kieu::ChiDoc: {
                const bool multi = t.giaTri.find(L'\n') != std::wstring::npos;
                c = CreateWindowExW(WS_EX_CLIENTEDGE, L"EDIT", dongWindows(t.giaTri).c_str(), WS_CHILD | WS_VISIBLE | ES_READONLY | (multi ? ES_MULTILINE | WS_VSCROLL : ES_AUTOHSCROLL), cx, y, ctlW, h, hwnd, menu, inst, nullptr);
                break;
            }
            default:
                c = CreateWindowExW(WS_EX_CLIENTEDGE, L"EDIT", t.giaTri.c_str(), WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_AUTOHSCROLL | (t.kieu == Kieu::MatKhau ? ES_PASSWORD : 0) | (t.kieu == Kieu::So || t.kieu == Kieu::Tien ? ES_RIGHT : 0), cx, y, ctlW, h, hwnd, menu, inst, nullptr);
                if (!t.goiY.empty()) SendMessageW(c, EM_SETCUEBANNER, TRUE, reinterpret_cast<LPARAM>(t.goiY.c_str()));
                break;
        }
        SendMessageW(c, WM_SETFONT, reinterpret_cast<WPARAM>(font(13)), TRUE);
        if (!t.goiY.empty() && t.kieu != Kieu::Chu && t.kieu != Kieu::So && t.kieu != Kieu::Tien && t.kieu != Kieu::MatKhau) {
            HWND tip = CreateWindowExW(0, L"STATIC", t.goiY.c_str(), WS_CHILD | WS_VISIBLE | SS_LEFT | SS_NOPREFIX, x, y + S(24), labelW - S(10), S(18), hwnd, nullptr, inst, nullptr);
            SendMessageW(tip, WM_SETFONT, reinterpret_cast<WPARAM>(font(10)), TRUE);
        }
        ctx.ctl.push_back(c);
        if (!firstFocus && t.kieu != Kieu::ChiDoc) firstFocus = c;
    }
    const int bw = S(116), bh = S(34), by = clientH - S(46);
    taoNut(hwnd, IDOK, nutOK.c_str(), clientW - pad - bw * 2 - S(10), by, bw, bh, true);
    taoNut(hwnd, IDCANCEL, L"Hủy", clientW - pad - bw, by, bw, bh, false);
    if (owner) EnableWindow(owner, FALSE);
    ShowWindow(hwnd, SW_SHOW);
    if (firstFocus) { SetFocus(firstFocus); SendMessageW(firstFocus, EM_SETSEL, 0, -1); }
    chayModal(hwnd, owner, ctx.xong);
    return ctx.ok;
}

// ===== Xem văn bản =====
namespace {
struct XemCtx { HWND owner{}, edit{}; std::wstring text, tenFile; bool xong{}; };
constexpr int ID_COPY = 2001, ID_SAVE = 2002;
LRESULT CALLBACK xemProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    auto* ctx = reinterpret_cast<XemCtx*>(GetWindowLongPtrW(hwnd, GWLP_USERDATA));
    auto close = [&] { ctx->xong = true; if (ctx->owner) EnableWindow(ctx->owner, TRUE); DestroyWindow(hwnd); };
    switch (msg) {
        case WM_NCCREATE: SetWindowLongPtrW(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(reinterpret_cast<CREATESTRUCTW*>(lp)->lpCreateParams)); break;
        case WM_COMMAND:
            if (!ctx) break;
            if (LOWORD(wp) == IDCANCEL || LOWORD(wp) == IDOK) { close(); return 0; }
            if (LOWORD(wp) == ID_COPY) { saoChep(hwnd, ctx->text); MessageBoxW(hwnd, L"Đã sao chép vào clipboard.", L"Sao chép", MB_OK | MB_ICONINFORMATION); return 0; }
            if (LOWORD(wp) == ID_SAVE) {
                std::wstring path;
                if (chonFileLuu(hwnd, path, L"Văn bản (*.txt)\0*.txt\0Tất cả\0*.*\0", L"txt", ctx->tenFile)) {
                    try { ghiFileUtf8(path, ctx->text); MessageBoxW(hwnd, (L"Đã lưu: " + path).c_str(), L"Lưu file", MB_OK | MB_ICONINFORMATION); }
                    catch (const std::exception& e) { MessageBoxW(hwnd, W(e.what()).c_str(), L"Lỗi", MB_OK | MB_ICONWARNING); }
                }
                return 0;
            }
            break;
        case WM_CLOSE: if (ctx) close(); return 0;
        case WM_CTLCOLORSTATIC: { HDC dc = reinterpret_cast<HDC>(wp); SetBkColor(dc, mau::the); static HBRUSH white = CreateSolidBrush(mau::the); return reinterpret_cast<LRESULT>(white); }
    }
    return DefWindowProcW(hwnd, msg, wp, lp);
}
}
void xemVanBan(HWND owner, const std::wstring& tieuDe, const std::wstring& noiDung, const std::wstring& tenFile) {
    static bool registered = false;
    if (!registered) { dangKyLop(L"CRMViewer", xemProc); registered = true; }
    XemCtx ctx; ctx.owner = owner; ctx.text = noiDung; ctx.tenFile = tenFile;
    const int cw = S(640), ch = S(560);
    const DWORD style = WS_POPUP | WS_CAPTION | WS_SYSMENU, ex = WS_EX_DLGMODALFRAME;
    RECT wr{0, 0, cw, ch}; AdjustWindowRectEx(&wr, style, FALSE, ex);
    const RECT pos = viTriGiua(owner, wr.right - wr.left, wr.bottom - wr.top);
    HWND hwnd = CreateWindowExW(ex, L"CRMViewer", tieuDe.c_str(), style, pos.left, pos.top, pos.right - pos.left, pos.bottom - pos.top, owner, nullptr, GetModuleHandleW(nullptr), &ctx);
    if (!hwnd) return;
    ctx.edit = CreateWindowExW(WS_EX_CLIENTEDGE, L"EDIT", dongWindows(noiDung).c_str(), WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL | ES_MULTILINE | ES_READONLY | ES_AUTOVSCROLL,
                               S(14), S(14), cw - S(28), ch - S(76), hwnd, nullptr, GetModuleHandleW(nullptr), nullptr);
    SendMessageW(ctx.edit, WM_SETFONT, reinterpret_cast<WPARAM>(font(13, false, L"Consolas")), TRUE);
    const int bw = S(120), bh = S(34), by = ch - S(50);
    taoNut(hwnd, ID_COPY, L"Sao chép", S(14), by, bw, bh, false);
    if (!tenFile.empty()) taoNut(hwnd, ID_SAVE, L"Lưu ra file...", S(14) + bw + S(10), by, bw, bh, false);
    taoNut(hwnd, IDOK, L"Đóng", cw - S(14) - bw, by, bw, bh, true);
    if (owner) EnableWindow(owner, FALSE);
    ShowWindow(hwnd, SW_SHOW);
    chayModal(hwnd, owner, ctx.xong);
}

// ===== File & clipboard =====
bool chonFileLuu(HWND owner, std::wstring& path, const wchar_t* filter, const wchar_t* ext, const std::wstring& tenMacDinh) {
    wchar_t buf[MAX_PATH]{}; lstrcpynW(buf, tenMacDinh.c_str(), MAX_PATH);
    OPENFILENAMEW of{}; of.lStructSize = sizeof(of); of.hwndOwner = owner; of.lpstrFilter = filter; of.lpstrFile = buf; of.nMaxFile = MAX_PATH;
    of.lpstrDefExt = ext; of.Flags = OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST | OFN_NOCHANGEDIR;
    if (!GetSaveFileNameW(&of)) return false;
    path = buf; return true;
}
bool chonFileMo(HWND owner, std::wstring& path, const wchar_t* filter) {
    wchar_t buf[MAX_PATH]{};
    OPENFILENAMEW of{}; of.lStructSize = sizeof(of); of.hwndOwner = owner; of.lpstrFilter = filter; of.lpstrFile = buf; of.nMaxFile = MAX_PATH;
    of.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_NOCHANGEDIR;
    if (!GetOpenFileNameW(&of)) return false;
    path = buf; return true;
}
void ghiFileUtf8(const std::wstring& path, const std::wstring& noiDung) {
    std::ofstream f(std::filesystem::path(path), std::ios::binary);
    if (!f) throw std::runtime_error("Không ghi được file.");
    std::string text = U(noiDung), out;
    for (char c : text) { if (c == '\n') out += '\r'; out += c; }
    f << "\xEF\xBB\xBF" << out; // BOM giúp Excel/Notepad nhận đúng UTF-8
}
void saoChep(HWND owner, const std::wstring& text) {
    if (!OpenClipboard(owner)) return;
    EmptyClipboard();
    const std::wstring data = dongWindows(text);
    HGLOBAL mem = GlobalAlloc(GMEM_MOVEABLE, (data.size() + 1) * sizeof(wchar_t));
    if (mem) { std::copy(data.c_str(), data.c_str() + data.size() + 1, static_cast<wchar_t*>(GlobalLock(mem))); GlobalUnlock(mem); SetClipboardData(CF_UNICODETEXT, mem); }
    CloseClipboard();
}

// ===== Danh sách cho ô chọn =====
std::vector<std::wstring> dsHoiVien(bool chiKichHoat) {
    std::vector<std::wstring> out;
    for (const auto& [id, hv] : C().hoiViens()) if (!chiKichHoat || hv->duocCheckIn()) out.push_back(W(id) + L" — " + W(hv->ten()));
    return out;
}
std::vector<std::wstring> dsPT(bool coRong) {
    std::vector<std::wstring> out; if (coRong) out.push_back(L"(Không)");
    for (const auto& [id, pt] : C().pts()) if (pt->dangLam()) out.push_back(W(id) + L" — " + W(pt->ten()) + L" (" + W(pt->chuyenMon()) + L")");
    return out;
}
std::vector<std::wstring> dsNhanSu(bool coRong) {
    std::vector<std::wstring> out; if (coRong) out.push_back(L"(Không)");
    for (auto* nv : C().danhSachNhanSu()) if (nv->dangLam()) out.push_back(W(nv->ma()) + L" — " + W(nv->ten()) + L" · " + W(nv->chucVu()));
    return out;
}
std::vector<std::wstring> dsGoi() {
    std::vector<std::wstring> out;
    for (const auto& [id, g] : C().goiDichVus()) if (g.dangBan()) out.push_back(W(id) + L" — " + W(g.ten()) + L" · " + tien(g.giaGoc()) + L" đ");
    return out;
}
std::vector<std::wstring> dsSanPham() {
    std::vector<std::wstring> out;
    for (const auto& [id, sp] : C().sanPhams()) out.push_back(W(id) + L" — " + W(sp.ten()));
    return out;
}
std::wstring timTrongDs(const std::vector<std::wstring>& ds, const std::string& ma) {
    for (const auto& s : ds) if (maTu(s) == ma) return s;
    return ds.empty() ? L"" : ds.front();
}
std::wstring tenHoiVien(const std::string& ma) {
    if (ma == crm::KHACH_LE) return L"Khách lẻ";
    auto it = C().hoiViens().find(ma); return it == C().hoiViens().end() ? W(ma) : W(it->second->ten());
}
std::wstring tenNhanSu(const std::string& ma) {
    if (ma.empty()) return L"";
    if (ma == "PHONG_TAP") return L"Phòng tập";
    auto* nv = C().timNhanSu(ma); return nv ? W(nv->ten()) : W(ma);
}
std::wstring vanBanHoaDon(const crm::HoaDon& hd) {
    std::wostringstream o;
    const std::wstring shop = W(C().caiDat("ten_phong_tap", "FITNESS CRM"));
    o << shop << L"\n" << W(C().caiDat("dia_chi", "")) << L"  Hotline: " << W(C().caiDat("hotline", "")) << L"\n";
    o << L"==============================================\n";
    o << L"              HÓA ĐƠN BÁN HÀNG\n";
    o << L"==============================================\n";
    o << L"Số HĐ   : " << W(hd.ma()) << L"\nNgày    : " << ngay(crm::parseDate(hd.ngay())) << L"\nKhách   : " << tenHoiVien(hd.maHoiVien());
    if (hd.maHoiVien() != crm::KHACH_LE) o << L" (" << W(hd.maHoiVien()) << L")";
    o << L"\nLoại    : " << W(hd.loai());
    if (!hd.maNhanVien().empty()) o << L"\nNhân viên: " << tenNhanSu(hd.maNhanVien());
    o << L"\n----------------------------------------------\n";
    for (const auto& d : hd.dongHang()) {
        o << W(d.ten) << L"\n";
        wchar_t line[96]; swprintf(line, 96, L"   %3d x %12ls = %14ls\n", d.soLuong, tien(d.donGia).c_str(), tien(d.thanhTien()).c_str());
        o << line;
    }
    o << L"----------------------------------------------\n";
    wchar_t buf[96];
    swprintf(buf, 96, L"Tạm tính      : %18ls đ\n", tien(hd.tamTinh()).c_str()); o << buf;
    if (hd.giamGia() > 0) { swprintf(buf, 96, L"Giảm giá      : %18ls đ\n", tien(hd.giamGia()).c_str()); o << buf; }
    swprintf(buf, 96, L"TỔNG CỘNG     : %18ls đ\n", tien(hd.tongTien()).c_str()); o << buf;
    swprintf(buf, 96, L"Đã thanh toán : %18ls đ (%ls)\n", tien(hd.daThanhToan()).c_str(), W(hd.phuongThuc()).c_str()); o << buf;
    if (hd.conNo() > 0) { swprintf(buf, 96, L"CÒN NỢ        : %18ls đ\n", tien(hd.conNo()).c_str()); o << buf; }
    if (!hd.ghiChu().empty()) o << L"Ghi chú: " << W(hd.ghiChu()) << L"\n";
    o << L"==============================================\n        Cảm ơn quý khách - Hẹn gặp lại!\n";
    return o.str();
}
}
namespace gui {
double docSo(const std::wstring& s) {
    std::wstring clean;
    for (wchar_t c : s) { if (iswdigit(c) || (c == L'-' && clean.empty())) clean += c; else if (c == L',') clean += L'.'; }
    if (clean.empty() || clean == L"-") return 0;
    try { return std::stod(clean); } catch (...) { return 0; }
}
std::wstring thapPhan(double v, int chuSo) {
    wchar_t buf[32]; swprintf(buf, 32, L"%.*f", chuSo, v);
    std::wstring s = buf; for (auto& c : s) if (c == L'.') c = L',';
    return s;
}
}
