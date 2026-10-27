#pragma once
#include "DateUtils.h"
#include "Nguoi.h"
#include "TrangThaiThe.h"
#include <memory>
#include <string>
#include <vector>

namespace crm {
class HoiVien : public Nguoi {
    std::string sdt_;
    Date ngaySinh_{}, ngayHetHan_{};
    bool sinhVien_{};
    int soBuoiConLai_{};
    bool gioiHanBuoi_{};
    std::vector<std::string> lichSuCheckIn_;
    std::unique_ptr<TrangThaiThe> trangThai_;
    std::string gioiTinh_, ngheNghiep_, email_, hang_{"Thường"}, maThe_, loaiThe_, maPT_, goiHienTai_, ghiChu_;
    Date ngayThamGia_{}, ngayBaoLuu_{};
    int soBuoiPT_{};
public:
    HoiVien(std::string ma, std::string ten, std::string sdt, Date ngaySinh, bool sinhVien);
    virtual ~HoiVien();
    HoiVien(const HoiVien&) = delete;
    HoiVien& operator=(const HoiVien&) = delete;
    HoiVien(HoiVien&&) noexcept = default;
    HoiVien& operator=(HoiVien&&) noexcept = default;
    const std::string& ma() const noexcept; const std::string& ten() const noexcept; const std::string& sdt() const noexcept;
    std::string vaiTro() const override;
    bool laSinhVien() const noexcept; int soBuoiConLai() const noexcept; bool gioiHanBuoi() const noexcept; Date ngayHetHan() const noexcept;
    Date ngaySinh() const noexcept;
    std::string trangThai() const; const std::vector<std::string>& lichSuCheckIn() const noexcept;
    bool duocCheckIn() const noexcept;
    void napLichSuCheckIn(std::vector<std::string> history);
    void datTrangThai(std::unique_ptr<TrangThaiThe> state);
    void kichHoat(Date hetHan, int soBuoi = 0, bool gioiHanBuoi = false); void dongBang(); void moBang(); void capNhatHetHan(Date ngay);
    HoiVien& operator++(); HoiVien operator++(int);

    // Hồ sơ mở rộng
    void capNhatThongTin(std::string ten, std::string sdt, Date ngaySinh, bool sinhVien, std::string gioiTinh, std::string ngheNghiep, std::string email);
    const std::string& gioiTinh() const noexcept; const std::string& ngheNghiep() const noexcept; const std::string& email() const noexcept;
    int tuoi(Date ngay = today()) const;
    const std::string& hang() const noexcept; void datHang(std::string hang);
    const std::string& maThe() const noexcept; const std::string& loaiThe() const noexcept; void ganThe(std::string maThe, std::string loaiThe);
    const std::string& maPT() const noexcept; void ganPT(std::string maPT);
    const std::string& goiHienTai() const noexcept; void datGoiHienTai(std::string ten);
    const std::string& ghiChu() const noexcept; void datGhiChu(std::string ghiChu);
    Date ngayThamGia() const noexcept; void datNgayThamGia(Date ngay);
    // Bảo lưu: ghi nhận ngày bắt đầu, khi mở lại cộng bù số ngày đã bảo lưu vào hạn thẻ.
    Date ngayBaoLuu() const noexcept; void baoLuu(Date tuNgay = today()); int moBaoLuu(Date ngay = today());
    int soBuoiPT() const noexcept; void napBuoiPT(int soBuoi); void datSoBuoiPT(int soBuoi); void truBuoiPT();
    std::string lanCheckInCuoi() const;
    int soNgayConLai(Date ngay = today()) const;
    void ketThucGoi();
};
}
