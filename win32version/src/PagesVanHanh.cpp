// Trang Thiết bị & vệ sinh, Tủ đồ.
#include "App.h"

namespace gui {
namespace {
const std::vector<std::wstring> kKhuVuc{L"Khu tạ tự do", L"Khu máy", L"Khu cardio", L"Phòng yoga", L"Phòng group-X", L"Khu functional", L"Phòng thay đồ", L"Khác"};
const std::vector<std::wstring> kTinhTrang{L"Hoạt động", L"Đang bảo trì", L"Hỏng", L"Thanh lý"};

void formThietBi(const crm::ThietBi* tb) {
    std::vector<Truong> f{
        {L"Mã thiết bị", tb ? Kieu::ChiDoc : Kieu::Chu, tb ? W(tb->ma()) : W(C().sinhMa("TB")), {}, true},
        {L"Tên thiết bị", Kieu::Chu, tb ? W(tb->ten()) : L"", {}, true, L"VD: Máy chạy bộ Impulse AC2990"},
        {L"Khu vực", Kieu::ChonSua, tb ? W(tb->khuVuc()) : kKhuVuc[1], kKhuVuc},
        {L"Ngày nhập", Kieu::Ngay, tb ? W(crm::formatDate(tb->ngayNhap())) : W(crm::formatDate(crm::today())), {}, true},
        {L"Hạn bảo hành", Kieu::Ngay, tb && tb->hanBaoHanh().ok() ? W(crm::formatDate(tb->hanBaoHanh())) : L""},
        {L"Bảo trì kế tiếp", Kieu::Ngay, tb ? W(crm::formatDate(tb->ngayBaoTri())) : W(crm::formatDate(crm::addMonths(crm::today(), 3))), {}, true},
        {L"Giá trị (đ)", Kieu::Tien, tb ? tien(tb->giaTri()) : L"0"},
        {L"Nhà cung cấp", Kieu::Chu, tb ? W(tb->nhaCungCap()) : L""},
    };
    if (!moForm(app().chinh, tb ? L"Sửa thiết bị" : L"Thêm thiết bị", f, [&](const KetQua& k) {
        if (tb) { auto& x = C().thietBi(tb->ma()); x.capNhat(k.chu(1), k.chu(2), k.ngay(3), k.ngay(4), k.so(6), k.chu(7)); x.lenLichBaoTri(k.ngay(5)); }
        else C().themThietBi(crm::ThietBi(k.chu(0), k.chu(1), k.chu(2), k.ngay(3), k.ngay(4), k.ngay(5), k.so(6), k.chu(7)));
    })) return;
    daThayDoi();
}
COLORREF mauTu(const std::string& tt) {
    if (tt == "Trống") return mau::xanhLaNhat;
    if (tt == "Đang dùng") return mau::xanhNhat;
    if (tt == "Thuê tháng") return mau::timNhat;
    return mau::doNhat;
}
}

void themTrangVanHanh(std::vector<Trang>& ds) {
    {
        Trang t;
        t.ten = L"Thiết bị & vệ sinh"; t.bieuTuong = L""; t.quyen = Q_ADMIN | Q_QUANLY | Q_LETAN;
        t.moTa = L"Quản lý trang thiết bị (mã, ngày nhập, bảo hành, tình trạng), lịch bảo trì có ghi chi phí và biên bản kiểm tra vệ sinh.";
        t.boLoc = {L"Tất cả thiết bị", L"Cần bảo trì / hỏng", L"Lịch sử bảo trì", L"Biên bản vệ sinh"};
        t.cot = [](int f) -> std::vector<Cot> {
            if (f == 2) return {{L"Mã TB", 80}, {L"Thiết bị", 220}, {L"Ngày", 95}, {L"Nội dung", 320}, {L"Chi phí", 110, true}, {L"Người thực hiện", 160}};
            if (f == 3) return {{L"Khu vực", 160}, {L"Ngày", 95}, {L"Người kiểm tra", 160}, {L"Kết quả", 110}, {L"Ghi chú", 380}};
            return {{L"Mã TB", 75}, {L"Thiết bị", 230}, {L"Khu vực", 130}, {L"Ngày nhập", 95}, {L"Bảo hành đến", 105}, {L"Bảo trì kế tiếp", 110}, {L"Giá trị", 110, true}, {L"Nhà cung cấp", 140}, {L"Tình trạng", 110}};
        };
        t.dong = [](int f) {
            std::vector<Dong> out;
            if (f == 2) {
                for (auto it = C().baoTris().rbegin(); it != C().baoTris().rend(); ++it) {
                    std::wstring ten; for (const auto& tb : C().thietBis()) if (tb.ma() == it->maThietBi()) ten = W(tb.ten());
                    out.push_back({W(it->maThietBi()), ten, ngay(it->ngay()), W(it->noiDung()), tien(it->chiPhi()), W(it->nguoiThucHien())});
                }
                return out;
            }
            if (f == 3) {
                for (auto it = C().kiemTraVeSinhs().rbegin(); it != C().kiemTraVeSinhs().rend(); ++it)
                    out.push_back({W(it->khuVuc()), ngay(it->ngayKiemTra()), W(it->nguoiKiemTra()), it->datYeuCau() ? L"✓ Đạt" : L"✗ Chưa đạt", W(it->ghiChu())});
                return out;
            }
            for (const auto& tb : C().thietBis()) {
                if (f == 1 && !tb.canBaoTri()) continue;
                out.push_back({W(tb.ma()), W(tb.ten()), W(tb.khuVuc()), ngay(tb.ngayNhap()), tb.hanBaoHanh().ok() ? ngay(tb.hanBaoHanh()) + (tb.conBaoHanh() ? L"" : L" (hết)") : L"—", ngay(tb.ngayBaoTri()), tien(tb.giaTri()), W(tb.nhaCungCap()), W(tb.trangThai())});
            }
            return out;
        };
        t.mauChu = [](const Dong& r, int f) -> COLORREF {
            if (f == 3) return r[3].find(L"Chưa") != std::wstring::npos ? mau::doDam : mau::xanhLa;
            if (f >= 2) return CLR_DEFAULT;
            if (r[8] == L"Hỏng") return mau::doDam;
            if (r[8] == L"Đang bảo trì") return mau::cam;
            if (r[8] == L"Thanh lý") return mau::xam;
            if (r[5].size() == 10 && r[5].substr(6, 4) + r[5].substr(3, 2) + r[5].substr(0, 2) <= W(crm::formatDate(crm::today())).erase(7, 1).erase(4, 1)) return mau::cam;
            return CLR_DEFAULT;
        };
        t.moDong = [](int f) { if (f <= 1) formThietBi(&C().thietBi(chon())); };
        t.thaoTac = {
            {L"＋ Thêm thiết bị", [] { formThietBi(nullptr); }, {0, 1}, false, true},
            {L"Sửa", [] { formThietBi(&C().thietBi(chon())); }, {0, 1}},
            {L"Cập nhật tình trạng", [] {
                auto& tb = C().thietBi(chon());
                std::vector<Truong> f{{L"Thiết bị", Kieu::ChiDoc, W(tb.ma()) + L" — " + W(tb.ten())}, {L"Tình trạng", Kieu::Chon, W(tb.trangThai()), kTinhTrang}};
                if (moForm(app().chinh, L"Tình trạng thiết bị", f, [&](const KetQua& k) { tb.capNhatTrangThai(k.chu(1)); })) daThayDoi();
            }, {0, 1}},
            {L"Ghi bảo trì", [] {
                auto& tb = C().thietBi(chon());
                std::vector<Truong> f{{L"Thiết bị", Kieu::ChiDoc, W(tb.ma()) + L" — " + W(tb.ten()) + (tb.conBaoHanh() ? L" (còn bảo hành)" : L"")}, {L"Nội dung", Kieu::NhieuDong, L"", {}, true},
                                      {L"Chi phí (đ)", Kieu::Tien, tb.conBaoHanh() ? L"0" : L""}, {L"Người thực hiện", Kieu::Chu, L""}, {L"Bảo trì kế tiếp", Kieu::Ngay, W(crm::formatDate(crm::addMonths(crm::today(), 3))), {}, true}};
                if (moForm(app().chinh, L"Ghi nhận bảo trì", f, [&](const KetQua& k) { C().baoTriThietBi(tb.ma(), k.chu(1), k.so(2), k.chu(3), k.ngay(4)); }, L"Lưu phiếu")) daThayDoi();
            }, {0, 1}, false, true},
            {L"＋ Biên bản vệ sinh", [] {
                auto khu = kKhuVuc; khu.push_back(L"Nhà vệ sinh"); khu.push_back(L"Quầy lễ tân");
                std::vector<Truong> f{{L"Khu vực", Kieu::ChonSua, khu[0], khu, true}, {L"Người kiểm tra", Kieu::Chu, W(app().tenDangNhap), {}, true}, {L"Ngày", Kieu::Ngay, W(crm::formatDate(crm::today())), {}, true},
                                      {L"Đạt yêu cầu", Kieu::Check, L"1"}, {L"Ghi chú", Kieu::NhieuDong, L""}};
                if (moForm(app().chinh, L"Biên bản kiểm tra vệ sinh", f, [&](const KetQua& k) { C().ghiNhanVeSinh(crm::KiemTraVeSinh(k.chu(0), k.chu(1), k.ngay(2), k.check(3), k.chu(4))); })) daThayDoi();
            }, {3}, false, true},
        };
        ds.push_back(std::move(t));
    }
    {
        Trang t;
        t.ten = L"Tủ đồ"; t.bieuTuong = L""; t.quyen = Q_ADMIN | Q_QUANLY | Q_LETAN;
        t.moTa = L"Tủ đồ thông minh: tự gán khi check-in, tự trả khi check-out, cho thuê theo tháng và báo hỏng.";
        t.boLoc = {L"Tất cả", L"Trống", L"Đang dùng", L"Thuê tháng", L"Hỏng"};
        t.cot = [](int) { return std::vector<Cot>{{L"Mã tủ", 80}, {L"Khu vực", 100}, {L"Trạng thái", 110}, {L"Hội viên", 220}, {L"Từ lúc", 140}, {L"Hết hạn thuê", 110}}; };
        t.dong = [](int f) {
            static const char* tt[] = {"", "Trống", "Đang dùng", "Thuê tháng", "Hỏng"};
            std::vector<Dong> out;
            for (const auto& tu : C().tuDos()) {
                if (f > 0 && tu.trangThai() != tt[f]) continue;
                out.push_back({W(tu.ma()), W(tu.khuVuc()), W(tu.trangThai()), tu.maHoiVien().empty() ? L"" : W(tu.maHoiVien()) + L" " + tenHoiVien(tu.maHoiVien()), luc(tu.batDau()), tu.hetHanThue().ok() ? ngay(tu.hetHanThue()) : L""});
            }
            return out;
        };
        t.mauChu = [](const Dong& r, int) -> COLORREF { if (r[2] == L"Hỏng") return mau::doDam; if (r[2] == L"Trống") return mau::xanhLa; if (r[2] == L"Thuê tháng") return mau::tim; return CLR_DEFAULT; };
        t.chieuCaoVe = [](int) { return 190; };
        t.ve = [](HDC dc, RECT c, int) {
            veKhung(dc, c, mau::the, mau::vien, S(14));
            veChu(dc, {S(16), S(8), c.right - S(16), S(30)}, L"Sơ đồ tủ đồ", font(13, true), mau::chu);
            int lx = S(150);
            for (const char* tt : {"Trống", "Đang dùng", "Thuê tháng", "Hỏng"}) {
                RECT o{lx, S(13), lx + S(12), S(25)}; veKhung(dc, o, mauTu(tt), mau::vien, S(4));
                const std::wstring ten = W(tt); veChu(dc, {lx + S(16), S(8), lx + S(110), S(30)}, ten, font(11), mau::phu);
                lx += S(100);
            }
            const int box = S(54), gap = S(8);
            int x = S(16), y = S(38);
            for (const auto& tu : C().tuDos()) {
                if (x + box > c.right - S(16)) { x = S(16); y += S(40) + gap; }
                if (y + S(40) > c.bottom - S(8)) { veChu(dc, {x, y, c.right, y + S(20)}, L"…", font(13, true), mau::phu); break; }
                RECT r{x, y, x + box, y + S(40)};
                const COLORREF bg = mauTu(tu.trangThai());
                veKhung(dc, r, bg, tu.trangThai() == "Hỏng" ? mau::doDam : mau::vien, S(8));
                veChu(dc, {r.left, r.top + S(2), r.right, r.top + S(22)}, W(tu.ma()), font(11, true), mau::chu, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
                veChu(dc, {r.left + S(2), r.top + S(20), r.right - S(2), r.bottom - S(2)}, tu.maHoiVien().empty() ? W(tu.khuVuc()) : W(tu.maHoiVien()), font(9), mau::phu, DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS);
                x += box + gap;
            }
        };
        t.thaoTac = {
            {L"＋ Thêm tủ hàng loạt", [] {
                std::vector<Truong> f{{L"Tiền tố mã", Kieu::Chu, L"T", {}, true, L"VD: N (nam), F (nữ)"}, {L"Số lượng", Kieu::So, L"10", {}, true}, {L"Khu vực", Kieu::Chon, L"Chung", {L"Nam", L"Nữ", L"Chung", L"VIP"}}};
                int n = 0;
                if (moForm(app().chinh, L"Thêm tủ đồ", f, [&](const KetQua& k) { n = C().themNhieuTuDo(k.chu(0), k.soNguyen(1), k.chu(2)); }, L"Thêm")) { daThayDoi(); thongBao(L"Đã thêm " + std::to_wstring(n) + L" tủ."); }
            }, {}, true, true},
            {L"Gán tủ", [] {
                const auto ma = chon();
                const auto hv = dsHoiVien(true);
                std::vector<Truong> f{{L"Tủ", Kieu::ChiDoc, W(ma)}, {L"Hội viên", Kieu::Chon, L"", hv, true}};
                if (moForm(app().chinh, L"Gán tủ đồ", f, [&](const KetQua& k) { C().ganTu(ma, k.ma(1)); })) daThayDoi();
            }},
            {L"Trả tủ", [] { C().traTu(chon()); daThayDoi(); }},
            {L"Cho thuê tháng", [] {
                const auto ma = chon();
                std::vector<Truong> f{{L"Tủ", Kieu::ChiDoc, W(ma) + L" · giá " + tien(std::stod(C().caiDat("gia_thue_tu", "100000"))) + L" đ/tháng"}, {L"Hội viên", Kieu::Chon, L"", dsHoiVien(true), true},
                                      {L"Số tháng", Kieu::So, L"1", {}, true}, {L"Hình thức thanh toán", Kieu::Chon, L"Tiền mặt", dsThanhToan()}};
                std::string hd;
                if (moForm(app().chinh, L"Cho thuê tủ theo tháng", f, [&](const KetQua& k) { hd = C().thueTuThang(ma, k.ma(1), k.soNguyen(2), k.chu(3)); }, L"Cho thuê")) {
                    daThayDoi(); thongBao(L"Đã cho thuê tủ " + W(ma) + L", hóa đơn " + W(hd) + L".");
                }
            }},
            {L"Báo hỏng", [] { const auto ma = chon(); if (hoi(L"Báo hỏng tủ " + W(ma) + L"?")) { C().tuDo(ma).baoHong(); daThayDoi(); } }},
            {L"Sửa xong", [] { C().tuDo(chon()).suaXong(); daThayDoi(); }, {0, 4}},
            {L"Xóa tủ", [] { const auto ma = chon(); if (hoi(L"Xóa tủ " + W(ma) + L"?")) { C().xoaTuDo(ma); daThayDoi(); } }, {}, true},
        };
        ds.push_back(std::move(t));
    }
}
}
