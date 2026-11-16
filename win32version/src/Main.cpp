// Cửa sổ chính Fitness CRM: sidebar điều hướng, header, thanh thao tác, vùng vẽ, ListView và thanh trạng thái.
#include "App.h"
#include <shellapi.h>
#include <uxtheme.h>
#include <algorithm>
#include <chrono>
#include <cwctype>
#include <map>

namespace gui {
namespace {
constexpr int ID_LIST = 100, ID_SEARCH = 101, ID_FILTER = 102, ID_STATUS = 103, ID_BTN = 200;
constexpr int IDM_SAVE = 900, IDM_BACKUP = 901, IDM_OPENDIR = 902, IDM_EXPORT = 903, IDM_PASS = 904, IDM_LOGOUT = 905, IDM_EXIT = 906, IDM_ABOUT = 907, IDM_KEYS = 908, IDM_FIND = 909, IDM_REFRESH = 910;
constexpr UINT_PTR TIMER_SEARCH = 1, TIMER_CLOCK = 2, TIMER_AUTOSAVE = 3;

UngDung g_app;
std::vector<Trang> g_trang;
std::vector<int> g_hien;             // chỉ số các trang được phép theo vai trò
int g_idx = 0;                       // trang hiện tại (chỉ số trong g_trang)
std::map<int, int> g_boLoc;
HWND g_list{}, g_canvas{}, g_search{}, g_filter{}, g_status{};
std::vector<HWND> g_btn;
std::vector<const ThaoTac*> g_btnAct;
ThaoTac g_xuatCSV;
std::vector<Dong> g_all, g_rows;
std::vector<Cot> g_cols;
int g_sortCol = -1; bool g_sortAsc = true;
int g_hover = -1;
bool g_trackMouse = false;
int g_canvasH = 0;

int sidebarW() { return S(232); }
int itemTop() { return S(76); }
int searchW() { return S(205); }
int filterW() { return S(200); }
int g_statusH = 0;
int userTop(int clientBottom) { return clientBottom - g_statusH - S(66); }
int itemH() {
    RECT c{}; if (g_app.chinh) GetClientRect(g_app.chinh, &c);
    const int n = std::max<int>(1, static_cast<int>(g_hien.size()));
    return std::clamp((userTop(c.bottom) - itemTop() - S(8)) / n, S(26), S(34));
}
const Trang& trang() { return g_trang[static_cast<std::size_t>(g_idx)]; }
HINSTANCE inst() { return GetModuleHandleW(nullptr); }

unsigned quyenCua(crm::VaiTro v) {
    switch (v) { case crm::VaiTro::Admin: return Q_ADMIN; case crm::VaiTro::QuanLy: return Q_QUANLY; case crm::VaiTro::LeTan: return Q_LETAN; case crm::VaiTro::Sale: return Q_SALE; case crm::VaiTro::PT: return Q_PT; }
    return Q_LETAN;
}
std::filesystem::path thuMucExe() {
    wchar_t buf[MAX_PATH]{}; GetModuleFileNameW(nullptr, buf, MAX_PATH);
    auto dir = std::filesystem::path(buf).parent_path();
    const auto name = chuThuong(dir.filename().wstring());
    if (name == L"build" || name == L"bin" || name == L"release" || name == L"debug") dir = dir.parent_path();
    return dir;
}
std::filesystem::path timMau(const std::filesystem::path& base) {
    for (auto p : {base / L"sample_data", base.parent_path() / L"sample_data", base.parent_path().parent_path() / L"sample_data"})
        if (std::filesystem::exists(p / L"hoi_vien.csv")) return p;
    return {};
}

// ===== Sắp xếp & lọc =====
std::wstring khoaSapXep(const std::wstring& s, double& so, bool& laSo) {
    laSo = false;
    // dd/mm/yyyy [hh:mm] -> yyyymmddhhmm
    if (s.size() >= 10 && s[2] == L'/' && s[5] == L'/') return s.substr(6, 4) + s.substr(3, 2) + s.substr(0, 2) + (s.size() >= 16 ? s.substr(11, 5) : L"");
    std::wstring digits; bool ok = !s.empty();
    for (wchar_t c : s) {
        if (iswdigit(c)) digits += c;
        else if (c == L'-' && digits.empty()) digits += c;
        else if (c == L',' ) digits += L'.';
        else if (c != L'.' && c != L' ' && c != L'%' && c != L'đ') { ok = false; break; }
    }
    if (ok && !digits.empty() && digits != L"-") { try { so = std::stod(digits); laSo = true; } catch (...) {} }
    return chuThuong(s);
}
void sapXep() {
    if (g_sortCol < 0) return;
    const auto col = static_cast<std::size_t>(g_sortCol);
    std::stable_sort(g_rows.begin(), g_rows.end(), [&](const Dong& a, const Dong& b) {
        const std::wstring& x = col < a.size() ? a[col] : L""; const std::wstring& y = col < b.size() ? b[col] : L"";
        double nx = 0, ny = 0; bool sx = false, sy = false;
        const auto kx = khoaSapXep(x, nx, sx), ky = khoaSapXep(y, ny, sy);
        const bool less = (sx && sy) ? nx < ny : kx < ky;
        const bool greater = (sx && sy) ? nx > ny : kx > ky;
        return g_sortAsc ? less : greater;
    });
}
void locTimKiem() {
    std::wstring q = chuThuong([] { wchar_t buf[256]{}; GetWindowTextW(g_search, buf, 256); return std::wstring(buf); }());
    g_rows.clear();
    for (const auto& r : g_all) {
        if (q.empty()) { g_rows.push_back(r); continue; }
        for (const auto& cell : r) if (chuThuong(cell).find(q) != std::wstring::npos) { g_rows.push_back(r); break; }
    }
    sapXep();
}
void capNhatTrangThai() {
    if (!g_status) return;
    const std::wstring user = L"  " + W(g_app.tenDangNhap) + L" · " + W(crm::tenVaiTro(g_app.vaiTro));
    const std::wstring data = L"Dữ liệu: " + g_app.thuMuc.wstring() + (g_app.thayDoi ? L"  (chưa lưu)" : L"  (đã lưu)");
    const std::wstring rows = std::to_wstring(g_rows.size()) + L" / " + std::to_wstring(g_all.size()) + L" dòng";
    SYSTEMTIME st; GetLocalTime(&st);
    wchar_t clock[64]; swprintf(clock, 64, L"%02u:%02u  %02u/%02u/%04u", st.wHour, st.wMinute, st.wDay, st.wMonth, st.wYear);
    SendMessageW(g_status, SB_SETTEXTW, 0, reinterpret_cast<LPARAM>(user.c_str()));
    SendMessageW(g_status, SB_SETTEXTW, 1, reinterpret_cast<LPARAM>(data.c_str()));
    SendMessageW(g_status, SB_SETTEXTW, 2, reinterpret_cast<LPARAM>(rows.c_str()));
    SendMessageW(g_status, SB_SETTEXTW, 3, reinterpret_cast<LPARAM>(clock));
}
void napCot() {
    SendMessageW(g_list, WM_SETREDRAW, FALSE, 0);
    while (SendMessageW(g_list, LVM_DELETECOLUMN, 0, 0)) {}
    g_cols = trang().cot(boLocHienTai());
    for (std::size_t i = 0; i < g_cols.size(); ++i) {
        LVCOLUMNW c{}; c.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_FMT;
        c.fmt = (g_cols[i].so && i > 0) ? LVCFMT_RIGHT : LVCFMT_LEFT;
        c.cx = S(g_cols[i].rong); c.pszText = const_cast<wchar_t*>(g_cols[i].ten.c_str());
        SendMessageW(g_list, LVM_INSERTCOLUMNW, i, reinterpret_cast<LPARAM>(&c));
    }
    SendMessageW(g_list, WM_SETREDRAW, TRUE, 0);
}
void layoutAll(HWND hwnd);
void taoNutThaoTac(HWND hwnd);
int viTriSidebar(LPARAM lp) {
    const int x = static_cast<short>(LOWORD(lp)), y = static_cast<short>(HIWORD(lp));
    RECT c{}; GetClientRect(g_app.chinh, &c);
    if (x < 0 || x >= sidebarW()) return -1;
    if (y >= itemTop() && y < itemTop() + static_cast<int>(g_hien.size()) * itemH()) return (y - itemTop()) / itemH();
    if (y >= userTop(c.bottom) && y < userTop(c.bottom) + S(56)) return 1000;
    return -1;
}
}

// ===== API dùng chung =====
UngDung& app() { return g_app; }
crm::HethongCRM& C() { return g_app.crm; }
int boLocHienTai() { auto it = g_boLoc.find(g_idx); return it == g_boLoc.end() ? 0 : it->second; }
const std::vector<Dong>& dongHienTai() { return g_rows; }
bool laQuanLy() { return g_app.vaiTro == crm::VaiTro::Admin || g_app.vaiTro == crm::VaiTro::QuanLy; }
unsigned quyenHienTai() { return quyenCua(g_app.vaiTro); }
std::string chonHoacRong() {
    const int idx = static_cast<int>(SendMessageW(g_list, LVM_GETNEXTITEM, static_cast<WPARAM>(-1), LVNI_SELECTED));
    if (idx < 0 || static_cast<std::size_t>(idx) >= g_rows.size() || g_rows[static_cast<std::size_t>(idx)].empty()) return {};
    return U(g_rows[static_cast<std::size_t>(idx)][0]);
}
std::string chon() {
    auto key = chonHoacRong();
    if (key.empty()) throw std::runtime_error("Hãy chọn một dòng trong danh sách trước.");
    return key;
}
void lamMoi() {
    if (!g_list) return;
    const std::string giu = chonHoacRong();
    try { g_all = trang().dong(boLocHienTai()); }
    catch (const std::exception& e) { g_all.clear(); loi(W(e.what())); }
    locTimKiem();
    SendMessageW(g_list, LVM_SETITEMCOUNT, g_rows.size(), LVSICF_NOINVALIDATEALL);
    int sel = -1;
    if (!giu.empty()) for (std::size_t i = 0; i < g_rows.size(); ++i) if (!g_rows[i].empty() && U(g_rows[i][0]) == giu) { sel = static_cast<int>(i); break; }
    LVITEMW it{}; it.stateMask = LVIS_SELECTED | LVIS_FOCUSED;
    SendMessageW(g_list, LVM_SETITEMSTATE, static_cast<WPARAM>(-1), reinterpret_cast<LPARAM>(&it));
    if (sel >= 0) { it.state = LVIS_SELECTED | LVIS_FOCUSED; SendMessageW(g_list, LVM_SETITEMSTATE, static_cast<WPARAM>(sel), reinterpret_cast<LPARAM>(&it)); SendMessageW(g_list, LVM_ENSUREVISIBLE, static_cast<WPARAM>(sel), FALSE); }
    InvalidateRect(g_list, nullptr, FALSE);
    const int h = trang().chieuCaoVe ? trang().chieuCaoVe(boLocHienTai()) : 0;
    if (h != g_canvasH) layoutAll(g_app.chinh);
    if (g_canvas) InvalidateRect(g_canvas, nullptr, FALSE);
    RECT head{sidebarW(), 0, 30000, S(92)}; InvalidateRect(g_app.chinh, &head, FALSE);
    capNhatTrangThai();
}
void daThayDoi() { g_app.thayDoi = true; lamMoi(); }
void luuDuLieu(bool imLang) {
    try {
        g_app.crm.luuCSV(g_app.thuMuc);
        g_app.thayDoi = false;
        capNhatTrangThai();
        if (!imLang) thongBao(L"Đã lưu toàn bộ dữ liệu vào:\n" + g_app.thuMuc.wstring());
    } catch (const std::exception& e) { loi(L"Lưu dữ liệu thất bại: " + W(e.what())); }
}

namespace {
void xuatCSV() {
    std::wstring path;
    if (!chonFileLuu(g_app.chinh, path, L"CSV (*.csv)\0*.csv\0", L"csv", trang().ten + L".csv")) return;
    auto q = [](const std::wstring& s) { std::wstring r = L"\""; for (wchar_t c : s) { if (c == L'"') r += L'"'; r += c; } return r + L"\""; };
    std::wstring out;
    for (std::size_t i = 0; i < g_cols.size(); ++i) out += (i ? L"," : L"") + q(g_cols[i].ten);
    out += L"\n";
    for (const auto& r : g_rows) { for (std::size_t i = 0; i < r.size(); ++i) out += (i ? L"," : L"") + q(r[i]); out += L"\n"; }
    ghiFileUtf8(path, out);
    thongBao(L"Đã xuất " + std::to_wstring(g_rows.size()) + L" dòng ra:\n" + path);
}
void chayThaoTac(const ThaoTac& t) {
    try { t.chay(); }
    catch (const HuyBo&) {}
    catch (const std::exception& e) { loi(W(e.what())); }
}
void taoNutThaoTac(HWND hwnd) {
    for (HWND b : g_btn) DestroyWindow(b);
    g_btn.clear(); g_btnAct.clear();
    const int f = boLocHienTai();
    for (const auto& t : trang().thaoTac) {
        if (t.chiQuanLy && !laQuanLy()) continue;
        if (!t.boLoc.empty() && std::find(t.boLoc.begin(), t.boLoc.end(), f) == t.boLoc.end()) continue;
        g_btnAct.push_back(&t);
    }
    g_btnAct.push_back(&g_xuatCSV);
    for (std::size_t i = 0; i < g_btnAct.size(); ++i) {
        HWND b = CreateWindowExW(0, L"BUTTON", g_btnAct[i]->nhan.c_str(), WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_OWNERDRAW, 0, 0, 10, 10, hwnd,
                                 reinterpret_cast<HMENU>(static_cast<INT_PTR>(ID_BTN + static_cast<int>(i))), inst(), nullptr);
        SendMessageW(b, WM_SETFONT, reinterpret_cast<WPARAM>(font(13)), TRUE);
        g_btn.push_back(b);
    }
}
void layoutAll(HWND hwnd) {
    if (!g_list) return;
    RECT c; GetClientRect(hwnd, &c);
    SendMessageW(g_status, WM_SIZE, 0, 0);
    RECT sr; GetWindowRect(g_status, &sr);
    const int statusH = sr.bottom - sr.top;
    g_statusH = statusH;
    const int parts[4] = {S(230), std::max(S(240), static_cast<int>(c.right) - S(330)), std::max(S(330), static_cast<int>(c.right) - S(160)), -1};
    SendMessageW(g_status, SB_SETPARTS, 4, reinterpret_cast<LPARAM>(parts));
    const int x0 = sidebarW() + S(24), right = c.right - S(24);
    const int sw = searchW(), fw = filterW(), h = S(30), top = S(82);
    MoveWindow(g_search, right - sw, top, sw, h, TRUE);
    const bool coLoc = !trang().boLoc.empty();
    ShowWindow(g_filter, coLoc ? SW_SHOW : SW_HIDE);
    if (coLoc) MoveWindow(g_filter, right - sw - S(10) - fw, top, fw, S(400), TRUE);
    HDC dc = GetDC(hwnd); auto old = SelectObject(dc, font(13));
    int x = x0, y = top;
    const int bh = S(30);
    const int toolbarRight = right - sw - (coLoc ? fw + S(10) : 0) - S(12);
    bool dongPhu = false;
    for (std::size_t i = 0; i < g_btn.size(); ++i) {
        SIZE sz{}; const auto& text = g_btnAct[i]->nhan;
        GetTextExtentPoint32W(dc, text.c_str(), static_cast<int>(text.size()), &sz);
        const int bw = sz.cx + S(24);
        if (!dongPhu && (!g_btnAct[i]->chinh || x + bw > toolbarRight)) {
            dongPhu = true;
            x = x0;
            y += bh + S(8);
        }
        if (x + bw > right && x > x0) { x = x0; y += bh + S(8); }
        MoveWindow(g_btn[i], x, y, bw, bh, TRUE);
        x += bw + S(8);
    }
    SelectObject(dc, old); ReleaseDC(hwnd, dc);
    y += bh + S(14);
    const int bottom = c.bottom - statusH - S(16);
    g_canvasH = trang().chieuCaoVe ? trang().chieuCaoVe(boLocHienTai()) : 0;
    int ch = g_canvasH > 0 ? S(g_canvasH) : 0;
    if (ch > 0) ch = std::min(ch, std::max(S(80), bottom - y - S(150)));
    ShowWindow(g_canvas, ch > 0 ? SW_SHOW : SW_HIDE);
    if (ch > 0) { MoveWindow(g_canvas, x0, y, right - x0, ch, TRUE); y += ch + S(12); }
    MoveWindow(g_list, x0, y, right - x0, std::max(S(60), bottom - y), TRUE);
    InvalidateRect(hwnd, nullptr, FALSE);
}
void chonTrang(HWND hwnd, int idx) {
    g_idx = idx; g_sortCol = -1; g_sortAsc = true;
    SetWindowTextW(g_search, L"");
    SendMessageW(g_filter, CB_RESETCONTENT, 0, 0);
    for (const auto& f : trang().boLoc) SendMessageW(g_filter, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(f.c_str()));
    SendMessageW(g_filter, CB_SETCURSEL, boLocHienTai(), 0);
    taoNutThaoTac(hwnd);
    napCot();
    layoutAll(hwnd);
    lamMoi();
    RECT side{0, 0, sidebarW(), 30000}; InvalidateRect(hwnd, &side, FALSE);
}
void doiBoLoc(HWND hwnd, int f) {
    g_boLoc[g_idx] = f; g_sortCol = -1;
    taoNutThaoTac(hwnd); napCot(); layoutAll(hwnd); lamMoi();
}

// ===== Vẽ cửa sổ chính =====
void veSidebar(HDC dc, RECT c) {
    RECT side{0, 0, sidebarW(), c.bottom};
    toNen(dc, side, mau::sidebar);
    toNen(dc, {side.right - 1, 0, side.right, side.bottom}, mau::vien);
    veChu(dc, {S(20), S(14), sidebarW() - S(12), S(38)}, L"Fitness CRM", font(16, true), mau::chu);
    veChu(dc, {S(20), S(38), sidebarW() - S(12), S(57)}, W(C().caiDat("ten_phong_tap", "")), font(12), mau::phu);
    toNen(dc, {S(14), S(65), sidebarW() - S(14), S(65) + 1}, mau::vien);
    for (std::size_t k = 0; k < g_hien.size(); ++k) {
        const int i = g_hien[k];
        const int y = itemTop() + static_cast<int>(k) * itemH();
        RECT r{S(10), y, sidebarW() - S(10), y + itemH() - S(4)};
        const bool sel = i == g_idx, hov = static_cast<int>(k) == g_hover;
        if (sel || hov) toNen(dc, r, sel ? mau::sidebarChon : RGB(232, 235, 240));
        if (sel) toNen(dc, {r.left, r.top, r.left + S(3), r.bottom}, mau::chinh);
        const COLORREF fg = sel ? mau::chinh : mau::sidebarChu;
        veChu(dc, {S(18), y, S(42), r.bottom}, g_trang[static_cast<std::size_t>(i)].bieuTuong, fontIcon(14), fg, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
        veChu(dc, {S(49), y, sidebarW() - S(14), r.bottom}, g_trang[static_cast<std::size_t>(i)].ten, font(13, sel), fg);
    }
    RECT user{S(10), userTop(c.bottom), sidebarW() - S(10), userTop(c.bottom) + S(56)};
    if (g_hover == 1000) toNen(dc, user, RGB(232, 235, 240));
    toNen(dc, {S(14), user.top, user.right - S(4), user.top + 1}, mau::vien);
    veChu(dc, {S(20), user.top + S(8), user.right - S(28), user.top + S(28)}, W(g_app.tenDangNhap), font(13, true), mau::chu);
    veChu(dc, {S(20), user.top + S(28), user.right - S(28), user.top + S(48)}, W(crm::tenVaiTro(g_app.vaiTro)), font(12), mau::phu);
    veChu(dc, {user.right - S(25), user.top, user.right - S(8), user.bottom}, L"▾", font(13), mau::phu);
}
void veHeader(HDC dc, RECT c) {
    const int x0 = sidebarW() + S(24);
    veChu(dc, {x0, S(16), c.right - S(24), S(48)}, trang().ten, font(20, true), mau::chu);
    const std::wstring sub = trang().phuDe ? trang().phuDe(boLocHienTai()) : trang().moTa;
    veChu(dc, {x0, S(48), c.right - S(24), S(71)}, sub, font(12), mau::phu);
    toNen(dc, {x0, S(72), c.right - S(24), S(72) + 1}, mau::vien);
}
void veNut(const DRAWITEMSTRUCT& d) {
    const int i = static_cast<int>(d.CtlID) - ID_BTN;
    if (i < 0 || static_cast<std::size_t>(i) >= g_btnAct.size()) return;
    const ThaoTac& t = *g_btnAct[static_cast<std::size_t>(i)];
    const bool pressed = d.itemState & ODS_SELECTED, primary = t.chinh, csv = &t == &g_xuatCSV;
    RECT r = d.rcItem;
    toNen(d.hDC, r, mau::nen);
    COLORREF bg = primary ? (pressed ? RGB(26, 67, 124) : mau::chinh) : (pressed ? RGB(235, 238, 243) : mau::the);
    COLORREF border = primary ? bg : mau::vien;
    if (csv && !primary) bg = pressed ? RGB(235, 238, 243) : mau::nen;
    veKhung(d.hDC, r, bg, border, S(4));
    veChu(d.hDC, r, t.nhan, font(13, primary), primary ? RGB(255, 255, 255) : (csv ? mau::phu : mau::chu), DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    if (d.itemState & ODS_FOCUS) { RECT f = r; InflateRect(&f, -S(4), -S(4)); DrawFocusRect(d.hDC, &f); }
}
LRESULT CALLBACK canvasProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    if (msg == WM_ERASEBKGND) return 1;
    if (msg == WM_PAINT) {
        PAINTSTRUCT ps; HDC dc = BeginPaint(hwnd, &ps);
        RECT c; GetClientRect(hwnd, &c);
        HDC mem = CreateCompatibleDC(dc); HBITMAP bmp = CreateCompatibleBitmap(dc, c.right, c.bottom); auto old = SelectObject(mem, bmp);
        toNen(mem, c, mau::nen);
        if (trang().ve) { try { trang().ve(mem, c, boLocHienTai()); } catch (const std::exception& e) { veChu(mem, c, W(e.what()), font(12), mau::doDam); } }
        BitBlt(dc, 0, 0, c.right, c.bottom, mem, 0, 0, SRCCOPY);
        SelectObject(mem, old); DeleteObject(bmp); DeleteDC(mem);
        EndPaint(hwnd, &ps); return 0;
    }
    return DefWindowProcW(hwnd, msg, wp, lp);
}

bool dangNhap() {
    std::vector<Truong> f{{L"Tên đăng nhập", Kieu::Chu, L"", {}, true}, {L"Mật khẩu", Kieu::MatKhau, L"", {}, true}};
    const auto* adminMacDinh = C().dangNhap("admin", "admin123");
    const std::wstring goiY = adminMacDinh && adminMacDinh->vaiTro() == crm::VaiTro::Admin
        ? L"Tài khoản quản trị mặc định: admin / admin123 — hãy đổi mật khẩu sau lần đăng nhập đầu."
        : L"";
    return moForm(nullptr, L"Đăng nhập Fitness CRM", f, [](const KetQua& k) {
        const auto* tk = C().dangNhap(k.chu(0), k.chu(1));
        if (!tk) throw std::runtime_error("Sai tên đăng nhập/mật khẩu hoặc tài khoản đã bị khóa.");
        g_app.tenDangNhap = tk->tenDangNhap(); g_app.vaiTro = tk->vaiTro(); g_app.maNhanVien = tk->maNhanVien();
    }, L"Đăng nhập", goiY);
}
void apDungQuyen(HWND hwnd) {
    g_hien.clear();
    const unsigned q = quyenHienTai();
    for (std::size_t i = 0; i < g_trang.size(); ++i) if (g_trang[i].quyen & q) g_hien.push_back(static_cast<int>(i));
    g_boLoc.clear();
    if (!g_hien.empty()) chonTrang(hwnd, g_hien.front());
}
void doiMatKhau() {
    std::vector<Truong> f{{L"Mật khẩu hiện tại", Kieu::MatKhau, L"", {}, true}, {L"Mật khẩu mới", Kieu::MatKhau, L"", {}, true}, {L"Nhập lại mật khẩu mới", Kieu::MatKhau, L"", {}, true}};
    if (moForm(g_app.chinh, L"Đổi mật khẩu", f, [](const KetQua& k) {
        if (!C().dangNhap(g_app.tenDangNhap, k.chu(0))) throw std::runtime_error("Mật khẩu hiện tại không đúng.");
        if (k.chu(1) != k.chu(2)) throw std::runtime_error("Mật khẩu nhập lại không khớp.");
        C().taiKhoan(g_app.tenDangNhap).doiMatKhau(k.chu(1));
    }, L"Đổi mật khẩu")) { daThayDoi(); thongBao(L"Đã đổi mật khẩu."); }
}
void dangXuat(HWND hwnd) {
    luuDuLieu(true);
    ShowWindow(hwnd, SW_HIDE);
    if (!dangNhap()) { DestroyWindow(hwnd); return; }
    apDungQuyen(hwnd);
    ShowWindow(hwnd, SW_SHOW); SetForegroundWindow(hwnd);
}
void saoLuu() {
    luuDuLieu(true);
    try {
        const auto t = crm::now();
        std::string stamp = crm::formatDate(crm::ngayCua(t)) + "_" + crm::formatTime(t);
        std::replace(stamp.begin(), stamp.end(), ':', 'h');
        const auto dest = g_app.thuMuc / L"backup" / W(stamp);
        std::filesystem::create_directories(dest);
        int n = 0;
        for (const auto& e : std::filesystem::directory_iterator(g_app.thuMuc))
            if (e.is_regular_file() && e.path().extension() == L".csv") { std::filesystem::copy_file(e.path(), dest / e.path().filename(), std::filesystem::copy_options::overwrite_existing); ++n; }
        thongBao(L"Đã sao lưu " + std::to_wstring(n) + L" file vào:\n" + dest.wstring());
    } catch (const std::exception& e) { loi(W(e.what())); }
}
HMENU taoMenu() {
    HMENU bar = CreateMenu(), file = CreatePopupMenu(), view = CreatePopupMenu(), help = CreatePopupMenu();
    AppendMenuW(file, MF_STRING, IDM_SAVE, L"&Lưu dữ liệu\tCtrl+S");
    AppendMenuW(file, MF_STRING, IDM_BACKUP, L"&Sao lưu dữ liệu...");
    AppendMenuW(file, MF_STRING, IDM_OPENDIR, L"&Mở thư mục dữ liệu");
    AppendMenuW(file, MF_STRING, IDM_EXPORT, L"&Xuất danh sách hiện tại ra CSV...");
    AppendMenuW(file, MF_SEPARATOR, 0, nullptr);
    AppendMenuW(file, MF_STRING, IDM_PASS, L"Đổi &mật khẩu...");
    AppendMenuW(file, MF_STRING, IDM_LOGOUT, L"Đăng &xuất");
    AppendMenuW(file, MF_SEPARATOR, 0, nullptr);
    AppendMenuW(file, MF_STRING, IDM_EXIT, L"&Thoát\tAlt+F4");
    AppendMenuW(view, MF_STRING, IDM_REFRESH, L"&Làm mới\tF5");
    AppendMenuW(view, MF_STRING, IDM_FIND, L"&Tìm kiếm\tCtrl+F");
    AppendMenuW(help, MF_STRING, IDM_KEYS, L"&Phím tắt");
    AppendMenuW(help, MF_STRING, IDM_ABOUT, L"&Giới thiệu");
    AppendMenuW(bar, MF_POPUP, reinterpret_cast<UINT_PTR>(file), L"&Tệp");
    AppendMenuW(bar, MF_POPUP, reinterpret_cast<UINT_PTR>(view), L"&Xem");
    AppendMenuW(bar, MF_POPUP, reinterpret_cast<UINT_PTR>(help), L"Trợ &giúp");
    return bar;
}

LRESULT onNotify(HWND hwnd, NMHDR* h) {
    if (h->hwndFrom != g_list) return 0;
    switch (h->code) {
        case LVN_GETDISPINFOW: {
            auto* d = reinterpret_cast<NMLVDISPINFOW*>(h);
            if ((d->item.mask & LVIF_TEXT) && d->item.iItem >= 0 && static_cast<std::size_t>(d->item.iItem) < g_rows.size()) {
                const auto& row = g_rows[static_cast<std::size_t>(d->item.iItem)];
                const auto col = static_cast<std::size_t>(d->item.iSubItem);
                const std::wstring& text = col < row.size() ? row[col] : L"";
                lstrcpynW(d->item.pszText, text.c_str(), d->item.cchTextMax);
            }
            return 0;
        }
        case NM_CUSTOMDRAW: {
            auto* cd = reinterpret_cast<NMLVCUSTOMDRAW*>(h);
            if (cd->nmcd.dwDrawStage == CDDS_PREPAINT) return CDRF_NOTIFYITEMDRAW;
            if (cd->nmcd.dwDrawStage == CDDS_ITEMPREPAINT) {
                const auto i = static_cast<std::size_t>(cd->nmcd.dwItemSpec);
                cd->clrTextBk = i % 2 ? mau::dongChan : mau::the;
                cd->clrText = mau::chu;
                if (trang().mauChu && i < g_rows.size()) { const COLORREF c = trang().mauChu(g_rows[i], boLocHienTai()); if (c != CLR_DEFAULT) cd->clrText = c; }
                return CDRF_NEWFONT;
            }
            return CDRF_DODEFAULT;
        }
        case LVN_COLUMNCLICK: {
            const int col = reinterpret_cast<NMLISTVIEW*>(h)->iSubItem;
            if (g_sortCol == col) g_sortAsc = !g_sortAsc; else { g_sortCol = col; g_sortAsc = true; }
            sapXep(); InvalidateRect(g_list, nullptr, FALSE);
            return 0;
        }
        case NM_DBLCLK:
            if (trang().moDong && !chonHoacRong().empty()) {
                try { trang().moDong(boLocHienTai()); } catch (const HuyBo&) {} catch (const std::exception& e) { loi(W(e.what())); }
            }
            return 0;
        case LVN_KEYDOWN:
            if (reinterpret_cast<NMLVKEYDOWN*>(h)->wVKey == VK_RETURN && trang().moDong && !chonHoacRong().empty()) {
                try { trang().moDong(boLocHienTai()); } catch (const HuyBo&) {} catch (const std::exception& e) { loi(W(e.what())); }
            }
            return 0;
    }
    (void)hwnd;
    return 0;
}

LRESULT CALLBACK mainProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {
        case WM_CREATE: {
            g_app.chinh = hwnd;
            g_status = CreateWindowExW(0, STATUSCLASSNAMEW, L"", WS_CHILD | WS_VISIBLE | SBARS_SIZEGRIP, 0, 0, 0, 0, hwnd, reinterpret_cast<HMENU>(ID_STATUS), inst(), nullptr);
            SendMessageW(g_status, WM_SETFONT, reinterpret_cast<WPARAM>(font(12)), TRUE);
            g_search = CreateWindowExW(WS_EX_CLIENTEDGE, L"EDIT", L"", WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_AUTOHSCROLL, 0, 0, 10, 10, hwnd, reinterpret_cast<HMENU>(ID_SEARCH), inst(), nullptr);
            SendMessageW(g_search, WM_SETFONT, reinterpret_cast<WPARAM>(font(13)), TRUE);
            SendMessageW(g_search, EM_SETCUEBANNER, TRUE, reinterpret_cast<LPARAM>(L"Tìm kiếm (Ctrl+F)"));
            g_filter = CreateWindowExW(0, L"COMBOBOX", L"", WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_VSCROLL | CBS_DROPDOWNLIST, 0, 0, 10, 300, hwnd, reinterpret_cast<HMENU>(ID_FILTER), inst(), nullptr);
            SendMessageW(g_filter, WM_SETFONT, reinterpret_cast<WPARAM>(font(13)), TRUE);
            g_canvas = CreateWindowExW(0, L"CRMCanvas", L"", WS_CHILD, 0, 0, 10, 10, hwnd, nullptr, inst(), nullptr);
            g_list = CreateWindowExW(0, WC_LISTVIEWW, L"", WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_BORDER | LVS_REPORT | LVS_SHOWSELALWAYS | LVS_SINGLESEL | LVS_OWNERDATA,
                                     0, 0, 10, 10, hwnd, reinterpret_cast<HMENU>(ID_LIST), inst(), nullptr);
            SendMessageW(g_list, WM_SETFONT, reinterpret_cast<WPARAM>(font(13)), TRUE);
            SendMessageW(g_list, LVM_SETEXTENDEDLISTVIEWSTYLE, 0, LVS_EX_FULLROWSELECT | LVS_EX_DOUBLEBUFFER | LVS_EX_HEADERDRAGDROP | LVS_EX_LABELTIP);
            SetWindowTheme(g_list, L"Explorer", nullptr);
            HIMAGELIST rowHeight = ImageList_Create(1, S(28), ILC_COLOR, 1, 1); // tăng chiều cao dòng
            SendMessageW(g_list, LVM_SETIMAGELIST, LVSIL_SMALL, reinterpret_cast<LPARAM>(rowHeight));
            g_xuatCSV.nhan = L"Xuất CSV"; g_xuatCSV.chay = xuatCSV;
            SetTimer(hwnd, TIMER_CLOCK, 30000, nullptr);
            SetTimer(hwnd, TIMER_AUTOSAVE, 5 * 60 * 1000, nullptr);
            return 0;
        }
        case WM_SIZE: layoutAll(hwnd); return 0;
        case WM_GETMINMAXINFO: { auto* mm = reinterpret_cast<MINMAXINFO*>(lp); mm->ptMinTrackSize = {S(1020), S(700)}; return 0; }
        case WM_ERASEBKGND: return 1;
        case WM_PAINT: {
            PAINTSTRUCT ps; HDC dc = BeginPaint(hwnd, &ps);
            RECT c; GetClientRect(hwnd, &c);
            HDC mem = CreateCompatibleDC(dc); HBITMAP bmp = CreateCompatibleBitmap(dc, c.right, c.bottom); auto old = SelectObject(mem, bmp);
            toNen(mem, c, mau::nen);
            veSidebar(mem, c);
            if (!g_trang.empty()) veHeader(mem, c);
            BitBlt(dc, 0, 0, c.right, c.bottom, mem, 0, 0, SRCCOPY);
            SelectObject(mem, old); DeleteObject(bmp); DeleteDC(mem);
            EndPaint(hwnd, &ps); return 0;
        }
        case WM_CTLCOLORSTATIC: { HDC dc = reinterpret_cast<HDC>(wp); SetBkColor(dc, mau::nen); static HBRUSH b = CreateSolidBrush(mau::nen); return reinterpret_cast<LRESULT>(b); }
        case WM_MOUSEMOVE: {
            RECT c; GetClientRect(hwnd, &c);
            const int hov = viTriSidebar(lp);
            if (hov != g_hover) { g_hover = hov; RECT side{0, 0, sidebarW(), c.bottom}; InvalidateRect(hwnd, &side, FALSE); }
            SetCursor(LoadCursor(nullptr, hov >= 0 ? IDC_HAND : IDC_ARROW));
            if (!g_trackMouse) { TRACKMOUSEEVENT t{sizeof(t), TME_LEAVE, hwnd, 0}; TrackMouseEvent(&t); g_trackMouse = true; }
            return 0;
        }
        case WM_SETCURSOR:
            if (LOWORD(lp) == HTCLIENT && g_hover >= 0) { SetCursor(LoadCursor(nullptr, IDC_HAND)); return TRUE; }
            break;
        case WM_MOUSELEAVE: g_trackMouse = false; if (g_hover != -1) { g_hover = -1; RECT side{0, 0, sidebarW(), 30000}; InvalidateRect(hwnd, &side, FALSE); } return 0;
        case WM_LBUTTONDOWN: {
            g_hover = viTriSidebar(lp);
            if (g_hover >= 0 && g_hover < static_cast<int>(g_hien.size())) { chonTrang(hwnd, g_hien[static_cast<std::size_t>(g_hover)]); SetFocus(g_list); }
            else if (g_hover == 1000) {
                HMENU m = CreatePopupMenu();
                AppendMenuW(m, MF_STRING, IDM_PASS, L"Đổi mật khẩu...");
                AppendMenuW(m, MF_STRING, IDM_LOGOUT, L"Đăng xuất");
                POINT p{static_cast<short>(LOWORD(lp)), static_cast<short>(HIWORD(lp))}; ClientToScreen(hwnd, &p);
                TrackPopupMenu(m, TPM_LEFTALIGN | TPM_BOTTOMALIGN, p.x, p.y, 0, hwnd, nullptr);
                DestroyMenu(m);
            }
            return 0;
        }
        case WM_DRAWITEM: veNut(*reinterpret_cast<DRAWITEMSTRUCT*>(lp)); return TRUE;
        case WM_NOTIFY: return onNotify(hwnd, reinterpret_cast<NMHDR*>(lp));
        case WM_TIMER:
            if (wp == TIMER_SEARCH) { KillTimer(hwnd, TIMER_SEARCH); locTimKiem(); SendMessageW(g_list, LVM_SETITEMCOUNT, g_rows.size(), 0); InvalidateRect(g_list, nullptr, FALSE); capNhatTrangThai(); }
            else if (wp == TIMER_CLOCK) capNhatTrangThai();
            else if (wp == TIMER_AUTOSAVE && g_app.thayDoi) luuDuLieu(true);
            return 0;
        case WM_COMMAND: {
            const int id = LOWORD(wp), code = HIWORD(wp);
            if (id >= ID_BTN && id < ID_BTN + static_cast<int>(g_btnAct.size()) && code == BN_CLICKED) { chayThaoTac(*g_btnAct[static_cast<std::size_t>(id - ID_BTN)]); return 0; }
            if (id == ID_SEARCH && code == EN_CHANGE) { SetTimer(hwnd, TIMER_SEARCH, 220, nullptr); return 0; }
            if (id == ID_FILTER && code == CBN_SELCHANGE) { doiBoLoc(hwnd, static_cast<int>(SendMessageW(g_filter, CB_GETCURSEL, 0, 0))); return 0; }
            if (id == IDOK && GetFocus() == g_list && trang().moDong && !chonHoacRong().empty()) {
                try { trang().moDong(boLocHienTai()); } catch (const HuyBo&) {} catch (const std::exception& e) { loi(W(e.what())); }
                return 0;
            }
            if (id == IDOK && GetFocus() == g_search) { SetFocus(g_list); return 0; }
            switch (id) {
                case IDM_SAVE: luuDuLieu(false); return 0;
                case IDM_BACKUP: saoLuu(); return 0;
                case IDM_OPENDIR: std::filesystem::create_directories(g_app.thuMuc); ShellExecuteW(hwnd, L"open", g_app.thuMuc.wstring().c_str(), nullptr, nullptr, SW_SHOWNORMAL); return 0;
                case IDM_EXPORT: try { xuatCSV(); } catch (const std::exception& e) { loi(W(e.what())); } return 0;
                case IDM_PASS: doiMatKhau(); return 0;
                case IDM_LOGOUT: dangXuat(hwnd); return 0;
                case IDM_EXIT: SendMessageW(hwnd, WM_CLOSE, 0, 0); return 0;
                case IDM_REFRESH: C().capNhatTrangThaiHetHan(); lamMoi(); return 0;
                case IDM_FIND: SetFocus(g_search); SendMessageW(g_search, EM_SETSEL, 0, -1); return 0;
                case IDM_KEYS: thongBao(L"Ctrl+S\tLưu dữ liệu\nCtrl+F\tTìm kiếm trong danh sách\nF5\tLàm mới, cập nhật thẻ hết hạn\nEnter / nhấp đúp\tThao tác mặc định với dòng đang chọn\nNhấp tiêu đề cột\tSắp xếp tăng/giảm", L"Phím tắt"); return 0;
                case IDM_ABOUT: thongBao(L"Fitness CRM — Win32 edition\n\nPhần mềm quản lý phòng gym viết bằng C++20, giao diện Win32 API thuần (User32, GDI, Common Controls v6, Common Dialogs), không dùng thư viện bên thứ ba.\n\nDữ liệu lưu dạng CSV UTF-8 tại:\n" + g_app.thuMuc.wstring(), L"Giới thiệu"); return 0;
            }
            break;
        }
        case WM_CLOSE:
            if (g_app.thayDoi) luuDuLieu(true);
            DestroyWindow(hwnd);
            return 0;
        case WM_DESTROY: PostQuitMessage(0); return 0;
    }
    return DefWindowProcW(hwnd, msg, wp, lp);
}
}
}

