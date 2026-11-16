// Sinh bộ dữ liệu giả (CSV UTF-8) cho Fitness CRM: hội viên, gói tập, lịch sử ra vào, hóa đơn, lịch PT,
// kho hàng, chấm công, thiết bị, tủ đồ, phản hồi... Mọi tên/số điện thoại đều là dữ liệu bịa.
// Build: g++ -std=c++20 -I ../include TaoDuLieuMau.cpp ../src/DateUtils.cpp ../src/TaiKhoan.cpp -o TaoDuLieuMau.exe
// Chạy:  TaoDuLieuMau.exe ../sample_data
#include "DateUtils.h"
#include "TaiKhoan.h"
#include <algorithm>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>
#include <random>
#include <set>
#include <sstream>
#include <vector>

using namespace crm;
namespace {
std::mt19937 rng(20261026);
int rnd(int a, int b) { return std::uniform_int_distribution<int>(a, b)(rng); }
bool coXacSuat(double p) { return std::uniform_real_distribution<double>(0, 1)(rng) < p; }
template<class T> const T& chon(const std::vector<T>& v) { return v[static_cast<std::size_t>(rnd(0, static_cast<int>(v.size()) - 1))]; }
std::string q(const std::string& s) { std::string r = "\""; for (char c : s) { if (c == '"') r += '"'; r += c; } return r + "\""; }
std::string so(double v) { std::ostringstream o; o << std::fixed << std::setprecision(0) << v; return o.str(); }
std::string pad(int n, int w) { std::ostringstream o; o << std::setw(w) << std::setfill('0') << n; return o.str(); }
std::string ngay(Date d) { return d.ok() ? formatDate(d) : ""; }
std::string luc(DateTime t) { return t.time_since_epoch().count() ? formatDateTime(t) : ""; }

struct Csv {
    std::ofstream f;
    Csv(const std::filesystem::path& p, const std::string& header) : f(p, std::ios::binary) { f << header << '\n'; }
    template<class... A> void dong(const A&... a) { bool first = true; ((f << (first ? "" : ",") << a, first = false), ...); f << '\n'; }
};

struct Goi { std::string ma, ten, loai; int thang, ngay, buoi, buoiPT; double gia, giaSV; int trongSo; };
const std::vector<Goi> kGoi{
    {"GT01", "Gói tập 1 ngày", "Theo ngày", 0, 0, 1, 0, 36000, 0, 4},
    {"GT02", "Gói tập 1 tháng", "Theo tháng", 1, 0, 0, 0, 339000, 240000, 34},
    {"GT03", "Gói tập 3 tháng", "Theo tháng", 3, 0, 0, 0, 899000, 0, 22},
    {"GT04", "Gói tập 6 tháng (5+1)", "Theo tháng", 6, 0, 0, 0, 1499000, 0, 9},
    {"GT05", "Gói tập 13 tháng (12+1)", "Theo tháng", 13, 0, 0, 0, 2999000, 0, 4},
    {"GT06", "Gói 12 buổi (trong 60 ngày)", "Theo số buổi", 0, 60, 12, 0, 450000, 0, 9},
    {"GT07", "1 tháng + 8 buổi PT", "Kèm PT", 1, 0, 0, 8, 2490000, 0, 12},
    {"GT08", "3 tháng + 24 buổi PT", "Kèm PT", 3, 0, 0, 24, 6490000, 0, 6},
};
const Goi& chonGoi() {
    int total = 0; for (const auto& g : kGoi) if (g.ma != "GT01") total += g.trongSo;
    int r = rnd(1, total);
    for (const auto& g : kGoi) { if (g.ma == "GT01") continue; r -= g.trongSo; if (r <= 0) return g; }
    return kGoi[1];
}
Date hetHan(const Goi& g, Date start) { if (g.thang == 0 && g.ngay == 0) return start; return addDays(addMonths(start, g.thang), g.ngay - 1); }

struct HV {
    std::string ma, ten, sdt, gioiTinh, nghe, email, maThe, loaiThe, maPT, goi, hang = "Thường", ghiChu, trangThai = "Mới tạo";
    Date sinh{}, thamGia{}, han{}, baoLuu{}; bool sv{}; int buoi{}, buoiPT{}; bool gioiHan{};
    std::vector<std::string> lichSu; double chiTieu{};
};
struct LuotTap { std::string hv; DateTime vao, ra; std::string cach, tu; };
struct Lich { std::string ma, hv, pt; DateTime bd, kt; std::string tt, note; };
struct HD { std::string ma, hv, ngay, loai, pt, nv, note; std::vector<std::tuple<std::string, int, double, std::string>> dong; double daTra{}; bool no{}; double giam{}; };
}

