// Trang Bán hàng (POS), Kho hàng, Thu chi & công nợ.
#include "App.h"
#include <algorithm>

namespace gui {
namespace {
const std::vector<std::wstring> kDanhMucSP{L"Nước uống", L"Thực phẩm bổ sung", L"Đồ ăn kiêng", L"Phụ kiện", L"Trang phục", L"Khác"};
const std::vector<std::wstring> kDanhMucChi{L"Điện nước", L"Mặt bằng", L"Lương", L"Nhập hàng", L"Bảo trì", L"Marketing", L"Vệ sinh", L"Khác"};
double tongGio() { double s = 0; for (const auto& [ma, sl] : app().gioHang) s += C().sanPham(ma).giaBan() * sl; return s; }
long long soMonGio() { long long s = 0; for (const auto& dong : app().gioHang) s += dong.second; return s; }
int soLuongTrongGio(const std::string& ma) {
    int tong = 0;
    for (const auto& [maDong, soLuong] : app().gioHang)
        if (maDong == ma) tong += soLuong;
    return tong;
}
void themVaoGio(const std::string& ma, int sl) {
    if (sl <= 0) throw std::invalid_argument("Số lượng phải lớn hơn 0.");
    const auto& sp = C().sanPham(ma);
    if (!sp.dangBan()) throw std::runtime_error(sp.ten() + " đã ngừng bán.");
    const int coTheThem = std::max(0, sp.tonKho() - soLuongTrongGio(ma));
    if (sl > coTheThem) throw std::runtime_error("Chỉ có thể thêm " + std::to_string(coTheThem) + " " + sp.donVi() + " " + sp.ten() + " theo tồn kho hiện tại.");
    app().gioHang.emplace_back(ma, sl);
}
void formSanPham(const crm::SanPham* sp) {
    std::vector<Truong> f{
        {L"Mã sản phẩm", sp ? Kieu::ChiDoc : Kieu::Chu, sp ? W(sp->ma()) : W(C().sinhMa("SP")), {}, true},
        {L"Tên sản phẩm", Kieu::Chu, sp ? W(sp->ten()) : L"", {}, true},
        {L"Danh mục", Kieu::ChonSua, sp ? W(sp->danhMuc()) : kDanhMucSP[0], kDanhMucSP, true},
        {L"Đơn vị tính", Kieu::ChonSua, sp ? W(sp->donVi()) : L"chai", {L"chai", L"lon", L"hộp", L"gói", L"thanh", L"cái", L"hũ"}},
        {L"Giá bán (đ)", Kieu::Tien, sp ? tien(sp->giaBan()) : L"", {}, true},
        {L"Giá nhập (đ)", Kieu::Tien, sp ? tien(sp->giaNhap()) : L"", {}, !sp},
        {L"Tồn đầu kỳ", sp ? Kieu::ChiDoc : Kieu::So, sp ? std::to_wstring(sp->tonKho()) : L"0"},
        {L"Mức tồn tối thiểu", Kieu::So, sp ? std::to_wstring(sp->mucToiThieu()) : L"5"},
    };
    if (!moForm(app().chinh, sp ? L"Sửa sản phẩm" : L"Thêm sản phẩm", f, [&](const KetQua& k) {
        if (sp) C().sanPham(sp->ma()).capNhat(k.chu(1), k.chu(2), k.chu(3), k.so(4), k.so(5), k.soNguyen(7));
        else C().themSanPham(crm::SanPham(k.chu(0), k.chu(1), k.chu(2), k.chu(3), k.so(4), k.so(5), k.soNguyen(6), k.soNguyen(7)));
    })) return;
    daThayDoi();
}
std::wstring noiDungHD(const crm::HoaDon& hd) {
    if (hd.dongHang().empty()) return L"";
    std::wstring s = W(hd.dongHang().front().ten);
    if (hd.dongHang().size() > 1) s += L" (+" + std::to_wstring(hd.dongHang().size() - 1) + L" món)";
    return s;
}
void xemHD(const std::string& ma) { const auto& hd = C().timHoaDon(ma); xemVanBan(app().chinh, L"Hóa đơn " + W(ma), vanBanHoaDon(hd), L"HoaDon_" + W(ma) + L".txt"); }
void formGiaoDich(bool chi) {
    std::vector<Truong> f{
        {L"Mã phiếu", Kieu::Chu, W(C().sinhMa(chi ? "CP" : "TK")), {}, true},
        {L"Nội dung", Kieu::Chu, L"", {}, true, chi ? L"VD: Tiền điện tháng 10" : L"VD: Cho thuê mặt bằng sự kiện"},
        {L"Danh mục", Kieu::ChonSua, chi ? kDanhMucChi[0] : L"Thu khác", chi ? kDanhMucChi : std::vector<std::wstring>{L"Thu khác", L"Tài trợ", L"Cho thuê", L"Sự kiện"}},
        {L"Ngày", Kieu::Ngay, W(crm::formatDate(crm::today())), {}, true},
        {L"Số tiền (đ)", Kieu::Tien, L"", {}, true},
    };
    if (moForm(app().chinh, chi ? L"Ghi nhận chi phí" : L"Ghi nhận thu khác", f, [&](const KetQua& k) {
        crm::GiaoDich g(k.chu(0), k.chu(1), k.ngay(3), chi ? crm::LoaiGiaoDich::Chi : crm::LoaiGiaoDich::Thu, k.so(4), k.chu(2));
        if (chi) C().ghiNhanChi(g); else C().ghiNhanThu(g);
    })) daThayDoi();
}
}

void themTrangTaiChinh(std::vector<Trang>& ds) {
    {
        Trang t;
        t.ten = L"Bán hàng (POS)"; t.bieuTuong = L""; t.quyen = Q_ADMIN | Q_QUANLY | Q_LETAN | Q_SALE;
        t.boLoc = {L"Sản phẩm đang bán", L"Giỏ hàng"};
        t.phuDe = [](int) { return L"Bán nước uống, thực phẩm bổ sung, đồ ăn kiêng... Nhấp đúp sản phẩm để thêm 1 vào giỏ. Giỏ hiện có " + std::to_wstring(soMonGio()) + L" món · " + tien(tongGio()) + L" đ."; };
        t.cot = [](int f) -> std::vector<Cot> {
            if (f == 1) return {{L"Dòng", 60, true}, {L"Mã SP", 80}, {L"Sản phẩm", 220}, {L"Số lượng", 90, true}, {L"Đơn giá", 110, true}, {L"Thành tiền", 130, true}};
            return {{L"Mã SP", 80}, {L"Sản phẩm", 260}, {L"Danh mục", 150}, {L"Đơn vị", 80}, {L"Giá bán", 110, true}, {L"Tồn kho", 90, true}, {L"Trong giỏ", 90, true}};
        };
        t.dong = [](int f) {
            std::vector<Dong> out;
            if (f == 1) {
                const auto& gio = app().gioHang;
                for (std::size_t i = 0; i < gio.size(); ++i) {
                    const auto& [ma, sl] = gio[i];
                    const auto& sp = C().sanPham(ma);
                    out.push_back({std::to_wstring(i + 1), W(ma), W(sp.ten()), std::to_wstring(sl), tien(sp.giaBan()), tien(sp.giaBan() * sl)});
                }
                return out;
            }
            for (const auto& [ma, sp] : C().sanPhams()) {
                if (!sp.dangBan()) continue;
                const int gio = soLuongTrongGio(ma);
                out.push_back({W(ma), W(sp.ten()), W(sp.danhMuc()), W(sp.donVi()), tien(sp.giaBan()), std::to_wstring(sp.tonKho()), gio ? std::to_wstring(gio) : L""});
            }
            return out;
        };
        t.mauChu = [](const Dong& r, int f) -> COLORREF { if (f == 0 && r[5] == L"0") return mau::xam; if (f == 0 && !r[6].empty()) return mau::chinh; return CLR_DEFAULT; };
        t.chieuCaoVe = [](int) { return 96; };
        t.ve = [](HDC dc, RECT c, int) {
            const int gap = S(14), w3 = (c.right - 2 * gap) / 3, h = S(88);
            veKPI(dc, {0, 0, w3, h}, L"GIỎ HÀNG", std::to_wstring(soMonGio()) + L" món", std::to_wstring(app().gioHang.size()) + L" dòng hàng", mau::chinh, mau::chinhNhat);
            veKPI(dc, {w3 + gap, 0, 2 * w3 + gap, h}, L"TẠM TÍNH", tien(tongGio()) + L" đ", L"Chưa gồm giảm giá", mau::xanh, mau::xanhNhat);
            double hn = 0; int n = 0; const auto d = crm::formatDate(crm::today());
            for (const auto& hd : C().hoaDons()) if (hd.loai() == "Bán lẻ" && hd.ngay() == d) { hn += hd.tongTien(); ++n; }
            veKPI(dc, {2 * (w3 + gap), 0, c.right, h}, L"BÁN LẺ HÔM NAY", tien(hn) + L" đ", std::to_wstring(n) + L" hóa đơn", mau::tim, mau::timNhat);
        };
        t.moDong = [](int f) { if (f == 0) { themVaoGio(chon(), 1); lamMoi(); } };
        t.thaoTac = {
            {L"＋ Thêm vào giỏ", [] {
                const auto ma = chon(); const auto& sp = C().sanPham(ma);
                std::vector<Truong> f{{L"Sản phẩm", Kieu::ChiDoc, W(ma) + L" — " + W(sp.ten()) + L" · " + tien(sp.giaBan()) + L" đ"}, {L"Số lượng", Kieu::So, L"1", {}, true}};
                if (moForm(app().chinh, L"Thêm vào giỏ", f, [&](const KetQua& k) { themVaoGio(ma, k.soNguyen(1)); }, L"Thêm")) lamMoi();
            }, {0}},
            {L"Bớt khỏi giỏ", [] {
                const auto dong = std::stoull(chon());
                auto& g = app().gioHang;
                if (dong == 0 || dong > g.size()) throw std::runtime_error("Dòng hàng không còn trong giỏ.");
                auto it = g.begin() + static_cast<std::ptrdiff_t>(dong - 1);
                if (--it->second == 0) g.erase(it);
                lamMoi();
            }, {1}},
            {L"Xóa giỏ", [] { if (!app().gioHang.empty() && hoi(L"Xóa toàn bộ giỏ hàng?")) { app().gioHang.clear(); lamMoi(); } }},
            {L"💳 Thanh toán", [] {
                if (app().gioHang.empty()) throw std::runtime_error("Giỏ hàng trống.");
                std::wstring tomTat;
                for (const auto& [ma, sl] : app().gioHang) { const auto& sp = C().sanPham(ma); tomTat += W(sp.ten()) + L" × " + std::to_wstring(sl) + L" = " + tien(sp.giaBan() * sl) + L" đ\n"; }
                tomTat += L"TẠM TÍNH: " + tien(tongGio()) + L" đ";
                auto khach = dsHoiVien(); khach.insert(khach.begin(), L"KHACH_LE — Khách lẻ");
                std::vector<Truong> f{{L"Giỏ hàng", Kieu::ChiDoc, tomTat}, {L"Khách hàng", Kieu::Chon, khach[0], khach}, {L"Giảm giá (đ)", Kieu::Tien, L"0"},
                                      {L"Hình thức thanh toán", Kieu::Chon, L"Tiền mặt", dsThanhToan()}, {L"Khách trả (đ)", Kieu::Tien, L"", {}, false, L"Để trống = trả đủ (hội viên được ghi nợ)"},
                                      {L"Nhân viên bán", Kieu::Chon, timTrongDs(dsNhanSu(true), app().maNhanVien), dsNhanSu(true)}};
                std::string ma;
                if (moForm(app().chinh, L"Thanh toán", f, [&](const KetQua& k) {
                    const auto kh = k.ma(1);
                    const double daTra = k.rong(4) ? -1 : k.so(4);
                    if (!k.rong(4) && daTra < 0) throw std::invalid_argument("Số tiền khách trả không được âm.");
                    ma = C().banLe(kh == crm::KHACH_LE ? std::string{} : kh, app().gioHang, k.so(2), daTra, k.chu(3), k.chon(5) <= 0 ? std::string{} : k.ma(5));
                }, L"Thanh toán")) {
                    app().gioHang.clear(); daThayDoi();
                    xemHD(ma);
                }
            }, {}, false, true},
        };
        ds.push_back(std::move(t));
    }
    {
        Trang t;
        t.ten = L"Kho hàng"; t.bieuTuong = L""; t.quyen = Q_ADMIN | Q_QUANLY | Q_LETAN;
        t.moTa = L"Danh mục hàng hóa, nhập hàng (tự ghi chi phí, giá vốn bình quân), kiểm kê chênh lệch và cảnh báo sắp hết hàng.";
        t.boLoc = {L"Tồn kho", L"Sắp hết hàng", L"Lịch sử nhập / xuất / kiểm kê", L"Ngừng bán"};
        t.cot = [](int f) -> std::vector<Cot> {
            if (f == 2) return {{L"Mã phiếu", 85}, {L"Ngày", 95}, {L"Loại", 95}, {L"Mã SP", 75}, {L"Sản phẩm", 220}, {L"Số lượng", 85, true}, {L"Đơn giá", 105, true}, {L"Ghi chú", 260}};
            return {{L"Mã SP", 75}, {L"Sản phẩm", 230}, {L"Danh mục", 140}, {L"Đơn vị", 70}, {L"Giá bán", 100, true}, {L"Giá vốn", 100, true}, {L"Tồn", 70, true}, {L"Tối thiểu", 80, true}, {L"Giá trị tồn", 120, true}, {L"Trạng thái", 110}};
        };
        t.dong = [](int f) {
            std::vector<Dong> out;
            if (f == 2) {
                for (auto it = C().phieuKhos().rbegin(); it != C().phieuKhos().rend(); ++it) {
                    auto sp = C().sanPhams().find(it->maSanPham());
                    out.push_back({W(it->ma()), ngay(it->ngay()), W(it->loai()), W(it->maSanPham()), sp == C().sanPhams().end() ? L"" : W(sp->second.ten()), std::to_wstring(it->soLuong()), tien(it->donGia()), W(it->ghiChu())});
                }
                return out;
            }
            for (const auto& [ma, sp] : C().sanPhams()) {
                if ((f == 0 && !sp.dangBan()) || (f == 1 && !sp.sapHet()) || (f == 3 && sp.dangBan())) continue;
                out.push_back({W(ma), W(sp.ten()), W(sp.danhMuc()), W(sp.donVi()), tien(sp.giaBan()), tien(sp.giaNhap()), std::to_wstring(sp.tonKho()), std::to_wstring(sp.mucToiThieu()), tien(sp.giaNhap() * sp.tonKho()),
                               !sp.dangBan() ? L"Ngừng bán" : sp.tonKho() == 0 ? L"Hết hàng" : sp.sapHet() ? L"Sắp hết" : L"Còn hàng"});
            }
            return out;
        };
        t.mauChu = [](const Dong& r, int f) -> COLORREF {
            if (f == 2) return r[2] == L"Nhập" ? mau::xanhLa : r[2] == L"Kiểm kê" ? mau::cam : CLR_DEFAULT;
            if (r[9] == L"Hết hàng") return mau::doDam;
            if (r[9] == L"Sắp hết") return mau::cam;
            if (r[9] == L"Ngừng bán") return mau::xam;
            return CLR_DEFAULT;
        };
        t.phuDe = [](int) { double v = 0; for (const auto& [m, sp] : C().sanPhams()) { (void)m; v += sp.giaNhap() * sp.tonKho(); } return L"Tổng giá trị tồn kho (theo giá vốn): " + tien(v) + L" đ. Nhập hàng tự động ghi một phiếu chi."; };
        t.moDong = [](int f) { if (f != 2) formSanPham(&C().sanPham(chon())); };
        auto nhap = [] {
            const auto key = chonHoacRong();
            const auto ds2 = dsSanPham();
            if (ds2.empty()) throw std::runtime_error("Chưa có sản phẩm.");
            std::vector<Truong> f{{L"Sản phẩm", Kieu::Chon, timTrongDs(ds2, key), ds2, true}, {L"Số lượng nhập", Kieu::So, L"", {}, true}, {L"Đơn giá nhập (đ)", Kieu::Tien, L"", {}, true},
                                  {L"Nhà cung cấp", Kieu::Chu, L""}, {L"Ghi phiếu chi", Kieu::Check, L"1"}};
            if (C().sanPhams().contains(key)) f[2].giaTri = tien(C().sanPham(key).giaNhap());
            if (moForm(app().chinh, L"Nhập hàng", f, [&](const KetQua& k) { C().nhapHang(k.ma(0), k.soNguyen(1), k.so(2), k.chu(3), k.check(4)); }, L"Nhập kho")) daThayDoi();
        };
        t.thaoTac = {
            {L"＋ Thêm sản phẩm", [] { formSanPham(nullptr); }, {}, false, true},
            {L"Sửa", [] { formSanPham(&C().sanPham(chon())); }, {0, 1, 3}},
            {L"Nhập hàng", nhap, {0, 1, 3}},
            {L"Kiểm kê", [] {
                const auto ma = chon(); const auto& sp = C().sanPham(ma);
                std::vector<Truong> f{{L"Sản phẩm", Kieu::ChiDoc, W(ma) + L" — " + W(sp.ten()) + L" (sổ sách: " + std::to_wstring(sp.tonKho()) + L")"}, {L"Tồn thực tế", Kieu::So, std::to_wstring(sp.tonKho()), {}, true}, {L"Ghi chú", Kieu::Chu, L""}};
                int diff = 0;
                if (moForm(app().chinh, L"Kiểm kê kho", f, [&](const KetQua& k) { diff = C().kiemKe(ma, k.soNguyen(1), k.chu(2)); }, L"Cập nhật")) {
                    daThayDoi(); thongBao(diff == 0 ? L"Khớp sổ sách." : L"Chênh lệch: " + std::wstring(diff > 0 ? L"+" : L"") + std::to_wstring(diff) + L" " + W(sp.donVi()));
                }
            }, {0, 1, 3}},
            {L"Ngừng / mở bán", [] { auto& sp = C().sanPham(chon()); sp.datDangBan(!sp.dangBan()); daThayDoi(); }, {0, 1, 3}, true},
            {L"Xóa", [] {
                const auto ma = chon();
                if (!hoi(L"Xóa sản phẩm " + W(ma) + L"?")) return;
                try { C().xoaSanPham(ma); } catch (const std::logic_error&) { daThayDoi(); throw; }
                daThayDoi();
            }, {0, 1, 3}, true},
        };
        ds.push_back(std::move(t));
    }
    {
        Trang t;
        t.ten = L"Thu chi & công nợ"; t.bieuTuong = L""; t.quyen = Q_ADMIN | Q_QUANLY;
        t.moTa = L"Hóa đơn (gói tập, bán lẻ, dịch vụ), đa phương thức thanh toán, công nợ, chi phí, thu khác và dòng tiền.";
        t.boLoc = {L"Hóa đơn", L"Công nợ", L"Chi phí", L"Thu khác"};
        t.cot = [](int f) -> std::vector<Cot> {
            if (f >= 2) return {{L"Mã phiếu", 85}, {L"Ngày", 95}, {L"Danh mục", 130}, {L"Nội dung", 380}, {L"Số tiền", 130, true}};
            return {{L"Mã HĐ", 80}, {L"Ngày", 95}, {L"Khách hàng", 180}, {L"Loại", 85}, {L"Nội dung", 250}, {L"Tổng tiền", 110, true}, {L"Đã trả", 110, true}, {L"Còn nợ", 100, true}, {L"Thanh toán", 110}, {L"Nhân viên", 130}};
        };
        t.dong = [](int f) {
            std::vector<Dong> out;
            if (f >= 2) {
                const auto& list = f == 2 ? C().chiPhis() : C().thuKhacs();
                for (auto it = list.rbegin(); it != list.rend(); ++it) out.push_back({W(it->ma()), ngay(it->ngay()), W(it->danhMuc()), W(it->noiDung()), tien(it->soTien())});
                return out;
            }
            for (auto it = C().hoaDons().rbegin(); it != C().hoaDons().rend(); ++it) {
                if (f == 1 && it->conNo() <= 0) continue;
                out.push_back({W(it->ma()), ngay(crm::parseDate(it->ngay())), tenHoiVien(it->maHoiVien()), W(it->loai()), noiDungHD(*it), tien(it->tongTien()), tien(it->daThanhToan()), it->conNo() > 0 ? tien(it->conNo()) : L"",
                               W(it->phuongThuc()), tenNhanSu(it->maNhanVien())});
            }
            return out;
        };
        t.mauChu = [](const Dong& r, int f) -> COLORREF { if (f < 2 && !r[7].empty()) return mau::doDam; if (f == 3) return mau::xanhLa; return CLR_DEFAULT; };
        t.chieuCaoVe = [](int) { return 96; };
        t.ve = [](HDC dc, RECT c, int) {
            const int gap = S(12), w5 = (c.right - 4 * gap) / 5, h = S(88);
            auto& x = C();
            veKPI(dc, {0, 0, w5, h}, L"TỔNG DOANH THU", tien(x.tongDoanhThu()) + L" đ", L"Hóa đơn + thu khác", mau::chinh, mau::chinhNhat);
            veKPI(dc, {w5 + gap, 0, 2 * w5 + gap, h}, L"ĐÃ THU", tien(x.tongDaThu()) + L" đ", L"Tiền thực nhận", mau::xanhLa, mau::xanhLaNhat);
            veKPI(dc, {2 * (w5 + gap), 0, 3 * w5 + 2 * gap, h}, L"CÔNG NỢ", tien(x.tongCongNo()) + L" đ", L"Phải thu từ hội viên", mau::doDam, mau::doNhat);
            veKPI(dc, {3 * (w5 + gap), 0, 4 * w5 + 3 * gap, h}, L"TỔNG CHI", tien(x.tongChi()) + L" đ", L"Chi phí vận hành", mau::cam, mau::camNhat);
            veKPI(dc, {4 * (w5 + gap), 0, c.right, h}, L"LỢI NHUẬN", tien(x.soDu()) + L" đ", L"Doanh thu − chi phí", x.soDu() >= 0 ? mau::tim : mau::doDam, mau::timNhat);
        };
        t.moDong = [](int f) { if (f < 2) xemHD(chon()); };
        t.thaoTac = {
            {L"Xem / in hóa đơn", [] { xemHD(chon()); }, {0, 1}},
            {L"Thu nợ", [] {
                const auto ma = chon(); const auto& hd = C().timHoaDon(ma);
                if (hd.conNo() <= 0) throw std::runtime_error("Hóa đơn này không còn công nợ.");
                std::vector<Truong> f{{L"Hóa đơn", Kieu::ChiDoc, W(ma) + L" — " + tenHoiVien(hd.maHoiVien()) + L" · còn nợ " + tien(hd.conNo()) + L" đ"}, {L"Số tiền thu", Kieu::Tien, tien(hd.conNo()), {}, true},
                                      {L"Hình thức", Kieu::Chon, L"Tiền mặt", dsThanhToan()}};
                if (moForm(app().chinh, L"Thu nợ", f, [&](const KetQua& k) { C().thuNo(ma, k.so(1), k.chu(2)); }, L"Thu tiền")) daThayDoi();
            }, {0, 1}, false, true},
            {L"＋ Ghi chi phí", [] { formGiaoDich(true); }, {2}, false, true},
            {L"＋ Ghi thu khác", [] { formGiaoDich(false); }, {3}, false, true},
            {L"Xóa phiếu", [] { const auto ma = chon(); if (hoi(L"Xóa phiếu " + W(ma) + L"?")) { C().xoaChiPhi(ma); daThayDoi(); } }, {2, 3}, true},
        };
        ds.push_back(std::move(t));
    }
}
}
