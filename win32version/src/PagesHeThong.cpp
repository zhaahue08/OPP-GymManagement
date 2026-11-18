#include "App.h"
#include <map>
namespace gui {
namespace {
const std::vector<std::wstring> kVaiTro{L"Quản trị", L"Quản lý", L"Lễ tân", L"Sale", L"Huấn luyện viên"};
const std::map<std::string, std::pair<std::wstring, std::string>>& moTaCaiDat() {
    static const std::map<std::string, std::pair<std::wstring, std::string>> m{
        {"ten_phong_tap", {L"Tên phòng tập (in trên hóa đơn)", "Fitness CRM"}},
        {"dia_chi", {L"Địa chỉ", ""}},
        {"hotline", {L"Hotline", ""}},
        {"cong_chuan", {L"Số ngày công chuẩn / tháng", "26"}},
        {"gia_thue_tu", {L"Giá thuê tủ đồ / tháng (đ)", "100000"}},
        {"phi_chuyen_nhuong", {L"Phí chuyển nhượng gói mặc định (đ)", "200000"}},
        {"qua_sinh_nhat_phan_tram", {L"Voucher sinh nhật mặc định (%)", "20"}},
        {"uu_dai_theo_hang", {L"Ưu đãi theo hạng thành viên (1 = bật)", "0"}},
    };
    return m;
}
std::wstring quyenMoTa(crm::VaiTro v) {
    switch (v) {
        //Quản lý cao nhất
        case crm::VaiTro::Admin: return L"Toàn quyền, quản lý tài khoản & cài đặt";
        //Quản lý nhân sự
        case crm::VaiTro::QuanLy: return L"Mọi nghiệp vụ, báo cáo, nhân sự, lương";
        //Lễ tân gym
        case crm::VaiTro::LeTan: return L"Hội viên, check-in, POS, kho, tủ đồ, CSKH";
        //Sale
        case crm::VaiTro::Sale: return L"Hội viên, gói tập & KM, POS, CSKH, báo cáo";
        //PT
        case crm::VaiTro::PT: return L"Lịch PT của mình, hội viên, chấm công";
    }
    return L"";
}
}
<<<<<<< HEAD
=======
//Thêm trang hệ thống mới
>>>>>>> 32a5f555dea8531a139fed5011dbfe899248b83d
void themTrangHeThong(std::vector<Trang>& ds) {
    Trang t;
    t.ten = L"Tài khoản & cài đặt"; t.bieuTuong = L""; t.quyen = Q_ADMIN;
    t.moTa = L"Phân quyền truy cập theo vai trò, khóa/mở tài khoản, đổi mật khẩu và các thông số vận hành của phòng tập.";
    t.boLoc = {L"Tài khoản đăng nhập", L"Cài đặt hệ thống"};
    t.cot = [](int f) -> std::vector<Cot> {
        if (f == 1) return {{L"Khóa", 190}, {L"Mô tả", 300}, {L"Giá trị", 320}};
        return {{L"Tên đăng nhập", 150}, {L"Vai trò", 130}, {L"Nhân viên", 220}, {L"Quyền truy cập", 330}, {L"Trạng thái", 100}};
    };
    t.dong = [](int f) {
        std::vector<Dong> out;
        if (f == 1) {
            for (const auto& [k, v] : moTaCaiDat()) out.push_back({W(k), v.first, W(C().caiDat(k, v.second))});
            return out;
        }
        for (const auto& tk : C().taiKhoans())
            out.push_back({W(tk.tenDangNhap()), W(crm::tenVaiTro(tk.vaiTro())), tk.maNhanVien().empty() ? L"" : W(tk.maNhanVien()) + L" " + tenNhanSu(tk.maNhanVien()), quyenMoTa(tk.vaiTro()), tk.khoa() ? L"Đã khóa" : L"Hoạt động"});
        return out;
    };
    t.mauChu = [](const Dong& r, int f) -> COLORREF { return f == 0 && r[4] == L"Đã khóa" ? mau::xam : CLR_DEFAULT; };
    auto suaCaiDat = [] {
        const auto key = chon();
        const auto& m = moTaCaiDat();
        auto it = m.find(key);
        std::vector<Truong> f{{L"Cài đặt", Kieu::ChiDoc, it == m.end() ? W(key) : it->second.first}, {L"Giá trị", Kieu::Chu, W(C().caiDat(key, it == m.end() ? "" : it->second.second))}};
        if (moForm(app().chinh, L"Sửa cài đặt", f, [&](const KetQua& k) {
            if ((key == "cong_chuan" || key == "gia_thue_tu" || key == "phi_chuyen_nhuong" || key == "qua_sinh_nhat_phan_tram") && (k.rong(1) || docSo(k.wchu(1)) <= 0)) throw std::runtime_error("Giá trị phải là số dương.");
            C().datCaiDat(key, key == "ten_phong_tap" || key == "dia_chi" || key == "hotline" ? k.chu(1) : U(thapPhan(docSo(k.wchu(1)), 0)));
        })) daThayDoi();
    };
    t.moDong = [suaCaiDat](int f) { if (f == 1) suaCaiDat(); };
    t.thaoTac = {
        {L"＋ Thêm tài khoản", [] {
            std::vector<Truong> f{{L"Tên đăng nhập", Kieu::Chu, L"", {}, true, L"Không dấu, không khoảng trắng"}, {L"Mật khẩu", Kieu::MatKhau, L"", {}, true},
                                  {L"Vai trò", Kieu::Chon, L"Lễ tân", kVaiTro}, {L"Gắn với nhân viên", Kieu::Chon, L"(Không)", dsNhanSu(true)}};
            if (moForm(app().chinh, L"Tạo tài khoản", f, [&](const KetQua& k) {
                C().themTaiKhoan(crm::TaiKhoan(k.chu(0), k.chu(1), crm::vaiTroTuTen(k.chu(2)), k.chon(3) <= 0 ? std::string{} : k.ma(3)));
            }, L"Tạo")) daThayDoi();
        }, {0}, false, true},
        {L"Sửa vai trò", [] {
            auto& tk = C().taiKhoan(chon());
            std::vector<Truong> f{{L"Tài khoản", Kieu::ChiDoc, W(tk.tenDangNhap())}, {L"Vai trò", Kieu::Chon, W(crm::tenVaiTro(tk.vaiTro())), kVaiTro}, {L"Gắn với nhân viên", Kieu::Chon, timTrongDs(dsNhanSu(true), tk.maNhanVien()), dsNhanSu(true)}};
            if (moForm(app().chinh, L"Phân quyền", f, [&](const KetQua& k) {
                if (tk.tenDangNhap() == app().tenDangNhap && crm::vaiTroTuTen(k.chu(1)) != crm::VaiTro::Admin) throw std::runtime_error("Không thể tự hạ quyền tài khoản đang đăng nhập.");
                tk.datVaiTro(crm::vaiTroTuTen(k.chu(1)), k.chon(2) <= 0 ? std::string{} : k.ma(2));
            })) daThayDoi();
        }, {0}},
        {L"Đặt lại mật khẩu", [] {
            auto& tk = C().taiKhoan(chon());
            std::vector<Truong> f{{L"Tài khoản", Kieu::ChiDoc, W(tk.tenDangNhap())}, {L"Mật khẩu mới", Kieu::MatKhau, L"", {}, true}};
            if (moForm(app().chinh, L"Đặt lại mật khẩu", f, [&](const KetQua& k) { tk.doiMatKhau(k.chu(1)); })) { daThayDoi(); thongBao(L"Đã đặt lại mật khẩu."); }
        }, {0}},
        {L"Khóa / mở khóa", [] {
            auto& tk = C().taiKhoan(chon());
            if (tk.tenDangNhap() == app().tenDangNhap) throw std::runtime_error("Không thể khóa tài khoản đang đăng nhập.");
            tk.datKhoa(!tk.khoa()); daThayDoi();
        }, {0}},
        {L"Xóa tài khoản", [] {
            const auto ten = chon();
            if (ten == app().tenDangNhap) throw std::runtime_error("Không thể xóa tài khoản đang đăng nhập.");
            if (hoi(L"Xóa tài khoản " + W(ten) + L"?")) { C().xoaTaiKhoan(ten); daThayDoi(); }
        }, {0}},
        {L"Sửa cài đặt", suaCaiDat, {1}, false, true},
    };
    ds.push_back(std::move(t));
}
}