int WINAPI wWinMain(HINSTANCE instance, HINSTANCE, PWSTR, int show) {
    using namespace gui;
    khoiTaoDPI();
    INITCOMMONCONTROLSEX icc{sizeof(icc), ICC_LISTVIEW_CLASSES | ICC_DATE_CLASSES | ICC_BAR_CLASSES | ICC_STANDARD_CLASSES};
    InitCommonControlsEx(&icc);

    auto& a = app();
    const auto base = thuMucExe();
    a.thuMuc = base / L"data";
    try {
        if (std::filesystem::exists(a.thuMuc / L"hoi_vien.csv")) a.crm.docCSV(a.thuMuc);
        else if (auto mau = timMau(base); !mau.empty()) { a.crm.docCSV(mau); a.thayDoi = true; }
    } catch (const std::exception& e) { loi(L"Không nạp được dữ liệu: " + W(e.what())); }

    WNDCLASSW canvas{}; canvas.lpfnWndProc = canvasProc; canvas.hInstance = instance; canvas.lpszClassName = L"CRMCanvas"; canvas.hCursor = LoadCursor(nullptr, IDC_ARROW);
    RegisterClassW(&canvas);
    WNDCLASSW wc{}; wc.lpfnWndProc = mainProc; wc.hInstance = instance; wc.lpszClassName = L"FitnessCRMWin32"; wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hIcon = LoadIconW(instance, MAKEINTRESOURCEW(1)); if (!wc.hIcon) wc.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
    RegisterClassW(&wc);

    themTrangTongQuan(g_trang); themTrangHoiVien(g_trang); themTrangDichVu(g_trang); themTrangTaiChinh(g_trang);
    themTrangNhanSu(g_trang); themTrangVanHanh(g_trang); themTrangCSKH(g_trang); themTrangBaoCao(g_trang); themTrangHeThong(g_trang);

    RECT work{}; SystemParametersInfoW(SPI_GETWORKAREA, 0, &work, 0);
    const int w = std::min(S(1360), static_cast<int>(work.right - work.left) - S(20)), h = std::min(S(860), static_cast<int>(work.bottom - work.top) - S(20));
    HWND hwnd = CreateWindowExW(0, wc.lpszClassName, L"Fitness CRM — Quản lý phòng gym", WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
                                work.left + (work.right - work.left - w) / 2, work.top + (work.bottom - work.top - h) / 2, w, h, nullptr, taoMenu(), instance, nullptr);
    if (!hwnd) return 1;
    if (!dangNhap()) { DestroyWindow(hwnd); return 0; }
    apDungQuyen(hwnd);
    ShowWindow(hwnd, show == SW_SHOWMINIMIZED ? SW_SHOWNORMAL : show);
    UpdateWindow(hwnd);
    if (!C().canhBaoNapDuLieu().empty()) {
        std::wstring text = L"Một số dòng dữ liệu không hợp lệ đã được bỏ qua:\n\n";
        for (const auto& s : C().canhBaoNapDuLieu()) text += W(s) + L"\n";
        xemVanBan(hwnd, L"Cảnh báo khi nạp dữ liệu", text);
    }

    ACCEL acc[] = {{FCONTROL | FVIRTKEY, 'S', IDM_SAVE}, {FCONTROL | FVIRTKEY, 'F', IDM_FIND}, {FVIRTKEY, VK_F5, IDM_REFRESH}};
    HACCEL accel = CreateAcceleratorTableW(acc, 3);
    MSG msg{};
    while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
        if (TranslateAcceleratorW(hwnd, accel, &msg)) continue;
        if (IsDialogMessageW(hwnd, &msg)) continue;
        TranslateMessage(&msg); DispatchMessageW(&msg);
    }
    DestroyAcceleratorTable(accel);
    return static_cast<int>(msg.wParam);
}
