#pragma once
#include "BanGhi.h"
#include "CheckIn.h"
#include "GoiDichVu.h"
#include "GiaoDich.h"
#include "HoaDon.h"
#include "HoiVien.h"
#include "KhuyenMai.h"
#include "KiemTraVeSinh.h"
#include "LichHen.h"
#include "NhanVien.h"
#include "PT.h"
#include "SanPham.h"
#include "TaiKhoan.h"
#include "ThietBi.h"
#include "TuDo.h"
#include <filesystem>
#include <map>
#include <memory>
#include <utility>
#include <vector>
namespace crm {
struct KetQuaDangKy {
    double giaGoc{}, phi{};
    std::string maHoaDon, maKhuyenMai, tenKhuyenMai, loai;
    Date tuNgay{}, denNgay{};
};
struct DongLuong {
    std::string maNhanVien, ten, chucVu;
    int soCong{}, soCaPT{};
    double soGio{}, doanhSo{}, hoaHong{}, luong{};
};
struct CanhBao {
    std::string loai, doiTuong, noiDung, ngay;
    int mucDo{}; // 0 = thông tin, 1 = cần chú ý, 2 = khẩn
};
class HethongCRM {
    std::map<std::string,std::shared_ptr<HoiVien>> hoiViens_;
    std::map<std::string,std::unique_ptr<PT>> pts_;
    std::vector<LichHen> lichHens_;
    std::vector<CheckIn> checkIns_;
    std::vector<HoaDon> hoaDons_;
    std::vector<GiaoDich> chiPhis_;
    std::vector<ThietBi> thietBis_;
    std::vector<KiemTraVeSinh> kiemTraVeSinhs_;
    std::unique_ptr<ChienLuocGia> chienLuocGia_;
    std::map<std::string,std::unique_ptr<NhanVien>> nhanViens_;
    std::map<std::string,GoiDichVu> goiDichVus_;
    std::vector<KhuyenMai> khuyenMais_;
    std::vector<DangKyGoi> dangKys_;
    std::vector<GiaoDich> thuKhacs_;
    std::map<std::string,SanPham> sanPhams_;
    std::vector<PhieuKho> phieuKhos_;
    std::vector<LichSuBaoTri> baoTris_;
    std::vector<TuDo> tuDos_;
    std::vector<ChamCong> chamCongs_;
    std::vector<PhanHoi> phanHois_;
    std::vector<PhieuInBody> inBodys_;
    std::vector<NhatKyTap> nhatKyTaps_;
    std::vector<GhiChuCSKH> ghiChuCSKHs_;
    std::vector<TaiKhoan> taiKhoans_;
    std::map<std::string,std::string> caiDat_;
    std::vector<std::string> canhBaoNap_;

    void khoiTaoGoiMacDinh();
    void xoaDuLieu();
    KetQuaDangKy tinhPhi(const HoiVien& hv, const GoiDichVu& goi, const std::string& maKhuyenMai, const KhuyenMai** kmApDung) const;
    KetQuaDangKy thucHienDangKy(HoiVien& hv, const GoiDichVu& goi, const Voucher* voucher, const std::string& maKhuyenMai,
                                const std::string& maNhanVien, double daTra, const std::string& phuongThuc);
    KhuyenMai* timKhuyenMaiSua(const std::string& ma);
    CheckIn* luotDangMo(const std::string& maHV);
    LichHen& timLich(const std::string& ma);
    void themHoaDon(HoaDon hoaDon);
public:
    HethongCRM(); ~HethongCRM();
    HethongCRM(const HethongCRM&) = delete; HethongCRM& operator=(const HethongCRM&) = delete;

    // ===== Hội viên =====
    void themHoiVien(std::shared_ptr<HoiVien> hv); void themPT(std::unique_ptr<PT> pt);
    std::shared_ptr<HoiVien> timHoiVien(const std::string& ma) const;
    std::shared_ptr<HoiVien> timHoiVienTheoThe(const std::string& maThe) const;
    PT& timPT(const std::string& ma) const;
    void xoaHoiVien(const std::string& ma);
    void ganThe(const std::string& maHV, const std::string& maThe, const std::string& loaiThe);
    void phanCongPT(const std::string& maHV, const std::string& maPT);
    void baoLuu(const std::string& maHV);
    int moBaoLuu(const std::string& maHV);
    void chuyenNhuong(const std::string& tuMa, const std::string& denMa, double phiChuyen, const std::string& maNhanVien = {});
    double tongChiTieu(const std::string& maHV) const;
    std::string capNhatHang(const std::string& maHV);
    int capNhatTrangThaiHetHan();
    void ghiInBody(const std::string& maHV, InBody chiSo, Date ngay = today());
    void ghiNhatKyTap(NhatKyTap nhatKy);
    bool laHoiVienMoi(const std::string& maHV) const;

