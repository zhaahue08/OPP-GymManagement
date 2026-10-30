#include "BaoCao.h"
#include "HethongCRM.h"
#include <algorithm>
#include <set>
namespace crm {
namespace {
Date ngayHoaDon(const HoaDon& hd) { try { return parseDate(hd.ngay()); } catch (...) { return Date{}; } }
}
std::vector<DongTaiChinh> BaoCao::taiChinh(const HethongCRM& crm, KyBaoCao ky) {
    std::map<std::string, DongTaiChinh> rows;
    auto row = [&](Date d) -> DongTaiChinh& { auto key = khoaKy(d, ky); auto& r = rows[key]; r.ky = key; return r; };
    for (const auto& hd : crm.hoaDons()) {
        const Date d = ngayHoaDon(hd); if (!d.ok()) continue;
        auto& r = row(d);
        if (hd.loai() == "Gói tập") r.doanhThuGoi += hd.tongTien();
        else if (hd.loai() == "Bán lẻ") r.doanhThuBanLe += hd.tongTien();
        else r.doanhThuDichVu += hd.tongTien();
        r.congNo += hd.conNo(); ++r.soHoaDon;
    }
    for (const auto& t : crm.thuKhacs()) row(t.ngay()).thuKhac += t.soTien();
    for (const auto& c : crm.chiPhis()) row(c.ngay()).chiPhi += c.soTien();
    std::vector<DongTaiChinh> out;
    for (auto& [k, r] : rows) { (void)k; out.push_back(r); }
    return out;
}
std::vector<DongDangKy> BaoCao::dangKy(const HethongCRM& crm, KyBaoCao ky) {
    std::map<std::string, DongDangKy> rows;
    for (const auto& d : crm.dangKys()) {
        auto key = khoaKy(d.ngay(), ky); auto& r = rows[key]; r.ky = key;
        if (d.loai() == "Mới") ++r.moi; else if (d.loai() == "Gia hạn") ++r.giaHan; else ++r.chuyenNhuong;
        r.doanhThu += d.phi();
    }
    std::vector<DongDangKy> out;
    for (auto& [k, r] : rows) { (void)k; out.push_back(r); }
    return out;
}
std::vector<DongNangSuat> BaoCao::nangSuatNhanVien(const HethongCRM& crm, Date tu, Date den) {
    auto trong = [&](Date d) { return d.ok() && tu <= d && d <= den; };
    std::vector<DongNangSuat> out;
    for (NhanVien* nv : crm.danhSachNhanSu()) {
        DongNangSuat r; r.maNhanVien = nv->ma(); r.ten = nv->ten(); r.chucVu = nv->chucVu();
        for (const auto& d : crm.dangKys()) if (d.maNhanVien() == nv->ma() && trong(d.ngay())) { ++r.soHopDong; r.doanhSoGoi += d.phi(); }
        for (const auto& hd : crm.hoaDons()) if (hd.maNhanVien() == nv->ma() && hd.loai() == "Bán lẻ" && trong(ngayHoaDon(hd))) { ++r.soHoaDonBanLe; r.doanhSoBanLe += hd.tongTien(); }
        KyLuong ky; ky.doanhSo = r.doanhSoGoi + r.doanhSoBanLe;
        r.hoaHong = nv->hoaHong(ky);
        if (r.soHopDong || r.soHoaDonBanLe || nv->chucVu() == "Sale") out.push_back(std::move(r));
    }
    std::sort(out.begin(), out.end(), [](const DongNangSuat& a, const DongNangSuat& b) { return a.doanhSoGoi + a.doanhSoBanLe > b.doanhSoGoi + b.doanhSoBanLe; });
    return out;
}
ThongKeQuayLai BaoCao::khachQuayLai(const HethongCRM& crm, int nguong) {
    ThongKeQuayLai tk;
    const Date d = today();
    for (const auto& [id, hv] : crm.hoiViens()) {
        DongQuayLai r; r.maHoiVien = id; r.ten = hv->ten(); r.trangThai = hv->trangThai();
        for (const auto& dk : crm.dangKys()) if (dk.maHoiVien() == id) ++r.soLanDangKy;
        r.soLuotTap = static_cast<int>(hv->lichSuCheckIn().size());
        r.lanCuoi = hv->lanCheckInCuoi();
        r.soNgayVang = r.lanCuoi.empty() ? soNgayGiua(hv->ngayThamGia(), d) : soNgayGiua(parseDate(r.lanCuoi), d);
        r.quayLai = r.soLanDangKy >= 2;
        ++tk.tongHoiVien;
        if (r.quayLai) ++tk.soQuayLai;
        if (!r.lanCuoi.empty() && r.soNgayVang <= 30) ++tk.soHoatDong30Ngay;
        if (r.soNgayVang >= nguong) ++tk.soLauKhongDen;
        tk.chiTiet.push_back(std::move(r));
    }
    // Tỷ lệ quay lại tính trên những hội viên đã từng mua gói.
    const auto daMua = std::count_if(tk.chiTiet.begin(), tk.chiTiet.end(), [](const DongQuayLai& r) { return r.soLanDangKy > 0; });
    tk.tyLeQuayLai = daMua ? 100.0 * tk.soQuayLai / daMua : 0.0;
    std::sort(tk.chiTiet.begin(), tk.chiTiet.end(), [](const DongQuayLai& a, const DongQuayLai& b) { return a.soLanDangKy != b.soLanDangKy ? a.soLanDangKy > b.soLanDangKy : a.soLuotTap > b.soLuotTap; });
    return tk;
}
std::array<int,24> BaoCao::luotTheoGio(const HethongCRM& crm, int soNgay) {
    std::array<int,24> h{};
    const Date from = addDays(today(), -soNgay);
    for (const auto& c : crm.checkIns()) if (ngayCua(c.vao()) >= from) ++h[static_cast<std::size_t>(gioCua(c.vao()))];
    return h;
}
std::array<int,7> BaoCao::luotTheoThu(const HethongCRM& crm, int soNgay) {
    std::array<int,7> w{};
    const Date from = addDays(today(), -soNgay);
    for (const auto& c : crm.checkIns()) if (ngayCua(c.vao()) >= from) ++w[static_cast<std::size_t>(thuTrongTuan(ngayCua(c.vao())))];
    return w;
}
std::vector<DongSanPhamBan> BaoCao::banHangTheoSanPham(const HethongCRM& crm, Date tu, Date den) {
    std::map<std::string, DongSanPhamBan> rows;
    for (const auto& hd : crm.hoaDons()) {
        const Date d = ngayHoaDon(hd);
        if (hd.loai() != "Bán lẻ" || !d.ok() || d < tu || d > den) continue;
        const double tyLe = hd.tamTinh() > 0 ? hd.tongTien() / hd.tamTinh() : 1.0; // phân bổ giảm giá theo tỷ lệ
        for (const auto& line : hd.dongHang()) {
            const std::string key = line.maSanPham.empty() ? line.ten : line.maSanPham;
            auto& r = rows[key]; r.maSanPham = line.maSanPham; r.ten = line.ten;
            r.soLuong += line.soLuong; r.doanhThu += line.thanhTien() * tyLe;
            if (auto it = crm.sanPhams().find(line.maSanPham); it != crm.sanPhams().end()) { r.danhMuc = it->second.danhMuc(); r.giaVon += it->second.giaNhap() * line.soLuong; }
        }
    }
    std::vector<DongSanPhamBan> out;
    for (auto& [k, r] : rows) { (void)k; out.push_back(r); }
    std::sort(out.begin(), out.end(), [](const DongSanPhamBan& a, const DongSanPhamBan& b) { return a.doanhThu > b.doanhThu; });
    return out;
}
std::map<std::string,double> BaoCao::chiPhiTheoDanhMuc(const HethongCRM& crm, Date tu, Date den) {
    std::map<std::string,double> out;
    for (const auto& c : crm.chiPhis()) if (tu <= c.ngay() && c.ngay() <= den) out[c.danhMuc()] += c.soTien();
    return out;
}
std::vector<DongDanhGia> BaoCao::danhGiaNhanSu(const HethongCRM& crm) {
    std::map<std::string, DongDanhGia> rows;
    for (const auto& p : crm.phanHois()) {
        auto& r = rows[p.maDoiTuong()]; r.ma = p.maDoiTuong();
        ++r.soDanhGia; r.diemTrungBinh += p.diem(); ++r.soPhieu[static_cast<std::size_t>(p.diem() - 1)];
    }
    std::vector<DongDanhGia> out;
    for (auto& [ma, r] : rows) {
        r.diemTrungBinh /= std::max(1, r.soDanhGia);
        if (ma == "PHONG_TAP") r.ten = "Phòng tập (chung)";
        else if (auto* nv = crm.timNhanSu(ma)) r.ten = nv->ten();
        else r.ten = "(đã xóa)";
        out.push_back(r);
    }
    std::sort(out.begin(), out.end(), [](const DongDanhGia& a, const DongDanhGia& b) { return a.diemTrungBinh > b.diemTrungBinh; });
    return out;
}
}
