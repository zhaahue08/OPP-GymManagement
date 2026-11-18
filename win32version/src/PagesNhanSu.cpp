// Trang Nhân sự và Chấm công & lương.
#include "App.h"
#include <set>
namespace gui {
namespace {

const std::vector<std::wstring> kChucVu{L"Quản lý", L"Lễ tân", L"Sale", L"Chăm sóc khách hàng", L"Kế toán", L"Bảo vệ", L"Tạp vụ", L"Kỹ thuật"};
void formNhanVien(crm::NhanVien* nv, bool laPT) {
    auto* pt = dynamic_cast<crm::PT*>(nv);
    if (nv) laPT = pt != nullptr;

    std::vector<Truong> f{
        {L"Mã nhân sự", nv ? Kieu::ChiDoc : Kieu::Chu, nv ? W(nv->ma()) : W(C().sinhMa(laPT ? "PT" : "NV")), {}, true},
        {L"Họ và tên", Kieu::Chu, nv ? W(nv->ten()) : L"", {}, true},
        {laPT ? L"Chuyên môn" : L"Chức vụ", Kieu::ChonSua, nv ? W(pt ? pt->chuyenMon() : nv->chucVu()) : (laPT ? L"Gym tổng hợp" : L"Lễ tân"),
         laPT ? std::vector<std::wstring>{L"Gym tổng hợp", L"Strength & conditioning", L"Giảm cân", L"Yoga / mobility", L"Boxing", L"Phục hồi chức năng"} : kChucVu, true},
        {L"Số điện thoại", Kieu::Chu, nv ? W(nv->sdt()) : L""},
        {L"Lương cơ bản / tháng", Kieu::Tien, nv ? tien(nv->luongCoBan()) : (laPT ? L"5.000.000" : L"6.000.000")},
        {L"Hoa hồng (% doanh số)", Kieu::So, nv ? thapPhan(nv->tyLeHoaHong() * 100) : (laPT ? L"10" : L"5")},
    };
    if (laPT) {
        f.push_back({L"Thù lao mỗi buổi dạy (đ)", Kieu::Tien, pt ? tien(pt->donGiaBuoi()) : L"150.000"});
        f.push_back({L"Giá buổi lẻ cho hội viên (đ)", Kieu::Tien, pt ? tien(pt->giaBuoiChoHoiVien()) : L"300.000"});
        f.push_back({L"Ca làm từ (giờ)", Kieu::So, pt ? std::to_wstring(pt->gioBatDau()) : L"6"});
        f.push_back({L"Ca làm đến (giờ)", Kieu::So, pt ? std::to_wstring(pt->gioKetThuc()) : L"21"});
    }
    if (!moForm(app().chinh, nv ? L"Sửa hồ sơ nhân sự" : (laPT ? L"Thêm huấn luyện viên" : L"Thêm nhân viên"), f, [&](const KetQua& k) {
        const double tyLe = k.so(5) / 100.0;
        if (laPT) {
            if (pt) pt->capNhatPT(k.chu(1), k.chu(2), k.chu(3), k.so(4), k.so(6), tyLe, k.soNguyen(8), k.soNguyen(9), k.so(7));
            else C().themPT(std::make_unique<crm::PT>(k.chu(0), k.chu(1), k.chu(2), k.chu(3), k.so(4), k.so(6), tyLe, k.soNguyen(8), k.soNguyen(9), k.so(7)));
        } else {
            if (nv) nv->capNhat(k.chu(1), k.chu(2), k.chu(3), k.so(4), tyLe);
            else C().themNhanVien(std::make_unique<crm::NhanVien>(k.chu(0), k.chu(1), k.chu(2), k.chu(3), k.so(4), tyLe));
        }
    })) return;
    daThayDoi();
}

crm::NhanVien& nvChon() { auto* nv = C().timNhanSu(chon()); if (!nv) throw std::runtime_error("Không tìm thấy nhân sự."); return *nv; }
std::string maNVMacDinh() {
    if (!app().maNhanVien.empty()) return app().maNhanVien;
    const auto key = chonHoacRong();
    return C().timNhanSu(key) ? key : std::string{};
}

void chamCong(bool vao) {
    const auto ds = dsNhanSu();
    if (ds.empty()) throw std::runtime_error("Chưa có nhân sự.");
    std::vector<Truong> f{{L"Nhân viên", Kieu::Chon, timTrongDs(ds, maNVMacDinh()), ds, true}};
    std::wstring msg;
    if (moForm(app().chinh, vao ? L"Chấm công vào ca" : L"Chấm công ra ca", f, [&](const KetQua& k) {
        if (!laQuanLy() && !app().maNhanVien.empty() && k.ma(0) != app().maNhanVien) throw std::runtime_error("Bạn chỉ được chấm công cho chính mình.");
        if (vao) C().chamCongVao(k.ma(0)); else C().chamCongRa(k.ma(0));
        msg = (vao ? L"Đã chấm công VÀO lúc " : L"Đã chấm công RA lúc ") + gio(crm::now()) + L" cho " + tenNhanSu(k.ma(0)) + L".";
    }, vao ? L"Vào ca" : L"Ra ca")) { daThayDoi(); thongBao(msg); }
}

std::pair<int, unsigned> kyLuong(int f) {
    const auto d = crm::today();
    int y = int(d.year()); unsigned m = unsigned(d.month());
    if (f == 3) { if (m == 1) { m = 12; --y; } else --m; }
    return {y, m};
}
void phieuLuong(int f) {
            if (f < 2) return;
            const auto ma = chon(); auto [y, m] = kyLuong(f);
            for (const auto& r : C().bangLuong(y, m)) if (r.maNhanVien == ma) {
                auto* nv = C().timNhanSu(ma);
                std::wstring text = W(C().caiDat("ten_phong_tap", "Fitness CRM")) + L"\nPHIẾU LƯƠNG THÁNG " + std::to_wstring(m) + L"/" + std::to_wstring(y) + L"\n==============================\n";
                text += L"Nhân viên : " + W(r.maNhanVien) + L" — " + W(r.ten) + L"\nChức vụ   : " + W(r.chucVu) + L"\n------------------------------\n";
                text += L"Lương cơ bản      : " + tien(nv->luongCoBan()) + L" đ\nNgày công / chuẩn : " + std::to_wstring(r.soCong) + L" / " + W(C().caiDat("cong_chuan", "26")) + L"\nSố giờ làm        : " + thapPhan(r.soGio) + L"\n";
                if (auto* pt = dynamic_cast<crm::PT*>(nv)) text += L"Ca PT đã dạy      : " + std::to_wstring(r.soCaPT) + L" × " + tien(pt->donGiaBuoi()) + L" đ\n";
                text += L"Doanh số          : " + tien(r.doanhSo) + L" đ\nHoa hồng (" + thapPhan(nv->tyLeHoaHong() * 100, 0) + L"%)     : " + tien(r.hoaHong) + L" đ\n------------------------------\nTHỰC LĨNH         : " + tien(r.luong) + L" đ\n";
                xemVanBan(app().chinh, L"Phiếu lương " + W(ma), text, L"PhieuLuong_" + W(ma) + L".txt");
                return;
            }
}
}
void themTrangNhanSu(std::vector<Trang>& ds) {
    {
        Trang t;
        t.ten = L"Nhân sự"; t.bieuTuong = L""; t.quyen = Q_ADMIN | Q_QUANLY;
        t.moTa = L"Hồ sơ nhân viên (sale, lễ tân, CSKH...) và huấn luyện viên: lương, hoa hồng, ca làm, số ca dạy, điểm đánh giá.";
        t.boLoc = {L"Đang làm việc", L"Huấn luyện viên", L"Nhân viên khác", L"Đã nghỉ việc"};
        t.cot = [](int) { return std::vector<Cot>{{L"Mã", 70}, {L"Họ tên", 170}, {L"Chức vụ / chuyên môn", 190}, {L"SĐT", 105}, {L"Vào làm", 95}, {L"Lương CB", 105, true}, {L"% HH", 60, true},
                                                  {L"Thù lao/buổi", 100, true}, {L"Ca làm", 80}, {L"Ca đã dạy", 80, true}, {L"Học viên", 75, true}, {L"Đánh giá", 90}, {L"Trạng thái", 90}}; };
        t.dong = [](int f) {
            std::vector<Dong> out;
            for (auto* nv : C().danhSachNhanSu()) {
                auto* pt = dynamic_cast<crm::PT*>(nv);
                if ((f == 0 && !nv->dangLam()) || (f == 1 && !pt) || (f == 2 && pt) || (f == 3 && nv->dangLam())) continue;
                int hocVien = 0; if (pt) for (const auto& [id, hv] : C().hoiViens()) { (void)id; if (hv->maPT() == pt->ma()) ++hocVien; }
                const auto [diem, soDG] = C().diemDanhGia(nv->ma());
                out.push_back({W(nv->ma()), W(nv->ten()), pt ? L"PT · " + W(pt->chuyenMon()) : W(nv->chucVu()), W(nv->sdt()), ngay(nv->ngayVaoLam()), tien(nv->luongCoBan()), thapPhan(nv->tyLeHoaHong() * 100, 0) + L"%",
                               pt ? tien(pt->donGiaBuoi()) : L"", pt ? std::to_wstring(pt->gioBatDau()) + L"h-" + std::to_wstring(pt->gioKetThuc()) + L"h" : L"", pt ? std::to_wstring(pt->soCaDaDay()) : L"",
                               pt ? std::to_wstring(hocVien) : L"", soDG ? thapPhan(diem) + L"★ (" + std::to_wstring(soDG) + L")" : L"", nv->dangLam() ? L"Đang làm" : L"Đã nghỉ"});
            }
            return out;
        };
        t.mauChu = [](const Dong& r, int) -> COLORREF { return r[12] == L"Đã nghỉ" ? mau::xam : CLR_DEFAULT; };
        t.moDong = [](int) { formNhanVien(&nvChon(), false); };
        t.thaoTac = {
            {L"＋ Thêm nhân viên", [] { formNhanVien(nullptr, false); }, {}, true, true},
            {L"＋ Thêm PT", [] { formNhanVien(nullptr, true); }, {}, true, true},
            {L"Sửa", [] { formNhanVien(&nvChon(), false); }, {}, true},
            {L"Cho nghỉ / đi làm lại", [] {
                auto& nv = nvChon();
                if (nv.dangLam()) { if (!hoi(L"Cho " + W(nv.ten()) + L" nghỉ việc?")) return; nv.nghiViec(); } else nv.diLamLai();
                daThayDoi();
            }, {}, true},
            {L"Học viên phụ trách", [] {
                auto& nv = nvChon();
                std::wstring text = L"Học viên do " + W(nv.ten()) + L" phụ trách:\n\n";
                int n = 0;
                for (const auto& [id, hv] : C().hoiViens()) if (hv->maPT() == nv.ma()) { text += W(id) + L"  " + W(hv->ten()) + L"  · " + W(hv->trangThai()) + L" · còn " + std::to_wstring(hv->soBuoiPT()) + L" buổi PT\n"; ++n; }
                if (!n) text += L"(chưa có)\n";
                xemVanBan(app().chinh, L"Học viên của PT", text);
            }},
        };
        ds.push_back(std::move(t));
    }
    {
        Trang t;
        t.ten = L"Chấm công & lương"; t.bieuTuong = L""; t.quyen = Q_TATCA;
        t.moTa = L"Chấm công vào/ra theo giờ thực, tổng hợp ngày công và tính lương = lương cứng theo công + hoa hồng doanh số + thù lao buổi PT.";
        t.boLoc = {L"Chấm công hôm nay", L"Chấm công tháng này", L"Bảng lương tháng này", L"Bảng lương tháng trước"};
        t.phuDe = [](int f) {
            if (f >= 2) { auto [y, m] = kyLuong(f); return L"Bảng lương tháng " + std::to_wstring(m) + L"/" + std::to_wstring(y) + L" — công chuẩn " + W(C().caiDat("cong_chuan", "26")) + L" ngày. Nhấp đúp để xem phiếu lương."; }
            return std::wstring(L"Chấm công vào/ra theo giờ thực; nhân viên chỉ xem và chấm công cho chính mình.");
        };
        t.cot = [](int f) -> std::vector<Cot> {
            if (f >= 2) return {{L"Mã NV", 75}, {L"Họ tên", 180}, {L"Chức vụ", 140}, {L"Ngày công", 85, true}, {L"Số giờ", 75, true}, {L"Ca PT", 65, true}, {L"Doanh số", 120, true}, {L"Hoa hồng", 110, true}, {L"Thực lĩnh", 130, true}};
            return {{L"Mã NV", 75}, {L"Họ tên", 180}, {L"Ngày", 95}, {L"Giờ vào", 80}, {L"Giờ ra", 80}, {L"Số giờ", 80, true}, {L"Trạng thái", 110}};
        };
        t.dong = [](int f) {
            std::vector<Dong> out;
            const bool chiMinh = !laQuanLy();
            const auto& toi = app().maNhanVien;
            if (f >= 2) {
                auto [y, m] = kyLuong(f);
                for (const auto& r : C().bangLuong(y, m)) {
                    if (chiMinh && r.maNhanVien != toi) continue;
                    out.push_back({W(r.maNhanVien), W(r.ten), W(r.chucVu), std::to_wstring(r.soCong), thapPhan(r.soGio), std::to_wstring(r.soCaPT), tien(r.doanhSo), tien(r.hoaHong), tien(r.luong)});
                }
                return out;
            }
            const auto d = crm::today();
            for (auto it = C().chamCongs().rbegin(); it != C().chamCongs().rend(); ++it) {
                if (chiMinh && it->maNhanVien() != toi) continue;
                if (f == 0 && it->ngay() != d) continue;
                if (f == 1 && (it->ngay().year() != d.year() || it->ngay().month() != d.month())) continue;
                out.push_back({W(it->maNhanVien()), tenNhanSu(it->maNhanVien()), ngay(it->ngay()), gio(it->vao()), it->daRa() ? gio(it->ra()) : L"—", thapPhan(it->soGio()), it->daRa() ? L"Hoàn tất" : L"Đang trong ca"});
            }
            return out;
        };
        t.mauChu = [](const Dong& r, int f) -> COLORREF { return f < 2 && r[6] == L"Đang trong ca" ? mau::xanhLa : CLR_DEFAULT; };
        t.moDong = phieuLuong;
        t.thaoTac = {
            {L"Chấm công vào", [] { chamCong(true); }, {}, false, true},
            {L"Chấm công ra", [] { chamCong(false); }},
            {L"Xem phiếu lương", [] { phieuLuong(boLocHienTai()); }, {2, 3}},
        };
        ds.push_back(std::move(t));
    }
}
}