    // ===== Gói tập, khuyến mãi =====
    double dangKyGoi(const std::string& maHV,GoiTap goi,const Voucher* voucher=nullptr);
    KetQuaDangKy dangKyGoi(const std::string& maHV, const std::string& maGoi, const std::string& maKhuyenMai = {},
                           const std::string& maNhanVien = {}, double daTra = -1, const std::string& phuongThuc = "Tiền mặt");
    KetQuaDangKy xemTruocPhi(const std::string& maHV, const std::string& maGoi, const std::string& maKhuyenMai = {}) const;
    void themGoi(GoiDichVu goi);
    GoiDichVu& goi(const std::string& ma);
    void xoaGoi(const std::string& ma);
    void themKhuyenMai(KhuyenMai km);
    void xoaKhuyenMai(const std::string& ma);
    const KhuyenMai* timKhuyenMai(const std::string& ma) const;
    void datUuDaiTheoHang(bool bat);
    bool uuDaiTheoHang() const;
    const ChienLuocGia& chienLuocGia() const noexcept;

    // ===== PT, lịch tập =====
    void datLich(LichHen lich);
    std::string datLichPT(const std::string& maHV, const std::string& maPT, DateTime batDau, int soPhut, const std::string& ghiChu = {});
    void hoanThanhLich(const std::string& ma);
    void huyLich(const std::string& ma, const std::string& lyDo = {});
    void vangLich(const std::string& ma);
    std::vector<DateTime> khungGioTrong(const std::string& maPT, Date ngay, int soPhut) const;

    // ===== Check-in / ra vào =====
    void checkIn(const std::string& maHV);
    const CheckIn& checkIn(const std::string& maHV, const std::string& phuongThuc, bool tuDongGanTu);
    int checkOut(const std::string& maHV);
    std::string quetThe(const std::string& maTheHoacMaHV, const std::string& phuongThuc);
    bool dangTrongPhong(const std::string& maHV) const;
    int soNguoiTrongPhong() const;
    int soLuotHomNay() const;

    // ===== Nhân sự, chấm công, lương =====
    void themNhanVien(std::unique_ptr<NhanVien> nv);
    NhanVien* timNhanSu(const std::string& ma) const;
    std::vector<NhanVien*> danhSachNhanSu() const;
    void chamCongVao(const std::string& maNV);
    void chamCongRa(const std::string& maNV);
    bool dangTrongCa(const std::string& maNV) const;
    std::vector<DongLuong> bangLuong(int nam, unsigned thang) const;

    // ===== Bán hàng, kho, tài chính =====
    void banHang(HoaDon hoaDon);
    std::string banLe(const std::string& maHV, const std::vector<std::pair<std::string,int>>& gioHang, double giamGia,
                      double daTra, const std::string& phuongThuc, const std::string& maNhanVien = {});
    void thuNo(const std::string& maHoaDon, double soTien, const std::string& phuongThuc);
    const HoaDon& timHoaDon(const std::string& ma) const;
    void ghiNhanChi(GiaoDich chiPhi);
    void ghiNhanThu(GiaoDich thu);
    void xoaChiPhi(const std::string& ma);
    void themSanPham(SanPham sp);
    SanPham& sanPham(const std::string& ma);
    void xoaSanPham(const std::string& ma);
    std::string nhapHang(const std::string& maSP, int soLuong, double donGia, const std::string& nhaCungCap, bool ghiChiPhi = true);
    int kiemKe(const std::string& maSP, int thucTe, const std::string& ghiChu);

