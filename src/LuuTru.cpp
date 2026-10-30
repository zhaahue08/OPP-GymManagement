// Đọc/ghi toàn bộ dữ liệu HethongCRM ra các file CSV (UTF-8) trong một thư mục.
#include "HethongCRM.h"
#include <fstream>
#include <functional>
#include <iomanip>
#include <sstream>
#include <stdexcept>

namespace crm {
namespace {
std::string csv(const std::string& s) { std::string r="\""; for(char c:s){if(c=='\"')r+='\"';if(c=='\n'||c=='\r')c=' ';r+=c;} return r+'\"'; }
std::vector<std::string> parseCSV(const std::string& line) {
    std::vector<std::string> out; std::string field; bool quoted=false;
    for(std::size_t i=0;i<line.size();++i){char c=line[i];if(quoted){if(c=='\"'&&i+1<line.size()&&line[i+1]=='\"'){field+='\"';++i;}else if(c=='\"')quoted=false;else field+=c;}else if(c=='\"')quoted=true;else if(c==','){out.push_back(field);field.clear();}else field+=c;}
    if(quoted) throw std::runtime_error("CSV có dấu nháy chưa đóng.");
    out.push_back(field);
    return out;
}
// Ghi số tiền đầy đủ chữ số (luồng mặc định sẽ in 2999000 thành 2.999e+06 và làm mất độ chính xác).
std::string so(double v) {
    std::ostringstream out; out << std::fixed << std::setprecision(2) << v;
    std::string s = out.str();
    while (!s.empty() && s.back() == '0') s.pop_back();
    if (!s.empty() && s.back() == '.') s.pop_back();
    return s == "-0" ? "0" : s;
}
std::string cell(const std::string& s) { return csv(s); }
std::string cell(const char* s) { return csv(s); }
std::string cell(double v) { return so(v); }
std::string cell(int v) { return std::to_string(v); }
std::string cell(bool v) { return v ? "1" : "0"; }
std::string ngay(Date d) { return d.ok() ? formatDate(d) : std::string{}; }
std::string luc(DateTime t) { return t.time_since_epoch().count() == 0 ? std::string{} : formatDateTime(t); }
Date docNgay(const std::string& s) { return s.empty() ? Date{} : parseDate(s); }
DateTime docLuc(const std::string& s) { return s.empty() ? DateTime{} : parseDateTime(s); }
int soNguyen(const std::string& s, int macDinh = 0) { return s.empty() ? macDinh : std::stoi(s); }
double soThuc(const std::string& s, double macDinh = 0) { return s.empty() ? macDinh : std::stod(s); }
const std::string& cot(const std::vector<std::string>& c, std::size_t i) { static const std::string rong; return i < c.size() ? c[i] : rong; }

class TepCSV {
    std::filesystem::path dich_, tam_;
    std::ofstream f_;
public:
    TepCSV(const std::filesystem::path& dir, const std::string& ten, const std::string& header) : dich_(dir / ten), tam_(dir / (ten + ".tmp")), f_(tam_, std::ios::binary) {
        if (!f_) throw std::runtime_error("Không ghi được " + ten);
        f_ << header << '\n';
    }
    template<class... A> void dong(const A&... a) {
        bool first = true; std::string line;
        ((line += (first ? "" : ","), line += cell(a), first = false), ...);
        f_ << line << '\n';
    }
    void xong() {
        f_.close();
        if (!f_) throw std::runtime_error("Lỗi khi ghi " + dich_.filename().string());
        std::filesystem::rename(tam_, dich_); // ghi xong mới thay file cũ, tránh hỏng dữ liệu khi lỗi giữa chừng
    }
};
bool docTep(const std::filesystem::path& dir, const std::string& ten, std::size_t minCot, std::vector<std::string>& canhBao,
            const std::function<void(const std::vector<std::string>&)>& xuLy, std::vector<std::string>* header = nullptr) {
    const auto path = dir / ten;
    if (!std::filesystem::exists(path)) return false;
    std::ifstream f(path, std::ios::binary);
    if (!f) { canhBao.push_back("Không mở được " + ten); return false; }
    std::string line; std::getline(f, line);
    if (line.rfind("\xEF\xBB\xBF", 0) == 0) line.erase(0, 3);
    if (!line.empty() && line.back() == '\r') line.pop_back();
    if (header) *header = parseCSV(line);
    int n = 1;
    while (std::getline(f, line)) {
        ++n;
        if (!line.empty() && line.back() == '\r') line.pop_back();
        if (line.empty()) continue;
        try {
            auto c = parseCSV(line);
            if (c.size() < minCot) throw std::runtime_error("thiếu cột (" + std::to_string(c.size()) + "/" + std::to_string(minCot) + ")");
            xuLy(c);
        } catch (const std::exception& e) {
            canhBao.push_back(ten + " dòng " + std::to_string(n) + ": " + e.what());
        }
    }
    return true;
}
}

void HethongCRM::luuCSV(const std::filesystem::path& dir) const {
    std::filesystem::create_directories(dir);
    {
        TepCSV f(dir, "hoi_vien.csv", "ma,ten,sdt,ngay_sinh,sinh_vien,ngay_het_han,so_buoi,gioi_han_buoi,trang_thai,lich_su_checkin,gioi_tinh,nghe_nghiep,email,hang,ma_the,loai_the,ma_pt,goi_hien_tai,ngay_tham_gia,ngay_bao_luu,so_buoi_pt,ghi_chu");
        for (const auto& [id, h] : hoiViens_) {
            std::string history;
            for (const auto& date : h->lichSuCheckIn()) { if (!history.empty()) history += ';'; history += date; }
            f.dong(id, h->ten(), h->sdt(), ngay(h->ngaySinh()), h->laSinhVien(), ngay(h->ngayHetHan()), h->soBuoiConLai(), h->gioiHanBuoi(), h->trangThai(), history,
                   h->gioiTinh(), h->ngheNghiep(), h->email(), h->hang(), h->maThe(), h->loaiThe(), h->maPT(), h->goiHienTai(), ngay(h->ngayThamGia()), ngay(h->ngayBaoLuu()), h->soBuoiPT(), h->ghiChu());
        }
        f.xong();
    }
    {
        TepCSV f(dir, "pt.csv", "ma,ten,chuyen_mon,sdt,luong_co_ban,don_gia_buoi,ty_le_hoa_hong,gio_bat_dau,gio_ket_thuc,gia_buoi_hoi_vien,so_ca,doanh_thu,cham_cong,ngay_vao_lam,dang_lam");
        for (const auto& [id, p] : pts_)
            f.dong(id, p->ten(), p->chuyenMon(), p->sdt(), p->luongCoBan(), p->donGiaBuoi(), p->tyLeHoaHong(), p->gioBatDau(), p->gioKetThuc(), p->giaBuoiChoHoiVien(),
                   p->soCaDaDay(), p->doanhThuCaNhan(), p->soBuoiChamCong(), ngay(p->ngayVaoLam()), p->dangLam());
        f.xong();
    }
    {
        TepCSV f(dir, "nhan_vien.csv", "ma,ten,chuc_vu,sdt,luong_co_ban,ty_le_hoa_hong,ngay_vao_lam,dang_lam");
        for (const auto& [id, n] : nhanViens_) f.dong(id, n->ten(), n->chucVu(), n->sdt(), n->luongCoBan(), n->tyLeHoaHong(), ngay(n->ngayVaoLam()), n->dangLam());
        f.xong();
    }
    {
        TepCSV f(dir, "goi_dich_vu.csv", "ma,ten,loai,so_thang,so_ngay,so_buoi,so_buoi_pt,gia,gia_sv,dang_ban");
        for (const auto& [id, g] : goiDichVus_) f.dong(id, g.ten(), tenLoaiGoi(g.kieu()), g.soThang(), g.soNgay(), g.soBuoi(), g.soBuoiPT(), g.giaGoc(), g.giaSinhVien(), g.dangBan());
        f.xong();
    }
    {
        TepCSV f(dir, "khuyen_mai.csv", "ma,ten,loai_giam,gia_tri,tu_ngay,den_ngay,tu_dong,ma_goi,so_luot_con,ma_hoi_vien,chi_hv_moi,da_dung");
        for (const auto& k : khuyenMais_)
            f.dong(k.ma(), k.ten(), k.voucher().loai() == LoaiGiamGia::PhanTram ? "phan_tram" : "tien_mat", k.voucher().giaTri(), ngay(k.tuNgay()), ngay(k.denNgay()),
                   k.tuDong(), k.maGoi(), k.soLuotConLai(), k.maHoiVien(), k.chiHoiVienMoi(), k.daDung());
        f.xong();
    }
    {
        TepCSV f(dir, "dang_ky.csv", "ma,ma_hoi_vien,ma_goi,ten_goi,loai,ngay,tu_ngay,den_ngay,gia_goc,phi,ma_nhan_vien,ma_km,ma_hoa_don");
        for (const auto& d : dangKys_)
            f.dong(d.ma(), d.maHoiVien(), d.maGoi(), d.tenGoi(), d.loai(), ngay(d.ngay()), ngay(d.tuNgay()), ngay(d.denNgay()), d.giaGoc(), d.phi(), d.maNhanVien(), d.maKhuyenMai(), d.maHoaDon());
        f.xong();
    }
    {
        TepCSV f(dir, "hoa_don.csv", "ma,ma_hoi_vien,ngay,ten_hang,so_luong,don_gia,ma_sp,loai,phuong_thuc,da_thanh_toan,co_cong_no,giam_gia,ma_nhan_vien,ghi_chu");
        for (const auto& h : hoaDons_)
            for (const auto& line : h.dongHang())
                f.dong(h.ma(), h.maHoiVien(), h.ngay(), line.ten, line.soLuong, line.donGia, line.maSanPham, h.loai(), h.phuongThuc(), h.daThanhToan(), h.coCongNo(), h.giamGia(), h.maNhanVien(), h.ghiChu());
        f.xong();
    }
    auto ghiGiaoDich = [&](const std::string& ten, const std::vector<GiaoDich>& list) {
        TepCSV f(dir, ten, "ma,noi_dung,ngay,so_tien,danh_muc");
        for (const auto& item : list) f.dong(item.ma(), item.noiDung(), ngay(item.ngay()), item.soTien(), item.danhMuc());
        f.xong();
    };
    ghiGiaoDich("chi_phi.csv", chiPhis_);
    ghiGiaoDich("thu_khac.csv", thuKhacs_);
    {
        TepCSV f(dir, "check_in.csv", "ma_hoi_vien,vao,ra,phuong_thuc,ma_tu");
        for (const auto& c : checkIns_) f.dong(c.maHoiVien(), luc(c.vao()), luc(c.ra()), c.phuongThuc(), c.maTu());
        f.xong();
    }
    {
        TepCSV f(dir, "lich_hen.csv", "ma,ma_hoi_vien,ma_pt,bat_dau,ket_thuc,trang_thai,ghi_chu");
        for (const auto& l : lichHens_) f.dong(l.ma(), l.maHoiVien(), l.maPT(), luc(l.batDau()), luc(l.ketThuc()), l.trangThai(), l.ghiChu());
        f.xong();
    }
    {
        TepCSV f(dir, "san_pham.csv", "ma,ten,danh_muc,don_vi,gia_ban,gia_nhap,ton_kho,muc_toi_thieu,dang_ban");
        for (const auto& [id, s] : sanPhams_) f.dong(id, s.ten(), s.danhMuc(), s.donVi(), s.giaBan(), s.giaNhap(), s.tonKho(), s.mucToiThieu(), s.dangBan());
        f.xong();
    }
    {
        TepCSV f(dir, "phieu_kho.csv", "ma,ma_sp,ngay,loai,so_luong,don_gia,ghi_chu");
        for (const auto& p : phieuKhos_) f.dong(p.ma(), p.maSanPham(), ngay(p.ngay()), p.loai(), p.soLuong(), p.donGia(), p.ghiChu());
        f.xong();
    }
    {
        TepCSV f(dir, "thiet_bi.csv", "ma,ten,khu_vuc,ngay_nhap,han_bao_hanh,ngay_bao_tri,gia_tri,nha_cung_cap,trang_thai");
        for (const auto& t : thietBis_) f.dong(t.ma(), t.ten(), t.khuVuc(), ngay(t.ngayNhap()), ngay(t.hanBaoHanh()), ngay(t.ngayBaoTri()), t.giaTri(), t.nhaCungCap(), t.trangThai());
        f.xong();
    }
    {
        TepCSV f(dir, "bao_tri.csv", "ma_tb,ngay,noi_dung,chi_phi,nguoi_thuc_hien");
        for (const auto& b : baoTris_) f.dong(b.maThietBi(), ngay(b.ngay()), b.noiDung(), b.chiPhi(), b.nguoiThucHien());
        f.xong();
    }
    {
        TepCSV f(dir, "ve_sinh.csv", "khu_vuc,nguoi_kiem_tra,ngay,dat,ghi_chu");
        for (const auto& v : kiemTraVeSinhs_) f.dong(v.khuVuc(), v.nguoiKiemTra(), ngay(v.ngayKiemTra()), v.datYeuCau(), v.ghiChu());
        f.xong();
    }
    {
        TepCSV f(dir, "tu_do.csv", "ma,khu_vuc,trang_thai,ma_hoi_vien,bat_dau,het_han_thue");
        for (const auto& t : tuDos_) f.dong(t.ma(), t.khuVuc(), t.trangThai(), t.maHoiVien(), luc(t.batDau()), ngay(t.hetHanThue()));
        f.xong();
    }
    {
        TepCSV f(dir, "cham_cong.csv", "ma_nv,vao,ra");
        for (const auto& c : chamCongs_) f.dong(c.maNhanVien(), luc(c.vao()), luc(c.ra()));
        f.xong();
    }
    {
        TepCSV f(dir, "phan_hoi.csv", "ma,ma_hoi_vien,ma_doi_tuong,diem,noi_dung,ngay,trang_thai,xu_ly");
        for (const auto& p : phanHois_) f.dong(p.ma(), p.maHoiVien(), p.maDoiTuong(), p.diem(), p.noiDung(), ngay(p.ngay()), p.trangThai(), p.xuLy());
        f.xong();
    }
    {
        TepCSV f(dir, "inbody.csv", "ma_hoi_vien,ngay,chieu_cao,can_nang,mo,co");
        for (const auto& p : inBodys_) f.dong(p.maHoiVien(), ngay(p.ngay()), p.chiSo().chieuCao(), p.chiSo().canNang(), p.chiSo().phanTramMo(), p.chiSo().khoiLuongCo());
        f.xong();
    }
    {
        TepCSV f(dir, "nhat_ky_tap.csv", "ma_hoi_vien,ngay,noi_dung,thoi_luong,ma_pt");
        for (const auto& n : nhatKyTaps_) f.dong(n.maHoiVien(), ngay(n.ngay()), n.noiDung(), n.thoiLuongPhut(), n.maPT());
        f.xong();
    }
    {
        TepCSV f(dir, "cskh.csv", "ma_hoi_vien,loai,noi_dung,ngay,nguoi_ghi");
        for (const auto& g : ghiChuCSKHs_) f.dong(g.maHoiVien(), g.loai(), g.noiDung(), ngay(g.ngay()), g.nguoiGhi());
        f.xong();
    }
    {
        TepCSV f(dir, "tai_khoan.csv", "ten_dang_nhap,muoi,bam,vai_tro,ma_nhan_vien,khoa");
        for (const auto& t : taiKhoans_) f.dong(t.tenDangNhap(), t.muoi(), t.bam(), tenVaiTro(t.vaiTro()), t.maNhanVien(), t.khoa());
        f.xong();
    }
    {
        TepCSV f(dir, "cai_dat.csv", "khoa,gia_tri");
        for (const auto& [k, v] : caiDat_) f.dong(k, v);
        f.xong();
    }
}

void HethongCRM::docCSV(const std::filesystem::path& dir) {
    xoaDuLieu();
    auto& w = canhBaoNap_;
    docTep(dir, "cai_dat.csv", 2, w, [&](const auto& c) { caiDat_[c[0]] = c[1]; });
    datUuDaiTheoHang(caiDat("uu_dai_theo_hang", "0") == "1");
    const bool coGoi = docTep(dir, "goi_dich_vu.csv", 10, w, [&](const auto& c) {
        GoiDichVu g(c[0], c[1], loaiGoiTuTen(c[2]), soNguyen(c[3]), soNguyen(c[4]), soNguyen(c[5]), soNguyen(c[6]), soThuc(c[7]), soThuc(c[8]));
        g.datDangBan(c[9] != "0");
        themGoi(std::move(g));
    });
    if (!coGoi) khoiTaoGoiMacDinh();
    docTep(dir, "nhan_vien.csv", 3, w, [&](const auto& c) {
        auto nv = std::make_unique<NhanVien>(c[0], c[1], c[2], cot(c, 3), soThuc(cot(c, 4)), soThuc(cot(c, 5)), cot(c, 6).empty() ? today() : parseDate(c[6]));
        if (cot(c, 7) == "0") nv->nghiViec();
        themNhanVien(std::move(nv));
    });
    docTep(dir, "pt.csv", 3, w, [&](const auto& c) {
        std::unique_ptr<PT> pt;
        if (c.size() >= 10) pt = std::make_unique<PT>(c[0], c[1], c[2], c[3], soThuc(c[4]), soThuc(c[5]), soThuc(c[6]), soNguyen(c[7], 6), soNguyen(c[8], 21), soThuc(c[9]));
        else pt = std::make_unique<PT>(c[0], c[1], c[2]);
        if (c.size() >= 13) pt->napThongKe(soNguyen(c[10]), soThuc(c[11]), soNguyen(c[12]));
        if (cot(c, 14) == "0") pt->nghiViec();
        themPT(std::move(pt));
    });
    docTep(dir, "hoi_vien.csv", 10, w, [&](const auto& c) {
        auto h = std::make_shared<HoiVien>(c[0], c[1], c[2], parseDate(c[3]), c[4] == "1");
        if (c.size() >= 22) {
            h->capNhatThongTin(c[1], c[2], parseDate(c[3]), c[4] == "1", c[10], c[11], c[12]);
            if (!c[13].empty()) h->datHang(c[13]);
            h->ganThe(c[14], c[15]); h->ganPT(c[16]); h->datGoiHienTai(c[17]);
            if (!c[18].empty()) h->datNgayThamGia(parseDate(c[18]));
            h->datSoBuoiPT(soNguyen(c[20])); h->datGhiChu(c[21]);
        }
        if (!c[5].empty()) {
            h->kichHoat(parseDate(c[5]), soNguyen(c[6]), c[7] == "1");
            auto state = taoTrangThaiTheoTen(c[8]);
            if (state->ten() == "Bảo lưu") h->baoLuu(c.size() >= 22 && !c[19].empty() ? parseDate(c[19]) : today());
            else h->datTrangThai(std::move(state));
        }
        if (!c[9].empty()) {
            std::vector<std::string> dates; std::istringstream in(c[9]); std::string date;
            while (std::getline(in, date, ';')) if (!date.empty()) dates.push_back(date);
            h->napLichSuCheckIn(std::move(dates));
        }
        themHoiVien(std::move(h));
    });
    docTep(dir, "khuyen_mai.csv", 11, w, [&](const auto& c) {
        KhuyenMai km(c[0], c[1], c[2] == "tien_mat" ? LoaiGiamGia::TienMat : LoaiGiamGia::PhanTram, soThuc(c[3]), parseDate(c[4]), parseDate(c[5]),
                     c[6] == "1", c[7], soNguyen(c[8], -1), c[9], c[10] == "1");
        km.napDaDung(soNguyen(cot(c, 11)));
        khuyenMais_.push_back(std::move(km));
    });
    docTep(dir, "dang_ky.csv", 10, w, [&](const auto& c) {
        dangKys_.emplace_back(c[0], c[1], c[2], c[3], c[4], parseDate(c[5]), docNgay(c[6]), parseDate(c[7]), soThuc(c[8]), soThuc(c[9]), cot(c, 10), cot(c, 11), cot(c, 12));
    });
    {
        std::map<std::string, HoaDon> invoices; std::vector<std::string> order; std::map<std::string, std::vector<std::string>> meta;
        docTep(dir, "hoa_don.csv", 6, w, [&](const auto& c) {
            auto [it, inserted] = invoices.try_emplace(c[0], c[0], c[1], c[2]);
            if (inserted) { order.push_back(c[0]); meta[c[0]] = c; }
            it->second.themHang(c[3], std::stoi(c[4]), std::stod(c[5]), cot(c, 6));
        });
        for (const auto& id : order) {
            auto& hd = invoices.at(id); const auto& c = meta.at(id);
            try {
                if (c.size() >= 14) {
                    hd.datLoai(c[7].empty() ? "Bán lẻ" : c[7]); hd.datGiamGia(soThuc(c[11])); hd.datNhanVien(c[12]); hd.datGhiChu(c[13]);
                    if (c[10] == "1") hd.ghiNhanThanhToan(soThuc(c[9]), c[8]); else hd.ghiNhanThanhToan(hd.tongTien(), c[8]);
                } else if (hd.dongHang().front().ten.rfind("Goi tap", 0) == 0 || hd.dongHang().front().ten.rfind("Gói tập", 0) == 0) hd.datLoai("Gói tập");
                if (hd.maHoiVien() != KHACH_LE) (void)timHoiVien(hd.maHoiVien());
                hoaDons_.push_back(std::move(hd));
            } catch (const std::exception& e) { w.push_back("hoa_don.csv " + id + ": " + e.what()); }
        }
    }
    docTep(dir, "chi_phi.csv", 4, w, [&](const auto& c) { chiPhis_.emplace_back(c[0], c[1], parseDate(c[2]), LoaiGiaoDich::Chi, std::stod(c[3]), cot(c, 4)); });
    docTep(dir, "thu_khac.csv", 4, w, [&](const auto& c) { thuKhacs_.emplace_back(c[0], c[1], parseDate(c[2]), LoaiGiaoDich::Thu, std::stod(c[3]), cot(c, 4)); });
    docTep(dir, "check_in.csv", 2, w, [&](const auto& c) { checkIns_.emplace_back(c[0], parseDateTime(c[1]), cot(c, 3), docLuc(cot(c, 2)), cot(c, 4)); });
    docTep(dir, "lich_hen.csv", 5, w, [&](const auto& c) {
        LichHen l(c[0], c[1], c[2], parseDateTime(c[3]), parseDateTime(c[4]));
        if (c.size() >= 6) l.napTrangThai(c[5], cot(c, 6));
        lichHens_.push_back(std::move(l));
    });
    docTep(dir, "san_pham.csv", 8, w, [&](const auto& c) {
        SanPham sp(c[0], c[1], c[2], c[3], soThuc(c[4]), soThuc(c[5]), soNguyen(c[6]), soNguyen(c[7]));
        sp.datDangBan(cot(c, 8) != "0");
        themSanPham(std::move(sp));
    });
    docTep(dir, "phieu_kho.csv", 6, w, [&](const auto& c) { phieuKhos_.emplace_back(c[0], c[1], parseDate(c[2]), c[3], soNguyen(c[4]), soThuc(c[5]), cot(c, 6)); });
    docTep(dir, "thiet_bi.csv", 6, w, [&](const auto& c) {
        ThietBi tb(c[0], c[1], c[2], docNgay(c[3]).ok() ? parseDate(c[3]) : today(), docNgay(c[4]), parseDate(c[5]), soThuc(cot(c, 6)), cot(c, 7));
        if (!cot(c, 8).empty()) tb.capNhatTrangThai(c[8]);
        themThietBi(std::move(tb));
    });
    docTep(dir, "bao_tri.csv", 4, w, [&](const auto& c) { baoTris_.emplace_back(c[0], parseDate(c[1]), c[2], soThuc(c[3]), cot(c, 4)); });
    docTep(dir, "ve_sinh.csv", 4, w, [&](const auto& c) { kiemTraVeSinhs_.emplace_back(c[0], c[1], parseDate(c[2]), c[3] == "1", cot(c, 4)); });
    docTep(dir, "tu_do.csv", 3, w, [&](const auto& c) {
        TuDo t(c[0], c[1]);
        t.napTrangThai(c[2], cot(c, 3), docLuc(cot(c, 4)), docNgay(cot(c, 5)));
        themTuDo(std::move(t));
    });
    docTep(dir, "cham_cong.csv", 2, w, [&](const auto& c) { chamCongs_.emplace_back(c[0], parseDateTime(c[1]), docLuc(cot(c, 2))); });
    docTep(dir, "phan_hoi.csv", 6, w, [&](const auto& c) {
        PhanHoi p(c[0], c[1], c[2], soNguyen(c[3]), c[4], parseDate(c[5]));
        p.napTrangThai(cot(c, 6), cot(c, 7));
        phanHois_.push_back(std::move(p));
    });
    docTep(dir, "inbody.csv", 6, w, [&](const auto& c) { inBodys_.emplace_back(c[0], parseDate(c[1]), InBody(soThuc(c[2]), soThuc(c[3]), soThuc(c[4]), soThuc(c[5]))); });
    docTep(dir, "nhat_ky_tap.csv", 4, w, [&](const auto& c) { nhatKyTaps_.emplace_back(c[0], parseDate(c[1]), c[2], soNguyen(c[3]), cot(c, 4)); });
    docTep(dir, "cskh.csv", 4, w, [&](const auto& c) { ghiChuCSKHs_.emplace_back(c[0], c[1], c[2], parseDate(c[3]), cot(c, 4)); });
    docTep(dir, "tai_khoan.csv", 4, w, [&](const auto& c) {
        taiKhoans_.push_back(TaiKhoan::tuDuLieu(c[0], c[1], c[2], vaiTroTuTen(c[3]), cot(c, 4), cot(c, 5) == "1"));
    });
    if (taiKhoans_.empty()) taiKhoans_.emplace_back("admin", "admin123", VaiTro::Admin);
    capNhatTrangThaiHetHan();
}
}
namespace crm {
int HethongCRM::nhapHoiVienCSV(const std::filesystem::path& file, std::vector<std::string>& loi) {
    if (!std::filesystem::exists(file)) throw std::runtime_error("Không tìm thấy file cần nhập.");
    int added = 0;
    docTep(file.parent_path(), file.filename().string(), 4, loi, [&](const auto& c) {
        const std::string ma = c[0].empty() ? sinhMa("HV") : c[0];
        auto h = std::make_shared<HoiVien>(ma, c[1], c[2], parseDate(c[3]), cot(c, 7) == "1");
        h->capNhatThongTin(c[1], c[2], parseDate(c[3]), cot(c, 7) == "1", cot(c, 4), cot(c, 5), cot(c, 6));
        themHoiVien(std::move(h));
        ++added;
    });
    return added;
}
}
