// Trang Hội viên, Check-in / ra vào, Chăm sóc khách hàng và Phản hồi.
#include "App.h"
#include "DichVuCSKH.h"
#include <algorithm>
#include <sstream>

namespace gui {
namespace {
std::shared_ptr<crm::HoiVien> hvChon() { return C().timHoiVien(chon()); }
std::wstring ngayChu(const std::string& s) { if (s.empty()) return L"—"; try { return ngay(crm::parseDate(s)); } catch (...) { return W(s); } }
std::wstring moTaHV(const crm::HoiVien& hv) {
    return W(hv.ma()) + L" — " + W(hv.ten()) + L" · " + W(hv.trangThai()) + (hv.ngayHetHan().ok() ? L" · hạn " + ngay(hv.ngayHetHan()) : L"");
}
const std::vector<std::wstring> kNghe{L"Sinh viên", L"Nhân viên văn phòng", L"Kinh doanh", L"Giáo viên", L"Kỹ sư / IT", L"Y tế", L"Lao động tự do", L"Nội trợ", L"Hưu trí", L"Khác"};
const std::vector<std::wstring> kGioiTinh{L"Nam", L"Nữ", L"Khác"};

std::wstring chiTietHoiVien(const crm::HoiVien& hv) {
    auto& c = C();
    std::wostringstream o;
    o << L"HỒ SƠ HỘI VIÊN " << W(hv.ma()) << L"\n==============================================\n";
    o << L"Họ tên       : " << W(hv.ten()) << L"\nSố điện thoại: " << W(hv.sdt()) << L"\nEmail        : " << W(hv.email());
    o << L"\nNgày sinh    : " << ngay(hv.ngaySinh()) << L" (" << hv.tuoi() << L" tuổi)\nGiới tính    : " << W(hv.gioiTinh()) << L"\nNghề nghiệp  : " << W(hv.ngheNghiep());
    o << L"\nSinh viên    : " << (hv.laSinhVien() ? L"Có" : L"Không") << L"\nHạng         : " << W(hv.hang()) << L"\nNgày tham gia: " << ngay(hv.ngayThamGia());
    o << L"\nThẻ          : " << (hv.maThe().empty() ? L"(chưa gán)" : W(hv.loaiThe()) + L" " + W(hv.maThe()));
    if (!hv.ghiChu().empty()) o << L"\nGhi chú      : " << W(hv.ghiChu());
    o << L"\n\nGÓI TẬP\n----------------------------------------------\n";
    o << L"Gói hiện tại : " << (hv.goiHienTai().empty() ? L"—" : W(hv.goiHienTai())) << L"\nTrạng thái   : " << W(hv.trangThai());
    if (hv.ngayHetHan().ok()) o << L"\nHết hạn      : " << ngay(hv.ngayHetHan()) << L" (còn " << hv.soNgayConLai() << L" ngày)";
    if (hv.ngayBaoLuu().ok()) o << L"\nBảo lưu từ   : " << ngay(hv.ngayBaoLuu());
    o << L"\nBuổi còn lại : " << (hv.gioiHanBuoi() ? std::to_wstring(hv.soBuoiConLai()) : L"Không giới hạn") << L"\nBuổi PT còn  : " << hv.soBuoiPT();
    o << L"\nPT phụ trách : " << (hv.maPT().empty() ? L"—" : W(hv.maPT()) + L" " + tenNhanSu(hv.maPT()));
    double no = 0; for (const auto& hd : c.hoaDons()) if (hd.maHoiVien() == hv.ma()) no += hd.conNo();
    o << L"\nTổng chi tiêu: " << tien(c.tongChiTieu(hv.ma())) << L" đ\nCông nợ      : " << tien(no) << L" đ\n";
    o << L"\nLỊCH SỬ MUA / GIA HẠN GÓI\n----------------------------------------------\n";
    for (const auto& d : c.dangKys()) if (d.maHoiVien() == hv.ma())
        o << ngay(d.ngay()) << L"  " << W(d.loai()) << L"  " << W(d.tenGoi()) << L"  " << ngay(d.tuNgay()) << L" → " << ngay(d.denNgay()) << L"  " << tien(d.phi()) << L" đ" << (d.maKhuyenMai().empty() ? L"" : L"  (KM " + W(d.maKhuyenMai()) + L")") << L"\n";
    o << L"\nLỊCH SỬ RA VÀO (20 lượt gần nhất)\n----------------------------------------------\n";
    int n = 0;
    for (auto it = c.checkIns().rbegin(); it != c.checkIns().rend() && n < 20; ++it) if (it->maHoiVien() == hv.ma()) {
        o << luc(it->vao()) << L"  →  " << (it->daRa() ? gio(it->ra()) : L"đang tập") << L"  " << it->soPhut() << L" phút  " << W(it->phuongThuc()) << (it->maTu().empty() ? L"" : L"  tủ " + W(it->maTu())) << L"\n"; ++n;
    }
    if (!n && !hv.lichSuCheckIn().empty()) for (const auto& s : hv.lichSuCheckIn()) o << ngayChu(s) << L"\n";
    o << L"\nLỊCH TẬP VỚI PT\n----------------------------------------------\n";
    for (const auto& l : c.lichHens()) if (l.maHoiVien() == hv.ma()) o << luc(l.batDau()) << L"  " << l.thoiLuongPhut() << L" phút  PT " << tenNhanSu(l.maPT()) << L"  [" << W(l.trangThai()) << L"]\n";
    o << L"\nCHỈ SỐ INBODY\n----------------------------------------------\n";
    for (const auto& p : c.inBodys()) if (p.maHoiVien() == hv.ma()) {
        const auto& s = p.chiSo();
        o << ngay(p.ngay()) << L"  Cao " << thapPhan(s.chieuCao(), 0) << L"cm  Nặng " << thapPhan(s.canNang()) << L"kg  Mỡ " << thapPhan(s.phanTramMo()) << L"%  Cơ " << thapPhan(s.khoiLuongCo()) << L"kg  BMI " << thapPhan(s.bmi()) << L" (" << W(s.danhGiaBMI()) << L")\n";
    }
    o << L"\nNHẬT KÝ TẬP LUYỆN\n----------------------------------------------\n";
    for (const auto& k : c.nhatKyTaps()) if (k.maHoiVien() == hv.ma()) o << ngay(k.ngay()) << L"  " << k.thoiLuongPhut() << L" phút  " << W(k.noiDung()) << (k.maPT().empty() ? L"" : L" (PT " + tenNhanSu(k.maPT()) + L")") << L"\n";
    o << L"\nHÓA ĐƠN\n----------------------------------------------\n";
    for (const auto& hd : c.hoaDons()) if (hd.maHoiVien() == hv.ma())
        o << W(hd.ma()) << L"  " << ngayChu(hd.ngay()) << L"  " << W(hd.loai()) << L"  " << tien(hd.tongTien()) << L" đ" << (hd.conNo() > 0 ? L"  (nợ " + tien(hd.conNo()) + L")" : L"") << L"\n";
    o << L"\nCHĂM SÓC KHÁCH HÀNG\n----------------------------------------------\n";
    for (const auto& g : c.ghiChuCSKHs()) if (g.maHoiVien() == hv.ma()) o << ngay(g.ngay()) << L"  [" << W(g.loai()) << L"] " << W(g.noiDung()) << L"\n";
    for (const auto& p : c.phanHois()) if (p.maHoiVien() == hv.ma()) o << ngay(p.ngay()) << L"  Phản hồi " << sao(p.diem()) << L" " << W(p.noiDung()) << L"\n";
    return o.str();
}

void formHoiVien(const crm::HoiVien* hv) {
    const auto loaiThe = dsLoaiThe();
    std::vector<Truong> f{
        {L"Mã hội viên", hv ? Kieu::ChiDoc : Kieu::Chu, hv ? W(hv->ma()) : W(C().sinhMa("HV")), {}, true},
        {L"Họ và tên", Kieu::Chu, hv ? W(hv->ten()) : L"", {}, true},
        {L"Số điện thoại", Kieu::Chu, hv ? W(hv->sdt()) : L"", {}, true, L"VD: 0901234567"},
        {L"Ngày sinh", Kieu::Ngay, hv ? W(crm::formatDate(hv->ngaySinh())) : L"2000-01-01", {}, true},
        {L"Giới tính", Kieu::Chon, hv ? W(hv->gioiTinh()) : L"Nam", kGioiTinh},
        {L"Nghề nghiệp", Kieu::ChonSua, hv ? W(hv->ngheNghiep()) : L"", kNghe},
        {L"Email", Kieu::Chu, hv ? W(hv->email()) : L"", {}, false, L"ten@email.com"},
        {L"Sinh viên (giá ưu đãi)", Kieu::Check, hv && hv->laSinhVien() ? L"1" : L"0"},
        {L"Loại thẻ", Kieu::Chon, hv && !hv->loaiThe().empty() ? W(hv->loaiThe()) : loaiThe[0], loaiThe},
        {L"Mã thẻ / mã vạch", Kieu::Chu, hv ? W(hv->maThe()) : L"", {}, false, L"Quét hoặc nhập mã thẻ"},
        {L"Ghi chú", Kieu::NhieuDong, hv ? W(hv->ghiChu()) : L""},
    };
    std::string ma;
    if (!moForm(app().chinh, hv ? L"Sửa thông tin hội viên" : L"Thêm hội viên mới", f, [&](const KetQua& k) {
        const bool sv = k.check(7);
        const std::string the = k.chon(8) <= 0 ? std::string{} : k.chu(9);
        if (k.chon(8) > 0 && the.empty()) throw std::runtime_error("Hãy nhập mã thẻ cho loại thẻ đã chọn.");
        if (hv) {
            auto sua = C().timHoiVien(hv->ma());
            sua->capNhatThongTin(k.chu(1), k.chu(2), k.ngay(3), sv, k.chu(4), k.chu(5), k.chu(6));
            C().ganThe(hv->ma(), the, k.chu(8));
            sua->datGhiChu(k.chu(10));
            ma = hv->ma();
        } else {
            auto moi = std::make_shared<crm::HoiVien>(k.chu(0), k.chu(1), k.chu(2), k.ngay(3), sv);
            moi->capNhatThongTin(k.chu(1), k.chu(2), k.ngay(3), sv, k.chu(4), k.chu(5), k.chu(6));
            moi->ganThe(the, k.chu(8)); moi->datGhiChu(k.chu(10));
            C().themHoiVien(moi);
            ma = k.chu(0);
        }
    })) return;
    daThayDoi();
    if (!hv && hoi(L"Đã thêm hội viên " + W(ma) + L". Đăng ký gói tập ngay?")) moDangKyGoi(ma);
}
}

void moDangKyGoi(const std::string& maHV) {
    auto hv = C().timHoiVien(maHV);
    std::vector<Truong> f{
        {L"Hội viên", Kieu::ChiDoc, moTaHV(*hv)},
        {L"Gói tập", Kieu::Chon, timTrongDs(dsGoi(), [&] { std::string ma = "GT02"; for (const auto& d : C().dangKys()) if (d.maHoiVien() == maHV && !d.maGoi().empty()) ma = d.maGoi(); return ma; }()), dsGoi(), true},
        {L"Mã khuyến mãi", Kieu::Chu, L"", {}, false, L"Để trống: tự áp dụng KM tốt nhất"},
        {L"Nhân viên tư vấn", Kieu::Chon, timTrongDs(dsNhanSu(true), app().maNhanVien), dsNhanSu(true)},
        {L"Hình thức thanh toán", Kieu::Chon, L"Tiền mặt", dsThanhToan()},
        {L"Số tiền khách trả", Kieu::Tien, L"", {}, false, L"Để trống = trả đủ"},
    };
    crm::KetQuaDangKy kq;
    if (!moForm(app().chinh, L"Đăng ký / gia hạn gói tập", f, [&](const KetQua& k) {
        const auto goi = k.ma(1), km = k.chu(2);
        const auto xem = C().xemTruocPhi(maHV, goi, km);
        std::wstring msg = L"Gói: " + W(C().goi(goi).ten()) + L"\nLoại: " + W(xem.loai) + L"\nGiá niêm yết: " + tien(xem.giaGoc) + L" đ";
        if (!xem.maKhuyenMai.empty()) msg += L"\nKhuyến mãi: " + W(xem.maKhuyenMai) + L" — " + W(xem.tenKhuyenMai);
        if (C().uuDaiTheoHang()) msg += L"\nƯu đãi hạng " + W(hv->hang()) + L": -" + thapPhan(crm::GiaTheoHangThanhVien::tyLeGiamTheoHang(hv->hang()) * 100, 0) + L"%";
        msg += L"\n\nPHẢI THANH TOÁN: " + tien(xem.phi) + L" đ\nHiệu lực: " + ngay(xem.tuNgay) + L" → " + ngay(xem.denNgay);
        const double tra = k.rong(5) ? -1 : k.so(5);
        if (tra >= 0 && tra + 0.5 < xem.phi) msg += L"\nKhách trả trước: " + tien(tra) + L" đ (ghi công nợ " + tien(xem.phi - tra) + L" đ)";
        if (!hoi(msg + L"\n\nXác nhận đăng ký?", L"Xác nhận thanh toán")) throw HuyBo{};
        const auto nv = k.chon(3) <= 0 ? std::string{} : k.ma(3);
        kq = C().dangKyGoi(maHV, goi, km, nv, tra, k.chu(4));
    }, L"Tiếp tục", L"Hệ thống tự tính giá sinh viên, ưu đãi hạng thành viên và chương trình khuyến mãi đang chạy.")) return;
    daThayDoi();
    if (hoi(L"Đăng ký thành công! Hóa đơn " + W(kq.maHoaDon) + L" — " + tien(kq.phi) + L" đ.\nThẻ có hiệu lực đến " + ngay(kq.denNgay) + L".\n\nXem/in hóa đơn?"))
        xemVanBan(app().chinh, L"Hóa đơn " + W(kq.maHoaDon), vanBanHoaDon(C().timHoaDon(kq.maHoaDon)), W(kq.maHoaDon) + L".txt");
}

void moQuetThe() {
    std::wstring cach = L"Thẻ từ";
    std::wstring ketQua = L"Sẵn sàng quét thẻ.\nQuét lại cùng mã để ghi giờ ra.";
    for (;;) {
        std::vector<Truong> f{
            {L"Mã thẻ / mã hội viên", Kieu::Chu, L"", {}, true, L"Quét thẻ hoặc nhập mã rồi nhấn Enter"},
            {L"Phương thức", Kieu::Chon, cach, {L"Thẻ từ", L"Mã vạch", L"Vân tay", L"Khuôn mặt", L"Thủ công"}},
            {L"Kết quả lần quét", Kieu::ChiDoc, ketQua},
        };
        bool daGhiNhan = false;
        if (!moForm(app().chinh, L"Quét thẻ ra vào", f, [&](const KetQua& k) {
            cach = k.wchu(1);
            try {
                ketQua = L"Đã ghi nhận\n" + W(C().quetThe(k.chu(0), k.chu(1)));
                daGhiNhan = true;
            } catch (const std::logic_error& e) {
                ketQua = L"Chưa ghi nhận\n" + W(e.what());
                if (ketQua.find(L"hết hạn") != std::wstring::npos || ketQua.find(L"hết số buổi") != std::wstring::npos)
                    ketQua += L"\nMở Hội viên → Đăng ký / Gia hạn để kiểm tra gói tập.";
            }
        }, L"Ghi nhận", L"Quét lần đầu để vào, quét lại để ra.")) break;
        if (daGhiNhan) daThayDoi();
    }
}

void themTrangHoiVien(std::vector<Trang>& ds) {
    {
        Trang t;
        t.ten = L"Hội viên"; t.bieuTuong = L""; t.quyen = Q_TATCA;
        t.moTa = L"Nhấp đúp vào hội viên để mở hồ sơ và lịch sử.";
        t.boLoc = {L"Tất cả hội viên", L"Đang hoạt động", L"Sắp hết hạn (≤ 7 ngày)", L"Hết hạn", L"Bảo lưu", L"Chưa kích hoạt", L"Còn công nợ", L"Sinh viên"};
        t.cot = [](int) { return std::vector<Cot>{{L"Mã", 70}, {L"Họ tên", 166}, {L"Trạng thái", 94}, {L"Gói hiện tại", 144},
                                                  {L"Hết hạn", 90}, {L"Buổi còn", 75, true}, {L"SĐT", 105}, {L"Mã thẻ", 100}, {L"Còn (ngày)", 85, true},
                                                  {L"Buổi PT", 65, true}, {L"PT phụ trách", 130}, {L"Tập gần nhất", 100}, {L"Hạng", 80}, {L"Tuổi", 50, true}, {L"Giới tính", 70}, {L"Nghề nghiệp", 130}}; };
        t.dong = [](int f) {
            std::vector<Dong> out;
            for (const auto& [id, hv] : C().hoiViens()) {
                const auto tt = hv->trangThai();
                const int left = hv->soNgayConLai();
                double no = 0;
                if (f == 6) for (const auto& hd : C().hoaDons()) if (hd.maHoiVien() == id) no += hd.conNo();
                const bool ok = f == 0 || (f == 1 && tt == "Kích hoạt") || (f == 2 && tt == "Kích hoạt" && left >= 0 && left <= 7) || (f == 3 && tt == "Hết hạn") || (f == 4 && tt == "Bảo lưu") ||
                                (f == 5 && tt == "Mới tạo") || (f == 6 && no > 0) || (f == 7 && hv->laSinhVien());
                if (!ok) continue;
                out.push_back({W(id), W(hv->ten()), W(tt), W(hv->goiHienTai()), ngay(hv->ngayHetHan()),
                               hv->gioiHanBuoi() ? std::to_wstring(hv->soBuoiConLai()) : L"∞", W(hv->sdt()), W(hv->maThe()),
                               hv->ngayHetHan().ok() && tt == "Kích hoạt" ? std::to_wstring(left) : L"",
                               std::to_wstring(hv->soBuoiPT()), hv->maPT().empty() ? L"" : tenNhanSu(hv->maPT()), ngayChu(hv->lanCheckInCuoi()),
                               W(hv->hang()), std::to_wstring(hv->tuoi()), W(hv->gioiTinh()), W(hv->ngheNghiep())});
            }
            return out;
        };
        t.mauChu = [](const Dong& r, int) -> COLORREF {
            if (r[2] == L"Hết hạn") return mau::doDam;
            if (r[2] == L"Bảo lưu") return mau::xanh;
            if (r[2] == L"Mới tạo") return mau::phu;
            if (!r[8].empty() && docSo(r[8]) <= 7) return mau::cam;
            return CLR_DEFAULT;
        };
        t.moDong = [](int) { auto hv = hvChon(); xemVanBan(app().chinh, L"Chi tiết hội viên " + W(hv->ma()), chiTietHoiVien(*hv), W(hv->ma()) + L".txt"); };
        t.thaoTac = {
            {L"Thêm hội viên", [] { formHoiVien(nullptr); }, {}, false, true},
            {L"Sửa", [] { formHoiVien(hvChon().get()); }},
            {L"Chi tiết & lịch sử", [] { auto hv = hvChon(); xemVanBan(app().chinh, L"Chi tiết hội viên " + W(hv->ma()), chiTietHoiVien(*hv), W(hv->ma()) + L".txt"); }},
            {L"Đăng ký / Gia hạn", [] { moDangKyGoi(chon()); }},
            {L"Bảo lưu", [] {
                auto hv = hvChon();
                if (!hoi(L"Bảo lưu thẻ của " + W(hv->ten()) + L" từ hôm nay?\nKhi mở bảo lưu, số ngày đã bảo lưu được cộng bù vào hạn thẻ; lịch PT sắp tới sẽ bị hủy.")) return;
                C().baoLuu(hv->ma()); daThayDoi();
            }},
            {L"Mở bảo lưu", [] {
                auto hv = hvChon(); const int n = C().moBaoLuu(hv->ma()); daThayDoi();
                thongBao(L"Đã mở bảo lưu, cộng bù " + std::to_wstring(n) + L" ngày.\nHạn thẻ mới: " + ngay(hv->ngayHetHan()));
            }},
            {L"Chuyển nhượng", [] {
                auto hv = hvChon();
                std::vector<std::wstring> nhan;
                for (const auto& [id, x] : C().hoiViens()) if (id != hv->ma() && x->trangThai() != "Kích hoạt" && x->trangThai() != "Bảo lưu") nhan.push_back(W(id) + L" — " + W(x->ten()));
                if (nhan.empty()) throw std::runtime_error("Không có hội viên nào đủ điều kiện nhận (cần trạng thái Mới tạo hoặc Hết hạn). Hãy thêm hồ sơ người nhận trước.");
                std::vector<Truong> f{{L"Người chuyển", Kieu::ChiDoc, moTaHV(*hv)}, {L"Người nhận", Kieu::Chon, L"", nhan, true},
                                      {L"Phí chuyển nhượng", Kieu::Tien, tien(std::stod(C().caiDat("phi_chuyen_nhuong", "200000")))}, {L"Nhân viên xử lý", Kieu::Chon, timTrongDs(dsNhanSu(true), app().maNhanVien), dsNhanSu(true)}};
                if (moForm(app().chinh, L"Chuyển nhượng gói tập", f, [&](const KetQua& k) {
                    if (!hoi(L"Chuyển toàn bộ thời hạn, số buổi và buổi PT còn lại sang " + k.wchu(1) + L"?")) throw HuyBo{};
                    C().chuyenNhuong(hv->ma(), k.ma(1), k.so(2), k.chon(3) <= 0 ? std::string{} : k.ma(3));
                }, L"Chuyển nhượng")) { daThayDoi(); thongBao(L"Đã chuyển nhượng gói tập."); }
            }},
            {L"Phân công PT", [] {
                auto hv = hvChon();
                std::vector<Truong> f{{L"Hội viên", Kieu::ChiDoc, moTaHV(*hv)}, {L"Huấn luyện viên", Kieu::Chon, timTrongDs(dsPT(true), hv->maPT()), dsPT(true)}};
                if (moForm(app().chinh, L"Phân công PT", f, [&](const KetQua& k) { C().phanCongPT(hv->ma(), k.chon(1) <= 0 ? std::string{} : k.ma(1)); })) daThayDoi();
            }},
            {L"Gán thẻ", [] {
                auto hv = hvChon(); const auto loai = dsLoaiThe();
                std::vector<Truong> f{{L"Hội viên", Kieu::ChiDoc, moTaHV(*hv)}, {L"Loại thẻ", Kieu::Chon, hv->loaiThe().empty() ? loai[1] : W(hv->loaiThe()), loai},
                                      {L"Mã thẻ", Kieu::Chu, hv->maThe().empty() ? L"CARD" + W(hv->ma().substr(2)) : W(hv->maThe()), {}, false, L"Quét thẻ hoặc nhập mã"}};
                if (moForm(app().chinh, L"Gán thẻ tập", f, [&](const KetQua& k) { C().ganThe(hv->ma(), k.chon(1) <= 0 ? std::string{} : k.chu(2), k.chu(1)); })) daThayDoi();
            }},
            {L"Ghi InBody", [] {
                auto hv = hvChon();
                std::vector<Truong> f{{L"Hội viên", Kieu::ChiDoc, moTaHV(*hv)}, {L"Ngày đo", Kieu::Ngay, W(crm::formatDate(crm::today())), {}, true}, {L"Chiều cao (cm)", Kieu::So, L"", {}, true},
                                      {L"Cân nặng (kg)", Kieu::So, L"", {}, true}, {L"Tỷ lệ mỡ (%)", Kieu::So, L"", {}, true}, {L"Khối lượng cơ (kg)", Kieu::So, L"", {}, true}};
                std::wstring kq;
                if (moForm(app().chinh, L"Phiếu đo InBody", f, [&](const KetQua& k) {
                    crm::InBody ib(k.so(2), k.so(3), k.so(4), k.so(5));
                    C().ghiInBody(hv->ma(), ib, k.ngay(1));
                    kq = L"BMI: " + thapPhan(ib.bmi()) + L" — " + W(ib.danhGiaBMI());
                })) { daThayDoi(); thongBao(L"Đã lưu phiếu InBody.\n" + kq); }
            }},
            {L"Nhật ký tập", [] {
                auto hv = hvChon();
                std::vector<Truong> f{{L"Hội viên", Kieu::ChiDoc, moTaHV(*hv)}, {L"Ngày tập", Kieu::Ngay, W(crm::formatDate(crm::today())), {}, true},
                                      {L"Nội dung / bài tập", Kieu::NhieuDong, L"", {}, true}, {L"Thời lượng (phút)", Kieu::So, L"60", {}, true}, {L"PT hướng dẫn", Kieu::Chon, timTrongDs(dsPT(true), hv->maPT()), dsPT(true)}};
                if (moForm(app().chinh, L"Ghi nhật ký tập luyện", f, [&](const KetQua& k) {
                    C().ghiNhatKyTap(crm::NhatKyTap(hv->ma(), k.ngay(1), k.chu(2), k.soNguyen(3), k.chon(4) <= 0 ? std::string{} : k.ma(4)));
                })) daThayDoi();
            }},
            {L"Nhập CSV", [] {
                std::wstring path;
                if (!chonFileMo(app().chinh, path, L"CSV (*.csv)\0*.csv\0")) return;
                std::vector<std::string> errs;
                const int n = C().nhapHoiVienCSV(std::filesystem::path(path), errs);
                daThayDoi();
                std::wstring msg = L"Đã nhập " + std::to_wstring(n) + L" hội viên.\n\nĐịnh dạng cột: ma,ten,sdt,ngay_sinh(YYYY-MM-DD),gioi_tinh,nghe_nghiep,email,sinh_vien(0/1)\n";
                if (!errs.empty()) { msg += L"\nCác dòng bị bỏ qua:\n"; for (const auto& e : errs) msg += W(e) + L"\n"; }
                xemVanBan(app().chinh, L"Kết quả nhập CSV", msg);
            }},
            {L"Xóa", [] {
                auto hv = hvChon();
                if (!hoi(L"Xóa hồ sơ " + W(hv->ma()) + L" — " + W(hv->ten()) + L"?\nHóa đơn và lịch sử tài chính vẫn được giữ lại.")) return;
                C().xoaHoiVien(hv->ma()); daThayDoi();
            }, {}, true},
        };
        ds.push_back(std::move(t));
    }
    {
        Trang t;
        t.ten = L"Check-in / Ra vào"; t.bieuTuong = L""; t.quyen = Q_ADMIN | Q_QUANLY | Q_LETAN;
        t.moTa = L"Dùng Quét thẻ để ghi giờ vào hoặc giờ ra.";
        t.boLoc = {L"Hôm nay", L"Đang trong phòng", L"7 ngày qua", L"Tất cả"};
        t.cot = [](int) { return std::vector<Cot>{{L"Mã HV", 75}, {L"Họ tên", 180}, {L"Giờ vào", 140}, {L"Giờ ra", 80}, {L"Thời lượng", 90, true}, {L"Phương thức", 100}, {L"Tủ đồ", 70}, {L"Trạng thái thẻ", 110}, {L"Hết hạn", 95}, {L"Buổi còn", 80, true}}; };
        t.dong = [](int f) {
            std::vector<Dong> out;
            const auto d = crm::today();
            for (auto it = C().checkIns().rbegin(); it != C().checkIns().rend() && out.size() < 3000; ++it) {
                const auto day = crm::ngayCua(it->vao());
                if ((f == 0 && day != d) || (f == 1 && (it->daRa() || day != d)) || (f == 2 && day < crm::addDays(d, -6))) continue;
                auto found = C().hoiViens().find(it->maHoiVien());
                const crm::HoiVien* hv = found == C().hoiViens().end() ? nullptr : found->second.get();
                out.push_back({W(it->maHoiVien()), hv ? W(hv->ten()) : L"(đã xóa)", luc(it->vao()), it->daRa() ? gio(it->ra()) : (day == d ? L"đang tập" : L"—"), std::to_wstring(it->soPhut()) + L"'", W(it->phuongThuc()), W(it->maTu()),
                               hv ? W(hv->trangThai()) : L"", hv ? ngay(hv->ngayHetHan()) : L"", hv && hv->gioiHanBuoi() ? std::to_wstring(hv->soBuoiConLai()) : L"∞"});
            }
            return out;
        };
        t.mauChu = [](const Dong& r, int) -> COLORREF { return r[3] == L"đang tập" ? mau::xanhLa : CLR_DEFAULT; };
        t.chieuCaoVe = [](int) { return 96; };
        t.ve = [](HDC dc, RECT c, int) {
            const int gap = S(14), w4 = (c.right - 3 * gap) / 4, h = S(88);
            int trong = 0, tong = 0; for (const auto& tu : C().tuDos()) { ++tong; if (tu.trong()) ++trong; }
            const auto gioArr = crm::BaoCao::luotTheoGio(C(), 30);
            const auto peak = static_cast<int>(std::max_element(gioArr.begin(), gioArr.end()) - gioArr.begin());
            veKPI(dc, {0, 0, w4, h}, L"ĐANG TRONG PHÒNG", std::to_wstring(C().soNguoiTrongPhong()), L"Hội viên chưa check-out", mau::xanhLa, mau::xanhLaNhat);
            veKPI(dc, {w4 + gap, 0, 2 * w4 + gap, h}, L"LƯỢT HÔM NAY", std::to_wstring(C().soLuotHomNay()), L"Tổng lượt check-in", mau::xanh, mau::xanhNhat);
            veKPI(dc, {2 * (w4 + gap), 0, 3 * w4 + 2 * gap, h}, L"GIỜ CAO ĐIỂM", std::to_wstring(peak) + L":00 - " + std::to_wstring(peak + 1) + L":00", L"Theo dữ liệu 30 ngày", mau::cam, mau::camNhat);
            veKPI(dc, {3 * (w4 + gap), 0, c.right, h}, L"TỦ ĐỒ TRỐNG", std::to_wstring(trong) + L" / " + std::to_wstring(tong), L"Sẵn sàng gán khi check-in", mau::tim, mau::timNhat);
        };
        t.thaoTac = {
            {L"Quét thẻ (vào / ra)", [] { moQuetThe(); }, {}, false, true},
            {L"Check-in thủ công", [] {
                const auto ds2 = dsHoiVien(true);
                if (ds2.empty()) throw std::runtime_error("Không có hội viên nào đang kích hoạt.");
                std::vector<Truong> f{{L"Hội viên", Kieu::Chon, L"", ds2, true}, {L"Tự gán tủ đồ", Kieu::Check, L"1"}};
                std::wstring msg;
                if (moForm(app().chinh, L"Check-in thủ công", f, [&](const KetQua& k) {
                    const auto& luot = C().checkIn(k.ma(0), "Thủ công", k.check(1));
                    msg = L"Check-in lúc " + gio(luot.vao()) + (luot.maTu().empty() ? L"" : L", tủ " + W(luot.maTu()));
                }, L"Check-in")) { daThayDoi(); thongBao(msg); }
            }},
            {L"Check-out", [] { const int p = C().checkOut(chon()); daThayDoi(); thongBao(L"Đã check-out. Thời gian tập: " + std::to_wstring(p) + L" phút."); }},
            {L"Gán tủ đồ", [] {
                const auto ma = chon();
                std::vector<std::wstring> tu; for (const auto& x : C().tuDos()) if (x.trong()) tu.push_back(W(x.ma()) + L" — " + W(x.khuVuc()));
                if (tu.empty()) throw std::runtime_error("Không còn tủ trống.");
                std::vector<Truong> f{{L"Hội viên", Kieu::ChiDoc, W(ma) + L" — " + tenHoiVien(ma)}, {L"Tủ đồ", Kieu::Chon, L"", tu, true}};
                if (moForm(app().chinh, L"Gán tủ đồ", f, [&](const KetQua& k) { C().ganTu(k.ma(1), ma); })) daThayDoi();
            }},
            {L"Hồ sơ hội viên", [] { auto hv = hvChon(); xemVanBan(app().chinh, L"Chi tiết hội viên " + W(hv->ma()), chiTietHoiVien(*hv), W(hv->ma()) + L".txt"); }},
        };
        ds.push_back(std::move(t));
    }
}

void themTrangCSKH(std::vector<Trang>& ds) {
    {
        Trang t;
        t.ten = L"Chăm sóc KH"; t.bieuTuong = L""; t.quyen = Q_ADMIN | Q_QUANLY | Q_LETAN | Q_SALE;
        t.moTa = L"Nhắc sinh nhật & tặng quà, nhắc gia hạn, khách lâu không đến, nhắc lịch PT, soạn tin nhắn và lưu nhật ký chăm sóc.";
        t.boLoc = {L"Sinh nhật hôm nay", L"Sinh nhật trong tháng", L"Sắp hết hạn (7 ngày)", L"Đã hết hạn (30 ngày qua)", L"Lâu không đến tập (≥ 14 ngày)", L"Nhắc lịch PT (24 giờ tới)", L"Nhật ký chăm sóc"};
        t.cot = [](int f) -> std::vector<Cot> {
            if (f == 5) return {{L"Mã HV", 80}, {L"Họ tên", 180}, {L"SĐT", 110}, {L"PT", 160}, {L"Thời gian", 150}, {L"Mã lịch", 90}};
            if (f == 6) return {{L"Mã HV", 80}, {L"Họ tên", 180}, {L"Ngày", 100}, {L"Loại", 110}, {L"Nội dung", 420}, {L"Người ghi", 100}};
            return {{L"Mã HV", 80}, {L"Họ tên", 180}, {L"SĐT", 110}, {L"Thông tin", 260}, {L"Tập gần nhất", 110}, {L"Chăm sóc gần nhất", 140}, {L"Hạng", 90}};
        };
        t.dong = [](int f) {
            std::vector<Dong> out;
            const auto d = crm::today();
            auto lanCSKH = [](const std::string& ma) { std::wstring s; for (const auto& g : C().ghiChuCSKHs()) if (g.maHoiVien() == ma) s = ngay(g.ngay()) + L" " + W(g.loai()); return s; };
            auto them = [&](const crm::HoiVien& hv, const std::wstring& info) { out.push_back({W(hv.ma()), W(hv.ten()), W(hv.sdt()), info, ngayChu(hv.lanCheckInCuoi()), lanCSKH(hv.ma()), W(hv.hang())}); };
            if (f == 4) { for (const auto& [ma, ngayVang] : C().hoiVienLauKhongDen(14)) them(*C().timHoiVien(ma), L"Vắng " + std::to_wstring(ngayVang) + L" ngày"); return out; }
            if (f == 5) {
                const auto from = crm::now(), to = from + std::chrono::hours{24};
                for (const auto& l : C().lichHens()) if (l.trangThai() == "Đã đặt" && l.batDau() >= from && l.batDau() <= to) {
                    auto it = C().hoiViens().find(l.maHoiVien());
                    out.push_back({W(l.maHoiVien()), tenHoiVien(l.maHoiVien()), it == C().hoiViens().end() ? L"" : W(it->second->sdt()), tenNhanSu(l.maPT()), luc(l.batDau()), W(l.ma())});
                }
                return out;
            }
            if (f == 6) {
                for (auto it = C().ghiChuCSKHs().rbegin(); it != C().ghiChuCSKHs().rend(); ++it) out.push_back({W(it->maHoiVien()), tenHoiVien(it->maHoiVien()), ngay(it->ngay()), W(it->loai()), W(it->noiDung()), W(it->nguoiGhi())});
                return out;
            }
            for (const auto& [id, hv] : C().hoiViens()) {
                (void)id;
                if (f == 0 && crm::DichVuCSKH::laSinhNhat(hv->ngaySinh(), d)) them(*hv, L"🎂 Sinh nhật hôm nay — tròn " + std::to_wstring(hv->tuoi()) + L" tuổi");
                if (f == 1 && hv->ngaySinh().month() == d.month()) them(*hv, L"Sinh nhật " + ngay(hv->ngaySinh()).substr(0, 5));
                if (f == 2 && hv->trangThai() == "Kích hoạt" && hv->ngayHetHan().ok() && crm::DichVuCSKH::canNhacGiaHan(hv->ngayHetHan(), 7, d)) them(*hv, L"Hết hạn " + ngay(hv->ngayHetHan()) + L" (còn " + std::to_wstring(hv->soNgayConLai()) + L" ngày)");
                if (f == 3 && hv->trangThai() == "Hết hạn" && hv->ngayHetHan().ok() && hv->soNgayConLai() >= -30) them(*hv, L"Đã hết hạn " + std::to_wstring(-hv->soNgayConLai()) + L" ngày");
            }
            return out;
        };
        auto soanTin = [] {
            const auto ma = chon(); const int f = boLocHienTai();
            const std::wstring shop = W(C().caiDat("ten_phong_tap", "Fitness CRM")), hotline = W(C().caiDat("hotline", ""));
            std::wstring text;
            if (f == 5) {
                for (const auto& l : C().lichHens()) if (l.maHoiVien() == ma && l.trangThai() == "Đã đặt" && l.batDau() >= crm::now()) { text = L"[" + shop + L"] Nhắc lịch: " + W(crm::DichVuCSKH::taoNhacLichTap(l)) + L". Hẹn gặp bạn!"; break; }
            } else {
                auto hv = C().timHoiVien(ma);
                const std::wstring ten = W(hv->ten());
                if (f <= 1) text = L"[" + shop + L"] Chúc mừng sinh nhật " + ten + L"! Chúc bạn tuổi mới thật khỏe và luôn giữ vững phong độ tập luyện. Chúng tôi gửi tặng bạn một voucher ưu đãi, liên hệ quầy lễ tân để nhận nhé!";
                else if (f == 2) text = L"[" + shop + L"] Chào " + ten + L", thẻ tập của bạn sẽ hết hạn ngày " + ngay(hv->ngayHetHan()) + L". Gia hạn sớm để không gián đoạn lộ trình và nhận ưu đãi dành cho hội viên thân thiết!";
                else if (f == 3) text = L"[" + shop + L"] Chào " + ten + L", thẻ tập của bạn đã hết hạn. Quay lại ngay hôm nay để nhận ưu đãi gia hạn đặc biệt nhé!";
                else text = L"[" + shop + L"] Chào " + ten + L", lâu rồi chưa gặp bạn ở phòng tập! Đội ngũ PT luôn sẵn sàng hỗ trợ bạn quay lại guồng tập luyện.";
            }
            if (!hotline.empty()) text += L" Hotline: " + hotline + L".";
            xemVanBan(app().chinh, L"Tin nhắn chăm sóc khách hàng", text);
        };
        t.thaoTac = {
            {L"🎁 Tặng quà sinh nhật", [] {
                auto hv = C().timHoiVien(chon());
                std::vector<Truong> f{{L"Hội viên", Kieu::ChiDoc, W(hv->ma()) + L" — " + W(hv->ten())}, {L"Giảm (%)", Kieu::So, W(C().caiDat("qua_sinh_nhat_phan_tram", "20")), {}, true}, {L"Hiệu lực (ngày)", Kieu::So, L"30", {}, true}};
                std::string code;
                if (moForm(app().chinh, L"Tặng voucher sinh nhật", f, [&](const KetQua& k) { code = C().tangQuaSinhNhat(hv->ma(), k.so(1), k.soNguyen(2), app().tenDangNhap); }, L"Tặng quà")) {
                    daThayDoi();
                    xemVanBan(app().chinh, L"Voucher sinh nhật", L"Đã tạo voucher " + W(code) + L" (dùng 1 lần, chỉ dành cho " + W(hv->ten()) + L").\n\nTin nhắn gợi ý:\n[" + W(C().caiDat("ten_phong_tap", "Fitness CRM")) + L"] Chúc mừng sinh nhật " + W(hv->ten()) + L"! Tặng bạn mã " + W(code) + L" giảm " + f[1].giaTri + L"% khi gia hạn gói tập trong " + f[2].giaTri + L" ngày.");
                }
            }, {0, 1}, false, true},
            {L"Soạn tin nhắn", soanTin, {0, 1, 2, 3, 4, 5}},
            {L"Ghi chú chăm sóc", [] {
                const auto ma = chon();
                std::vector<Truong> f{{L"Hội viên", Kieu::ChiDoc, W(ma) + L" — " + tenHoiVien(ma)}, {L"Loại", Kieu::Chon, L"Gọi điện", {L"Gọi điện", L"Gia hạn", L"Sinh nhật", L"Lịch tập", L"Ưu đãi", L"Khiếu nại", L"Khác"}},
                                      {L"Nội dung", Kieu::NhieuDong, L"", {}, true}};
                if (moForm(app().chinh, L"Ghi chú chăm sóc khách hàng", f, [&](const KetQua& k) { C().ghiChuCSKH(crm::GhiChuCSKH(ma, k.chu(1), k.chu(2), crm::today(), app().tenDangNhap)); })) daThayDoi();
            }, {0, 1, 2, 3, 4, 5}},
            {L"Gửi ưu đãi cho cả danh sách", [] {
                std::vector<std::wstring> kms;
                for (const auto& k : C().khuyenMais()) if (k.conHieuLuc() && k.maHoiVien().empty()) kms.push_back(W(k.ma()) + L" — " + W(k.ten()));
                if (kms.empty()) throw std::runtime_error("Chưa có chương trình khuyến mãi nào đang chạy. Tạo khuyến mãi ở trang Gói tập & KM.");
                if (dongHienTai().empty()) throw std::runtime_error("Danh sách hiện tại trống.");
                std::vector<Truong> f{{L"Chương trình", Kieu::Chon, L"", kms, true}, {L"Tên chiến dịch", Kieu::Chu, L"Ưu đãi tri ân hội viên", {}, true}};
                std::wstring all;
                if (moForm(app().chinh, L"Gửi ưu đãi", f, [&](const KetQua& k) {
                    const auto* km = C().timKhuyenMai(k.ma(0));
                    int n = 0;
                    for (const auto& r : dongHienTai()) {
                        const auto ma = U(r[0]);
                        if (!C().hoiViens().contains(ma)) continue;
                        all += W(crm::DichVuCSKH::taoThongBaoUuDai(ma, k.chu(1), km->voucher())) + L"\n";
                        C().ghiChuCSKH(crm::GhiChuCSKH(ma, "Ưu đãi", "Gửi ưu đãi " + km->ma() + ": " + k.chu(1), crm::today(), app().tenDangNhap)); ++n;
                    }
                    all = L"Đã ghi nhận gửi cho " + std::to_wstring(n) + L" hội viên.\n\n" + all;
                }, L"Tạo tin nhắn")) { daThayDoi(); xemVanBan(app().chinh, L"Tin nhắn ưu đãi", all); }
            }, {0, 1, 2, 3, 4}},
        };
        ds.push_back(std::move(t));
    }
    {
        Trang t;
        t.ten = L"Phản hồi & đánh giá"; t.bieuTuong = L""; t.quyen = Q_TATCA;
        t.moTa = L"Ghi nhận phản hồi, chấm điểm nhân viên/PT/phòng tập và theo dõi xử lý khiếu nại.";
        t.boLoc = {L"Tất cả phản hồi", L"Chưa xử lý", L"Điểm thấp (≤ 2★)", L"Tổng hợp theo nhân sự"};
        t.cot = [](int f) -> std::vector<Cot> {
            if (f == 3) return {{L"Mã", 90}, {L"Đối tượng", 200}, {L"Số đánh giá", 110, true}, {L"Điểm TB", 90, true}, {L"Xếp hạng", 110}};
            return {{L"Mã", 75}, {L"Ngày", 95}, {L"Hội viên", 170}, {L"Đánh giá cho", 170}, {L"Điểm", 90}, {L"Nội dung", 330}, {L"Trạng thái", 90}, {L"Kết quả xử lý", 220}};
        };
        t.dong = [](int f) {
            std::vector<Dong> out;
            if (f == 3) {
                for (const auto& r : crm::BaoCao::danhGiaNhanSu(C())) out.push_back({W(r.ma), W(r.ten), std::to_wstring(r.soDanhGia), thapPhan(r.diemTrungBinh), sao(static_cast<int>(r.diemTrungBinh + 0.5))});
                return out;
            }
            for (auto it = C().phanHois().rbegin(); it != C().phanHois().rend(); ++it) {
                if ((f == 1 && it->trangThai() != "Mới") || (f == 2 && it->diem() > 2)) continue;
                out.push_back({W(it->ma()), ngay(it->ngay()), W(it->maHoiVien()) + L" " + tenHoiVien(it->maHoiVien()), tenNhanSu(it->maDoiTuong()), sao(it->diem()), W(it->noiDung()), W(it->trangThai()), W(it->xuLy())});
            }
            return out;
        };
        t.mauChu = [](const Dong& r, int f) -> COLORREF { if (f == 3) return CLR_DEFAULT; return r[6] == L"Mới" ? (r[4].find(L"★★★") == 0 ? mau::cam : mau::doDam) : CLR_DEFAULT; };
        t.thaoTac = {
            {L"＋ Thêm phản hồi", [] {
                const auto hv = dsHoiVien();
                if (hv.empty()) throw std::runtime_error("Chưa có hội viên.");
                auto dt = dsNhanSu(); dt.insert(dt.begin(), L"PHONG_TAP — Phòng tập (cơ sở vật chất, dịch vụ chung)");
                std::vector<Truong> f{{L"Hội viên", Kieu::Chon, L"", hv, true}, {L"Đánh giá cho", Kieu::Chon, L"", dt, true},
                                      {L"Điểm", Kieu::Chon, L"5 ★★★★★", {L"5 ★★★★★", L"4 ★★★★☆", L"3 ★★★☆☆", L"2 ★★☆☆☆", L"1 ★☆☆☆☆"}}, {L"Nội dung", Kieu::NhieuDong, L"", {}, true}};
                if (moForm(app().chinh, L"Phản hồi của hội viên", f, [&](const KetQua& k) { C().themPhanHoi(k.ma(0), k.ma(1), 5 - k.chon(2), k.chu(3)); })) daThayDoi();
            }, {}, false, true},
            {L"Đánh dấu đã xử lý", [] {
                const auto ma = chon();
                std::vector<Truong> f{{L"Phản hồi", Kieu::ChiDoc, W(ma)}, {L"Kết quả xử lý", Kieu::NhieuDong, L"", {}, true}};
                if (moForm(app().chinh, L"Xử lý phản hồi", f, [&](const KetQua& k) { C().xuLyPhanHoi(ma, k.chu(1)); })) daThayDoi();
            }, {0, 1, 2}},
        };
        ds.push_back(std::move(t));
    }
}
}