    // ===== Thiết bị, vệ sinh, tủ đồ =====
    void themThietBi(ThietBi thietBi);
    ThietBi& thietBi(const std::string& ma);
    void baoTriThietBi(const std::string& maTB, const std::string& noiDung, double chiPhi, const std::string& nguoiThucHien, Date ngayTiepTheo);
    void ghiNhanVeSinh(KiemTraVeSinh bienBan);
    void themTuDo(TuDo tu);
    int themNhieuTuDo(const std::string& tienTo, int soLuong, const std::string& khuVuc);
    TuDo& tuDo(const std::string& ma);
    void xoaTuDo(const std::string& ma);
    void ganTu(const std::string& maTu, const std::string& maHV);
    void traTu(const std::string& maTu);
    std::string thueTuThang(const std::string& maTu, const std::string& maHV, int soThang, const std::string& phuongThuc);
    std::string tuCuaHoiVien(const std::string& maHV) const;

    // ===== CSKH, phản hồi =====
    std::vector<std::string> nhacSinhNhat(Date ngay=today()) const;
    std::vector<std::string> nhacGiaHan(int trongSoNgay=7) const;
    std::vector<std::string> nhacLichTap(int trongSoNgay=1) const;
    std::vector<std::pair<std::string,int>> hoiVienLauKhongDen(int soNgay = 14) const;
    std::string tangQuaSinhNhat(const std::string& maHV, double phanTram, int soNgayHieuLuc, const std::string& nguoiGhi = {});
    void ghiChuCSKH(GhiChuCSKH ghiChu);
    std::string themPhanHoi(const std::string& maHV, const std::string& maDoiTuong, int diem, const std::string& noiDung);
    void xuLyPhanHoi(const std::string& ma, const std::string& ketQua);
    std::pair<double,int> diemDanhGia(const std::string& maDoiTuong) const;
    std::vector<CanhBao> canhBao() const;

    // ===== Tài khoản, cài đặt =====
    void themTaiKhoan(TaiKhoan tk);
    const TaiKhoan* dangNhap(const std::string& ten, const std::string& matKhau) const;
    TaiKhoan& taiKhoan(const std::string& ten);
    void xoaTaiKhoan(const std::string& ten);
    std::string caiDat(const std::string& khoa, const std::string& macDinh = {}) const;
    void datCaiDat(const std::string& khoa, const std::string& giaTri);

    // ===== Tổng hợp =====
    std::string sinhMa(const std::string& tienTo) const;
    double tongDoanhThu() const noexcept;
    double tongChi() const noexcept;
    double soDu() const noexcept;
    double tongCongNo() const noexcept;
    double tongDaThu() const noexcept;

    const auto& hoiViens() const noexcept { return hoiViens_; }
    const auto& pts() const noexcept { return pts_; }
    const auto& nhanViens() const noexcept { return nhanViens_; }
    const auto& lichHens() const noexcept { return lichHens_; }
    const auto& checkIns() const noexcept { return checkIns_; }
    const auto& hoaDons() const noexcept { return hoaDons_; }
    const auto& chiPhis() const noexcept { return chiPhis_; }
    const auto& thuKhacs() const noexcept { return thuKhacs_; }
    const auto& thietBis() const noexcept { return thietBis_; }
    const auto& kiemTraVeSinhs() const noexcept { return kiemTraVeSinhs_; }
    const auto& goiDichVus() const noexcept { return goiDichVus_; }
    const auto& khuyenMais() const noexcept { return khuyenMais_; }
    const auto& dangKys() const noexcept { return dangKys_; }
    const auto& sanPhams() const noexcept { return sanPhams_; }
    const auto& phieuKhos() const noexcept { return phieuKhos_; }
    const auto& baoTris() const noexcept { return baoTris_; }
    const auto& tuDos() const noexcept { return tuDos_; }
    const auto& chamCongs() const noexcept { return chamCongs_; }
    const auto& phanHois() const noexcept { return phanHois_; }
    const auto& inBodys() const noexcept { return inBodys_; }
    const auto& nhatKyTaps() const noexcept { return nhatKyTaps_; }
    const auto& ghiChuCSKHs() const noexcept { return ghiChuCSKHs_; }
    const auto& taiKhoans() const noexcept { return taiKhoans_; }
    const auto& canhBaoNapDuLieu() const noexcept { return canhBaoNap_; }

    void luuCSV(const std::filesystem::path& thuMuc) const;
    void docCSV(const std::filesystem::path& thuMuc);
    // Nhập danh sách hội viên từ file CSV ngoài (cột: ma,ten,sdt,ngay_sinh[,gioi_tinh,nghe_nghiep,email,sinh_vien]); trả về số dòng đã thêm.
    int nhapHoiVienCSV(const std::filesystem::path& file, std::vector<std::string>& loi);
};
}