int main(int argc, char** argv) {
    const std::filesystem::path out = argc > 1 ? argv[1] : "sample_data";
    std::filesystem::create_directories(out);
    const Date d0 = today();
    const DateTime luc0 = now();

    // ===== Nhân sự =====
    struct NV { std::string ma, ten, chucVu, sdt; double luong, hh; Date vao; };
    const std::vector<NV> nvs{
        {"NV001", "Trương Thị Hồng Nhung", "Quản lý", "0912000101", 15000000, 0.02, addDays(d0, -900)},
        {"NV002", "Lâm Minh Tuấn", "Lễ tân", "0912000102", 6500000, 0.03, addDays(d0, -420)},
        {"NV003", "Đinh Thu Trang", "Lễ tân", "0912000103", 6500000, 0.03, addDays(d0, -200)},
        {"NV004", "Phùng Gia Huy", "Sale", "0912000104", 5500000, 0.06, addDays(d0, -380)},
        {"NV005", "Mai Ngọc Ánh", "Sale", "0912000105", 5500000, 0.06, addDays(d0, -150)},
        {"NV006", "Hà Quốc Việt", "Chăm sóc khách hàng", "0912000106", 6000000, 0.02, addDays(d0, -300)},
    };
    struct PTx { std::string ma, ten, mon, sdt; double luong, donGia, hh; int bd, kt; double giaHV; int soCa{}; double dt{}; int cong{}; };
    std::vector<PTx> pts{
        {"PT001", "Bùi Hoàng Nam", "Strength & conditioning", "0933000201", 5000000, 150000, 0.10, 6, 14, 350000},
        {"PT002", "Nguyễn Thu Hà", "Yoga / mobility", "0933000202", 5000000, 140000, 0.10, 8, 17, 300000},
        {"PT003", "Trần Tuấn Kiệt", "Giảm cân", "0933000203", 5500000, 160000, 0.10, 14, 22, 350000},
        {"PT004", "Lê Mỹ Duyên", "Gym tổng hợp", "0933000204", 4500000, 130000, 0.08, 15, 22, 300000},
        {"PT005", "Võ Đức Thịnh", "Boxing", "0933000205", 5000000, 150000, 0.10, 6, 12, 380000},
    };
    std::vector<std::string> sales{"NV002", "NV003", "NV004", "NV004", "NV005", "NV005"};

    // ===== Hội viên =====
    const std::vector<std::string> ho{"Nguyễn", "Trần", "Lê", "Phạm", "Hoàng", "Huỳnh", "Phan", "Vũ", "Võ", "Đặng", "Bùi", "Đỗ", "Hồ", "Ngô", "Dương", "Lý"};
    const std::vector<std::string> demNam{"Văn", "Minh", "Hoàng", "Gia", "Quốc", "Đức", "Thanh", "Hải", "Bảo", "Anh"};
    const std::vector<std::string> demNu{"Thị", "Ngọc", "Thu", "Minh", "Thanh", "Bảo", "Khánh", "Mỹ", "Phương", "Hải"};
    const std::vector<std::string> tenNam{"Anh", "Bảo", "Dũng", "Hiếu", "Hùng", "Khang", "Khoa", "Long", "Minh", "Nam", "Phúc", "Quân", "Sơn", "Tâm", "Trung", "Tú", "Vinh", "Đạt", "Huy", "Lộc"};
    const std::vector<std::string> tenNu{"Anh", "Châu", "Hà", "Hương", "Linh", "Mai", "Ngân", "Nhi", "Quỳnh", "Thảo", "Trang", "Uyên", "Vy", "Yến", "Trâm", "My", "Hạnh", "Lan", "Giang", "Thư"};
    const std::vector<std::string> nghe{"Nhân viên văn phòng", "Kinh doanh", "Kỹ sư / IT", "Giáo viên", "Y tế", "Lao động tự do", "Nội trợ", "Sinh viên"};
    const std::vector<std::string> cach{"Thẻ từ", "Thẻ từ", "Mã vạch", "Vân tay", "Khuôn mặt"};
    std::vector<HV> hvs;
    std::vector<HD> hds; std::vector<std::string> dkRows;
    std::vector<LuotTap> luots; std::vector<Lich> lichs;
    std::map<std::string, std::set<long long>> ptBan, hvBan;
    int soHD = 0, soDK = 0, soLH = 0;
    auto maHD = [&] { return "HD" + pad(++soHD, 4); };
    const int N = 180;
    const std::vector<int> gioTrongSo{0, 0, 0, 0, 0, 2, 8, 9, 5, 3, 2, 2, 3, 2, 2, 3, 5, 9, 10, 9, 6, 3, 1, 0};
    auto chonGio = [&] { int total = 0; for (int w : gioTrongSo) total += w; int r = rnd(1, total); for (int h = 0; h < 24; ++h) { r -= gioTrongSo[static_cast<std::size_t>(h)]; if (r <= 0) return h; } return 18; };
    for (int i = 1; i <= N; ++i) {
        HV h; h.ma = "HV" + pad(i, 3);
        const bool nu = coXacSuat(0.48);
        h.gioiTinh = nu ? "Nữ" : "Nam";
        h.ten = chon(ho) + " " + (nu ? chon(demNu) : chon(demNam)) + " " + (nu ? chon(tenNu) : chon(tenNam));
        h.sdt = "09" + pad(rnd(10000000, 99999999), 8);
        const int tuoi = coXacSuat(0.3) ? rnd(18, 23) : rnd(24, 55);
        h.sinh = Date{std::chrono::year{int(d0.year()) - tuoi}, std::chrono::month{static_cast<unsigned>(rnd(1, 12))}, std::chrono::day{static_cast<unsigned>(rnd(1, 28))}};
        if (i % 37 == 5) h.sinh = Date{std::chrono::year{int(d0.year()) - tuoi}, d0.month(), d0.day()}; // vài người sinh nhật hôm nay
        h.sv = tuoi <= 23 && coXacSuat(0.7);
        h.nghe = h.sv ? "Sinh viên" : chon(nghe);
        if (coXacSuat(0.6)) h.email = "hoivien" + pad(i, 3) + "@example.com";
        if (coXacSuat(0.85)) { h.loaiThe = chon(cach); h.maThe = (h.loaiThe == "Mã vạch" ? "BC" : h.loaiThe == "Vân tay" ? "FP" : h.loaiThe == "Khuôn mặt" ? "FC" : "RF") + pad(1000 + i, 5); }
        h.thamGia = addDays(d0, -rnd(5, 230));
        if (coXacSuat(0.05)) { hvs.push_back(h); continue; } // chỉ tạo hồ sơ, chưa mua gói
        Date start = h.thamGia; bool first = true; int buoiDaDung = 0; int ptTong = 0;
        for (;;) {
            const Goi& g = first && coXacSuat(0.08) ? kGoi[0] : chonGoi();
            const Date end = hetHan(g, start);
            double gia = (h.sv && g.giaSV > 0) ? g.giaSV : g.gia;
            std::string km;
            if (soNgayGiua(start, d0) <= 20 && start <= d0 && coXacSuat(0.5)) { gia *= 0.9; km = "KM0001"; }
            HD hd; hd.ma = maHD(); hd.hv = h.ma; hd.ngay = formatDate(start); hd.loai = "Gói tập"; hd.nv = chon(sales);
            hd.dong.push_back({g.ten.rfind("Gói", 0) == 0 ? g.ten : "Gói tập " + g.ten, 1, gia, ""});
            if (!km.empty()) hd.note = "Khuyến mãi KM0001";
            hd.daTra = gia; hd.no = false;
            if (soNgayGiua(start, d0) <= 40 && coXacSuat(0.08)) { hd.daTra = std::round(gia * 0.5 / 1000) * 1000; hd.no = true; }
            hd.pt = coXacSuat(0.7) ? "Tiền mặt" : chon(std::vector<std::string>{"Chuyển khoản", "Thẻ ngân hàng", "Ví điện tử"});
            hds.push_back(hd);
            h.chiTieu += gia;
            std::ostringstream dk;
            dk << q("DK" + pad(++soDK, 4)) << ',' << q(h.ma) << ',' << q(g.ma) << ',' << q(g.ten) << ',' << q(first ? "Mới" : "Gia hạn") << ',' << q(formatDate(start)) << ',' << q(formatDate(start)) << ','
               << q(formatDate(end)) << ',' << so(g.gia) << ',' << so(gia) << ',' << q(hd.nv) << ',' << q(km) << ',' << q(hd.ma);
            dkRows.push_back(dk.str());
            h.goi = g.ten; h.han = end; h.gioiHan = g.buoi > 0; h.buoi = g.buoi; ptTong = g.buoiPT;
            if (g.buoiPT > 0 && h.maPT.empty()) h.maPT = chon(pts).ma;
            // Lượt tập trong kỳ (chỉ 75 ngày gần nhất để file gọn)
            const double tanSuat = std::uniform_real_distribution<double>(0.18, 0.6)(rng);
            buoiDaDung = 0;
            for (Date d = std::max(start, addDays(d0, -75)); d <= std::min(end, d0); d = addDays(d, 1)) {
                if (h.gioiHan && buoiDaDung >= h.buoi) break;
                if (!coXacSuat(tanSuat)) continue;
                int gio = chonGio(); const int phut = rnd(0, 59);
                DateTime vao = taoThoiDiem(d, gio, phut);
                if (d == d0 && vao >= luc0) continue;
                DateTime ra = vao + std::chrono::minutes{rnd(45, 120)};
                if (d == d0 && ra > luc0) ra = DateTime{};
                luots.push_back({h.ma, vao, ra, h.maThe.empty() ? "Thủ công" : h.loaiThe, ""});
                h.lichSu.push_back(formatDate(d));
                ++buoiDaDung;
            }
            // Buổi PT: 2 buổi/tuần theo ca của PT phụ trách
            if (g.buoiPT > 0) {
                int done = 0;
                const PTx* pt = nullptr; for (const auto& p : pts) if (p.ma == h.maPT) pt = &p;
                for (Date d = std::max(start, addDays(d0, -60)); d <= std::min(end, addDays(d0, 10)) && done < g.buoiPT; d = addDays(d, rnd(2, 4))) {
                    for (int tries = 0; tries < 6; ++tries) {
                        const int gio = rnd(pt->bd, pt->kt - 1);
                        const DateTime bd = taoThoiDiem(d, gio, 0);
                        const long long key = bd.time_since_epoch().count();
                        if (ptBan[pt->ma].contains(key) || hvBan[h.ma].contains(key)) continue;
                        ptBan[pt->ma].insert(key); hvBan[h.ma].insert(key);
                        Lich l{"LH" + pad(++soLH, 4), h.ma, pt->ma, bd, bd + std::chrono::minutes{60}, "Đã đặt", ""};
                        if (bd < luc0) {
                            if (coXacSuat(0.9)) { l.tt = "Hoàn thành"; ++done; } else { l.tt = coXacSuat(0.5) ? "Vắng mặt" : "Đã hủy"; l.note = l.tt == "Đã hủy" ? "Hội viên bận đột xuất" : ""; }
                        } else l.note = chon(std::vector<std::string>{"Tập ngực - vai", "Tập chân - mông", "Cardio HIIT", "Tập lưng - xô", "Core & mobility"});
                        lichs.push_back(l);
                        break;
                    }
                }
                h.buoiPT = std::max(0, ptTong - done);
            } else if (!(h.han >= d0)) h.buoiPT = 0;
            first = false;
            const Date next = addDays(end, 1 + (coXacSuat(0.6) ? 0 : rnd(3, 25)));
            if (next > d0 || !coXacSuat(0.7)) break;
            start = next;
        }
        if (h.gioiHan) h.buoi = std::max(0, h.buoi - buoiDaDung);
        h.trangThai = h.han >= d0 ? "Kích hoạt" : "Hết hạn";
        if (h.trangThai == "Kích hoạt" && coXacSuat(0.05)) { h.trangThai = "Bảo lưu"; h.baoLuu = addDays(d0, -rnd(3, 20)); }
        h.hang = h.chiTieu >= 10000000 ? "Kim cương" : h.chiTieu >= 5000000 ? "Vàng" : h.chiTieu >= 2000000 ? "Bạc" : "Thường";
        if (!h.maPT.empty() && coXacSuat(0.2)) h.ghiChu = "Mục tiêu: giảm 5kg trong 3 tháng";
        hvs.push_back(h);
    }
    // Hội viên bảo lưu không thể đang ở trong phòng
    std::set<std::string> baoLuu; for (const auto& h : hvs) if (h.trangThai == "Bảo lưu") baoLuu.insert(h.ma);
    std::erase_if(luots, [&](const LuotTap& l) { return baoLuu.contains(l.hv) && ngayCua(l.vao) >= addDays(d0, -2); });
    std::sort(luots.begin(), luots.end(), [](const LuotTap& a, const LuotTap& b) { return a.vao < b.vao; });

    // ===== Tủ đồ =====
    struct Tu { std::string ma, khu, tt = "Trống", hv{}; DateTime bd{}; Date het{}; };
    std::vector<Tu> tus;
    for (int i = 1; i <= 12; ++i) tus.push_back({"N" + pad(i, 2), "Nam"});
    for (int i = 1; i <= 12; ++i) tus.push_back({"F" + pad(i, 2), "Nữ"});
    for (int i = 1; i <= 4; ++i) tus.push_back({"V" + pad(i, 2), "VIP"});
    tus[11].tt = "Hỏng";
    std::map<std::string, const HV*> hvMap; for (const auto& h : hvs) hvMap[h.ma] = &h;
    for (auto& l : luots) {
        if (l.ra.time_since_epoch().count() != 0) continue;
        const std::string khu = hvMap[l.hv]->gioiTinh == "Nữ" ? "Nữ" : "Nam";
        for (auto& t : tus) if (t.tt == "Trống" && t.khu == khu) { t.tt = "Đang dùng"; t.hv = l.hv; t.bd = l.vao; l.tu = t.ma; break; }
    }
    {
        int k = 0;
        for (const auto& h : hvs) if (h.trangThai == "Kích hoạt" && h.hang != "Thường" && k < 2) { tus[24 + static_cast<std::size_t>(k)].tt = "Thuê tháng"; tus[24 + static_cast<std::size_t>(k)].hv = h.ma; tus[24 + static_cast<std::size_t>(k)].bd = taoThoiDiem(addDays(d0, -10), 9, 0); tus[24 + static_cast<std::size_t>(k)].het = addDays(d0, 20 - 18 * k); ++k; }
    }

    // ===== Kho & bán lẻ =====
    struct SP { std::string ma, ten, dm, dv; double ban, nhap; int ton = 0, min; int batch; };
    std::vector<SP> sps{
        {"SP001", "Nước suối Aquafina 500ml", "Nước uống", "chai", 10000, 5000, 0, 24, 96},
        {"SP002", "Nước điện giải Pocari 500ml", "Nước uống", "chai", 20000, 13000, 0, 12, 48},
        {"SP003", "Revive chanh muối", "Nước uống", "chai", 15000, 9000, 0, 12, 48},
        {"SP004", "Whey Protein Isolate (1 lần dùng)", "Thực phẩm bổ sung", "gói", 45000, 28000, 0, 10, 40},
        {"SP005", "Whey Gold Standard 2.27kg", "Thực phẩm bổ sung", "hộp", 1890000, 1550000, 0, 1, 2},
        {"SP006", "BCAA Xtend 30 lần", "Thực phẩm bổ sung", "hũ", 650000, 480000, 0, 1, 3},
        {"SP007", "Thanh protein bar", "Đồ ăn kiêng", "thanh", 35000, 22000, 0, 10, 40},
        {"SP008", "Ức gà sous-vide 200g", "Đồ ăn kiêng", "hộp", 55000, 35000, 0, 8, 30},
        {"SP009", "Salad ức gà", "Đồ ăn kiêng", "hộp", 65000, 40000, 0, 6, 20},
        {"SP010", "Khăn tập microfiber", "Phụ kiện", "cái", 60000, 30000, 0, 5, 20},
        {"SP011", "Găng tay tập gym", "Phụ kiện", "đôi", 180000, 110000, 0, 3, 10},
        {"SP012", "Bình lắc shaker 700ml", "Phụ kiện", "cái", 90000, 50000, 0, 4, 15},
    };
    std::vector<std::string> pkRows, cpRows; int soPK = 0, soCP = 0;
    auto chi = [&](Date d, const std::string& nd, double tien, const std::string& dm) {
        if (d > d0) return;
        std::ostringstream o; o << q("CP" + pad(++soCP, 4)) << ',' << q(nd) << ',' << q(formatDate(d)) << ',' << so(tien) << ',' << q(dm); cpRows.push_back(o.str());
    };
    const Date batDauKho = addDays(d0, -150);
    std::vector<std::string> leTan{"NV002", "NV003", "NV002", "NV004"};
    for (Date d = batDauKho; d <= d0; d = addDays(d, 1)) {
        const int off = soNgayGiua(batDauKho, d);
        if (off % 30 == 0) for (auto& s : sps) {
            if ((s.ma == "SP005" || s.ma == "SP009") && soNgayGiua(d, d0) < 30) continue; // để 2 mặt hàng sắp hết
            const int sl = s.batch;
            std::ostringstream o; o << q("PK" + pad(++soPK, 4)) << ',' << q(s.ma) << ',' << q(formatDate(d)) << ',' << q("Nhập") << ',' << sl << ',' << so(s.nhap) << ',' << q("NCC: Công ty Thể Thao Việt");
            pkRows.push_back(o.str()); s.ton += sl;
            chi(d, "Nhập " + std::to_string(sl) + " " + s.dv + " " + s.ten, sl * s.nhap, "Nhập hàng");
        }
        const int soDon = rnd(0, 5);
        for (int k = 0; k < soDon; ++k) {
            HD hd; hd.ma = maHD(); hd.ngay = formatDate(d); hd.loai = "Bán lẻ"; hd.nv = chon(leTan); hd.pt = coXacSuat(0.6) ? "Tiền mặt" : "Chuyển khoản";
            hd.hv = coXacSuat(0.6) ? hvs[static_cast<std::size_t>(rnd(0, N - 1))].ma : "KHACH_LE";
            const int mon = rnd(1, 3); double tong = 0;
            std::set<std::string> used;
            for (int m = 0; m < mon; ++m) {
                auto& s = sps[static_cast<std::size_t>(coXacSuat(0.6) ? rnd(0, 3) : rnd(0, 11))];
                if (used.contains(s.ma)) continue;
                const int sl = s.ban > 500000 ? 1 : rnd(1, 3);
                if (s.ton < sl) continue;
                s.ton -= sl; used.insert(s.ma);
                hd.dong.push_back({s.ten, sl, s.ban, s.ma}); tong += sl * s.ban;
                std::ostringstream o; o << q("PK" + pad(++soPK, 4)) << ',' << q(s.ma) << ',' << q(formatDate(d)) << ',' << q("Xuất bán") << ',' << sl << ',' << so(s.ban) << ',' << q("Hóa đơn " + hd.ma);
                pkRows.push_back(o.str());
            }
            if (hd.dong.empty()) { --soHD; continue; }
            hd.daTra = tong;
            hds.push_back(hd);
        }
    }
    // Hóa đơn thuê tủ tháng
    for (const auto& t : tus) if (t.tt == "Thuê tháng") {
        HD hd; hd.ma = maHD(); hd.hv = t.hv; hd.ngay = formatDate(addDays(d0, -10)); hd.loai = "Dịch vụ"; hd.pt = "Tiền mặt";
        hd.dong.push_back({"Thuê tủ " + t.ma + " 1 tháng", 1, 100000, ""}); hd.daTra = 100000; hds.push_back(hd);
    }
    // Buổi PT lẻ đã hoàn thành của hội viên hết buổi (ghi công nợ)
    std::sort(hds.begin(), hds.end(), [](const HD& a, const HD& b) { return a.ngay < b.ngay; });
    {   // đánh lại số hóa đơn theo thời gian
        std::map<std::string, std::string> doiMa; int n = 0;
        for (auto& h : hds) { const auto moi = "HD" + pad(++n, 4); doiMa[h.ma] = moi; h.ma = moi; }
        for (auto& row : dkRows) { const auto p = row.rfind(",\"HD"); const auto cu = row.substr(p + 2, row.size() - p - 3); row = row.substr(0, p + 1) + q(doiMa[cu]); }
        for (auto& row : pkRows) { const auto p = row.find("Hóa đơn HD"); if (p != std::string::npos) { const auto cu = row.substr(p + 11, 6); row.replace(p + 11, 6, doiMa[cu]); } }
    }

    // ===== Chi phí cố định hằng tháng =====
    for (int m = 7; m >= 0; --m) {
        int y = int(d0.year()), mo = static_cast<int>(unsigned(d0.month())) - m; while (mo <= 0) { mo += 12; --y; }
        const Date dau{std::chrono::year{y}, std::chrono::month{static_cast<unsigned>(mo)}, std::chrono::day{1}};
        const std::string th = std::to_string(mo) + "/" + std::to_string(y);
        chi(dau, "Tiền thuê mặt bằng tháng " + th, 15000000, "Mặt bằng");
        chi(addDays(dau, 2), "Điện nước tháng " + th, rnd(42, 60) * 100000.0, "Điện nước");
        chi(addDays(dau, 4), "Lương nhân viên tháng " + th, 34000000, "Lương");
        chi(addDays(dau, 9), "Quảng cáo Facebook/TikTok tháng " + th, rnd(20, 45) * 100000.0, "Marketing");
        chi(addDays(dau, 14), "Vật tư vệ sinh tháng " + th, rnd(6, 12) * 100000.0, "Vệ sinh");
    }

    // ===== Thiết bị =====
    struct TB { std::string ma, ten, khu; int tuoiNgay; int bh; int baoTriSau; double gia; std::string ncc, tt; };
    const std::vector<TB> tbs{
        {"TB001", "Máy chạy bộ Impulse AC2990", "Khu cardio", 700, 1095, -3, 85000000, "Impulse Việt Nam", "Hoạt động"},
        {"TB002", "Máy chạy bộ Impulse AC2990", "Khu cardio", 700, 1095, 25, 85000000, "Impulse Việt Nam", "Hoạt động"},
        {"TB003", "Xe đạp tập Spinning", "Khu cardio", 500, 730, 12, 18000000, "Elip Sport", "Hoạt động"},
        {"TB004", "Máy chèo thuyền Concept2", "Khu cardio", 300, 730, 40, 32000000, "Concept2", "Hoạt động"},
        {"TB005", "Giàn tạ đa năng Smith", "Khu máy", 800, 1095, 5, 46000000, "Life Fitness", "Hoạt động"},
        {"TB006", "Máy đạp đùi Leg Press", "Khu máy", 800, 1095, -10, 39000000, "Life Fitness", "Hỏng"},
        {"TB007", "Máy kéo xô Lat Pulldown", "Khu máy", 650, 1095, 60, 28000000, "Life Fitness", "Hoạt động"},
        {"TB008", "Bộ tạ đơn 2-40kg", "Khu tạ tự do", 900, 365, 90, 36000000, "Elip Sport", "Hoạt động"},
        {"TB009", "Ghế tập đa năng", "Khu tạ tự do", 400, 365, 2, 6500000, "Elip Sport", "Hoạt động"},
        {"TB010", "Máy InBody 570", "Quầy lễ tân", 365, 730, 30, 180000000, "InBody Việt Nam", "Hoạt động"},
        {"TB011", "Thảm yoga cao su (20 tấm)", "Phòng yoga", 200, 180, 45, 9000000, "Yoga Plus", "Hoạt động"},
        {"TB012", "Hệ thống âm thanh phòng group-X", "Phòng group-X", 600, 365, 15, 25000000, "Sound Pro", "Đang bảo trì"},
        {"TB013", "Cổng kiểm soát ra vào (thẻ từ + vân tay)", "Quầy lễ tân", 500, 730, 20, 42000000, "ZKTeco", "Hoạt động"},
        {"TB014", "Máy giặt khăn công nghiệp", "Phòng thay đồ", 450, 365, -1, 15000000, "Electrolux", "Hoạt động"},
    };
    std::vector<std::string> btRows;
    for (const auto& t : tbs) {
        const Date nhap = addDays(d0, -t.tuoiNgay);
        for (int k = 1; k * 90 < t.tuoiNgay && k <= 4; ++k) {
            const Date d = addDays(nhap, k * 90);
            const double cp = d <= addDays(nhap, t.bh) ? 0 : rnd(3, 15) * 100000.0;
            std::ostringstream o; o << q(t.ma) << ',' << q(formatDate(d)) << ',' << q(chon(std::vector<std::string>{"Bảo dưỡng định kỳ, tra dầu", "Thay dây curoa", "Siết ốc, căn chỉnh", "Vệ sinh động cơ"})) << ',' << so(cp) << ',' << q("Kỹ thuật NCC");
            btRows.push_back(o.str());
            if (cp > 0) chi(d, "Bảo trì " + t.ten, cp, "Bảo trì");
        }
    }

    // ===== Ghi file =====
    auto csvHV = [&] {
        Csv f(out / "hoi_vien.csv", "ma,ten,sdt,ngay_sinh,sinh_vien,ngay_het_han,so_buoi,gioi_han_buoi,trang_thai,lich_su_checkin,gioi_tinh,nghe_nghiep,email,hang,ma_the,loai_the,ma_pt,goi_hien_tai,ngay_tham_gia,ngay_bao_luu,so_buoi_pt,ghi_chu");
        for (const auto& h : hvs) {
            std::string ls; for (const auto& s : h.lichSu) { if (!ls.empty()) ls += ';'; ls += s; }
            f.dong(q(h.ma), q(h.ten), q(h.sdt), q(ngay(h.sinh)), h.sv ? 1 : 0, q(ngay(h.han)), h.buoi, h.gioiHan ? 1 : 0, q(h.trangThai), q(ls), q(h.gioiTinh), q(h.nghe), q(h.email), q(h.hang),
                   q(h.maThe), q(h.loaiThe), q(h.maPT), q(h.goi), q(ngay(h.thamGia)), q(ngay(h.baoLuu)), h.buoiPT, q(h.ghiChu));
        }
    };
    csvHV();
    {
        Csv f(out / "nhan_vien.csv", "ma,ten,chuc_vu,sdt,luong_co_ban,ty_le_hoa_hong,ngay_vao_lam,dang_lam");
        for (const auto& n : nvs) f.dong(q(n.ma), q(n.ten), q(n.chucVu), q(n.sdt), so(n.luong), n.hh, q(formatDate(n.vao)), 1);
    }
    for (const auto& l : lichs) if (l.tt == "Hoàn thành") for (auto& p : pts) if (p.ma == l.pt) { ++p.soCa; p.dt += p.giaHV; }
    // ===== Chấm công 45 ngày, Chủ nhật nghỉ =====
    std::vector<std::string> ccRows;
    auto chamCong = [&](const std::string& ma, int bd, int kt) {
        int n = 0;
        for (Date d = addDays(d0, -45); d <= d0; d = addDays(d, 1)) {
            if (thuTrongTuan(d) == 0 || coXacSuat(0.06)) continue;
            const DateTime vao = taoThoiDiem(d, bd, rnd(0, 20)) - std::chrono::minutes{10};
            if (vao > luc0) continue;
            DateTime ra = taoThoiDiem(d, std::min(kt, 23), rnd(0, 25));
            if (ra > luc0) ra = DateTime{};
            ccRows.push_back(q(ma) + "," + q(luc(vao)) + "," + q(luc(ra))); ++n;
        }
        return n;
    };
    for (const auto& n : nvs) chamCong(n.ma, n.ma == "NV003" ? 14 : 7, n.ma == "NV003" ? 22 : 16);
    for (auto& p : pts) p.cong = chamCong(p.ma, p.bd, p.kt);
    {
        Csv f(out / "pt.csv", "ma,ten,chuyen_mon,sdt,luong_co_ban,don_gia_buoi,ty_le_hoa_hong,gio_bat_dau,gio_ket_thuc,gia_buoi_hoi_vien,so_ca,doanh_thu,cham_cong,ngay_vao_lam,dang_lam");
        for (const auto& p : pts) f.dong(q(p.ma), q(p.ten), q(p.mon), q(p.sdt), so(p.luong), so(p.donGia), p.hh, p.bd, p.kt, so(p.giaHV), p.soCa, so(p.dt), p.cong, q(formatDate(addDays(d0, -rnd(200, 800)))), 1);
    }
    { Csv f(out / "cham_cong.csv", "ma_nv,vao,ra"); for (const auto& r : ccRows) f.f << r << '\n'; }
    {
        Csv f(out / "goi_dich_vu.csv", "ma,ten,loai,so_thang,so_ngay,so_buoi,so_buoi_pt,gia,gia_sv,dang_ban");
        for (const auto& g : kGoi) f.dong(q(g.ma), q(g.ten), q(g.loai), g.thang, g.ngay, g.buoi, g.buoiPT, so(g.gia), so(g.giaSV), 1);
    }
    {
        Csv f(out / "khuyen_mai.csv", "ma,ten,loai_giam,gia_tri,tu_ngay,den_ngay,tu_dong,ma_goi,so_luot_con,ma_hoi_vien,chi_hv_moi,da_dung");
        int dung = 0; for (const auto& r : dkRows) if (r.find("\"KM0001\"") != std::string::npos) ++dung;
        f.dong(q("KM0001"), q("Ưu đãi mùa thu - giảm 10% mọi gói"), q("phan_tram"), 10, q(formatDate(addDays(d0, -20))), q(formatDate(addDays(d0, 15))), 1, q(""), -1, q(""), 0, dung);
        f.dong(q("BLACKFRIDAY"), q("Black Friday giảm 15% gói 3 tháng"), q("phan_tram"), 15, q(formatDate(addDays(d0, 5))), q(formatDate(addDays(d0, 35))), 0, q("GT03"), 200, q(""), 0, 0);
        f.dong(q("HSSV200"), q("Tân hội viên HSSV giảm 200.000đ"), q("tien_mat"), 200000, q(formatDate(addDays(d0, -60))), q(formatDate(addDays(d0, 60))), 0, q(""), 50, q(""), 1, 3);
        f.dong(q("HE2026"), q("Chào hè 2026 giảm 20%"), q("phan_tram"), 20, q(formatDate(addDays(d0, -150))), q(formatDate(addDays(d0, -90))), 1, q(""), -1, q(""), 0, 0);
    }
    { Csv f(out / "dang_ky.csv", "ma,ma_hoi_vien,ma_goi,ten_goi,loai,ngay,tu_ngay,den_ngay,gia_goc,phi,ma_nhan_vien,ma_km,ma_hoa_don"); for (const auto& r : dkRows) f.f << r << '\n'; }
    {
        Csv f(out / "hoa_don.csv", "ma,ma_hoi_vien,ngay,ten_hang,so_luong,don_gia,ma_sp,loai,phuong_thuc,da_thanh_toan,co_cong_no,giam_gia,ma_nhan_vien,ghi_chu");
        for (const auto& h : hds) for (const auto& [ten, sl, gia, sp] : h.dong)
            f.dong(q(h.ma), q(h.hv), q(h.ngay), q(ten), sl, so(gia), q(sp), q(h.loai), q(h.pt), so(h.daTra), h.no ? 1 : 0, so(h.giam), q(h.nv), q(h.note));
    }
    { Csv f(out / "chi_phi.csv", "ma,noi_dung,ngay,so_tien,danh_muc"); for (const auto& r : cpRows) f.f << r << '\n'; }
    {
        Csv f(out / "thu_khac.csv", "ma,noi_dung,ngay,so_tien,danh_muc");
        f.dong(q("TK0001"), q("Cho thuê phòng group-X tổ chức sự kiện"), q(formatDate(addDays(d0, -40))), 3000000, q("Cho thuê"));
        f.dong(q("TK0002"), q("Tài trợ giải chạy cộng đồng"), q(formatDate(addDays(d0, -12))), 5000000, q("Tài trợ"));
    }
    {
        Csv f(out / "check_in.csv", "ma_hoi_vien,vao,ra,phuong_thuc,ma_tu");
        for (const auto& l : luots) f.dong(q(l.hv), q(luc(l.vao)), q(luc(l.ra)), q(l.cach), q(l.tu));
    }
    {
        Csv f(out / "lich_hen.csv", "ma,ma_hoi_vien,ma_pt,bat_dau,ket_thuc,trang_thai,ghi_chu");
        std::sort(lichs.begin(), lichs.end(), [](const Lich& a, const Lich& b) { return a.bd < b.bd; });
        int n = 0; for (auto& l : lichs) l.ma = "LH" + pad(++n, 4);
        for (const auto& l : lichs) f.dong(q(l.ma), q(l.hv), q(l.pt), q(luc(l.bd)), q(luc(l.kt)), q(l.tt), q(l.note));
        Csv nk(out / "nhat_ky_tap.csv", "ma_hoi_vien,ngay,noi_dung,thoi_luong,ma_pt");
        for (const auto& l : lichs) if (l.tt == "Hoàn thành") {
            std::string ten; for (const auto& p : pts) if (p.ma == l.pt) ten = p.ten;
            nk.dong(q(l.hv), q(formatDate(ngayCua(l.bd))), q("Buổi tập cùng PT " + ten + (l.note.empty() ? "" : ": " + l.note)), 60, q(l.pt));
        }
        for (int i = 0; i < 40; ++i) {
            const auto& h = hvs[static_cast<std::size_t>(rnd(0, N - 1))]; if (h.lichSu.empty()) continue;
            nk.dong(q(h.ma), q(h.lichSu.back()), q(chon(std::vector<std::string>{"Chạy bộ 5km + giãn cơ", "Squat 5x5 80kg, deadlift 3x5", "Yoga flow 45 phút", "Bench press 4x8, kéo xà", "HIIT 30 phút + plank"})), rnd(40, 90), q(""));
        }
    }
    {
        Csv f(out / "san_pham.csv", "ma,ten,danh_muc,don_vi,gia_ban,gia_nhap,ton_kho,muc_toi_thieu,dang_ban");
        for (const auto& s : sps) f.dong(q(s.ma), q(s.ten), q(s.dm), q(s.dv), so(s.ban), so(s.nhap), s.ton, s.min, 1);
        Csv p(out / "phieu_kho.csv", "ma,ma_sp,ngay,loai,so_luong,don_gia,ghi_chu");
        for (const auto& r : pkRows) p.f << r << '\n';
    }
    {
        Csv f(out / "thiet_bi.csv", "ma,ten,khu_vuc,ngay_nhap,han_bao_hanh,ngay_bao_tri,gia_tri,nha_cung_cap,trang_thai");
        for (const auto& t : tbs) f.dong(q(t.ma), q(t.ten), q(t.khu), q(formatDate(addDays(d0, -t.tuoiNgay))), q(formatDate(addDays(d0, t.bh - t.tuoiNgay))), q(formatDate(addDays(d0, t.baoTriSau))), so(t.gia), q(t.ncc), q(t.tt));
        Csv b(out / "bao_tri.csv", "ma_tb,ngay,noi_dung,chi_phi,nguoi_thuc_hien");
        for (const auto& r : btRows) b.f << r << '\n';
    }
    {
        Csv f(out / "ve_sinh.csv", "khu_vuc,nguoi_kiem_tra,ngay,dat,ghi_chu");
        const std::vector<std::string> khu{"Khu tạ tự do", "Khu cardio", "Phòng thay đồ", "Nhà vệ sinh", "Phòng yoga"};
        for (int i = 10; i >= 0; --i) for (int k = 0; k < 2; ++k) {
            const bool dat = coXacSuat(0.88);
            f.dong(q(chon(khu)), q(chon(std::vector<std::string>{"Lâm Minh Tuấn", "Đinh Thu Trang"})), q(formatDate(addDays(d0, -i))), dat ? 1 : 0, q(dat ? "Sạch sẽ" : "Sàn còn ướt, thiếu giấy"));
        }
    }
    {
        Csv f(out / "tu_do.csv", "ma,khu_vuc,trang_thai,ma_hoi_vien,bat_dau,het_han_thue");
        for (const auto& t : tus) f.dong(q(t.ma), q(t.khu), q(t.tt), q(t.hv), q(luc(t.bd)), q(ngay(t.het)));
    }
    {
        Csv f(out / "phan_hoi.csv", "ma,ma_hoi_vien,ma_doi_tuong,diem,noi_dung,ngay,trang_thai,xu_ly");
        const std::vector<std::pair<int, std::string>> nd{{5, "PT rất nhiệt tình, chỉnh form kỹ"}, {5, "Phòng sạch, nhạc hay"}, {4, "Giáo án phù hợp, tiến bộ rõ"}, {4, "Lễ tân thân thiện"},
                                                          {3, "Giờ cao điểm hơi đông, chờ máy lâu"}, {2, "Máy đạp đùi hỏng nhiều ngày chưa sửa"}, {1, "Phòng thay đồ thiếu nước nóng"}, {5, "Tư vấn gói rõ ràng"}};
        for (int i = 1; i <= 28; ++i) {
            const auto& [diem, text] = nd[static_cast<std::size_t>(i % 3 == 0 ? rnd(0, 7) : rnd(0, 3))];
            std::string dt = diem <= 3 && coXacSuat(0.6) ? "PHONG_TAP" : coXacSuat(0.65) ? pts[static_cast<std::size_t>(rnd(0, 4))].ma : nvs[static_cast<std::size_t>(rnd(1, 4))].ma;
            const bool xl = coXacSuat(0.6) || i < 10;
            f.dong(q("PH" + pad(i, 4)), q(hvs[static_cast<std::size_t>(rnd(0, N - 1))].ma), q(dt), diem, q(text), q(formatDate(addDays(d0, -rnd(0, 60) * (i > 24 ? 0 : 1)))), q(xl ? "Đã xử lý" : "Mới"), q(xl ? "Đã gọi điện cảm ơn/xin lỗi khách" : ""));
        }
    }
    {
        Csv f(out / "inbody.csv", "ma_hoi_vien,ngay,chieu_cao,can_nang,mo,co");
        for (int i = 0; i < 35; ++i) {
            const auto& h = hvs[static_cast<std::size_t>(rnd(0, N - 1))];
            const bool nu = h.gioiTinh == "Nữ";
            double cao = nu ? rnd(150, 168) : rnd(162, 182), nang = nu ? rnd(45, 68) : rnd(58, 90), mo = nu ? rnd(22, 34) : rnd(14, 28);
            for (int k = 2; k >= 0; --k) { f.dong(q(h.ma), q(formatDate(addDays(d0, -30 * k - rnd(0, 5)))), so(cao), nang, mo, std::round(nang * (nu ? 0.36 : 0.43))); nang -= rnd(0, 2); mo -= rnd(0, 2) * 0.5; }
        }
    }
    {
        Csv f(out / "cskh.csv", "ma_hoi_vien,loai,noi_dung,ngay,nguoi_ghi");
        for (int i = 0; i < 15; ++i) {
            const auto& h = hvs[static_cast<std::size_t>(rnd(0, N - 1))];
            f.dong(q(h.ma), q(chon(std::vector<std::string>{"Gọi điện", "Gia hạn", "Ưu đãi"})), q(chon(std::vector<std::string>{"Đã nhắc gia hạn, khách hẹn cuối tuần", "Giới thiệu gói PT kèm ưu đãi", "Hỏi thăm lý do vắng tập"})), q(formatDate(addDays(d0, -rnd(1, 40)))), q("cskh"));
        }
    }
    {
        Csv f(out / "tai_khoan.csv", "ten_dang_nhap,muoi,bam,vai_tro,ma_nhan_vien,khoa");
        const std::vector<std::tuple<std::string, std::string, VaiTro, std::string>> tks{
            {"admin", "admin123", VaiTro::Admin, ""}, {"quanly", "quanly123", VaiTro::QuanLy, "NV001"}, {"letan", "letan123", VaiTro::LeTan, "NV002"},
            {"sale", "sale123", VaiTro::Sale, "NV004"}, {"cskh", "cskh123", VaiTro::LeTan, "NV006"}, {"pt", "pt123", VaiTro::PT, "PT001"}};
        for (const auto& [ten, mk, vt, nv] : tks) { TaiKhoan t(ten, mk, vt, nv); f.dong(q(t.tenDangNhap()), q(t.muoi()), q(t.bam()), q(tenVaiTro(vt)), q(nv), 0); }
    }
    {
        Csv f(out / "cai_dat.csv", "khoa,gia_tri");
        f.dong(q("ten_phong_tap"), q("Ocean Fitness Center")); f.dong(q("dia_chi"), q("123 Nguyễn Văn Linh, Q.7, TP.HCM")); f.dong(q("hotline"), q("1900 6868"));
        f.dong(q("cong_chuan"), q("26")); f.dong(q("gia_thue_tu"), q("100000")); f.dong(q("phi_chuyen_nhuong"), q("200000")); f.dong(q("qua_sinh_nhat_phan_tram"), q("20")); f.dong(q("uu_dai_theo_hang"), q("0"));
    }
    std::cout << "Đã tạo dữ liệu mẫu tại " << out.string() << ": " << hvs.size() << " hội viên, " << hds.size() << " hóa đơn, " << luots.size() << " lượt ra vào, " << lichs.size() << " lịch PT.\n";
}
