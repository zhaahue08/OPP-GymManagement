#pragma once
#include "DateUtils.h"
#include "Nguoi.h"
#include <string>
namespace crm {
struct KyLuong {
    int soCong{};        // số ngày có chấm công trong tháng
    int congChuan{26};   // số công chuẩn của tháng
    double doanhSo{};    // doanh số bán hàng/gói do nhân viên phụ trách
    int soCaPT{};        // số buổi PT đã dạy (chỉ dùng cho PT)
    double doanhThuPT{}; // doanh thu các buổi PT trong kỳ
};
class NhanVien : public Nguoi {
protected:
    std::string chucVu_, sdt_;
    double luongCoBan_{}, tyLeHoaHong_{};
    Date ngayVaoLam_{};
    bool dangLam_{true};
public:
    NhanVien(std::string ma, std::string ten, std::string chucVu, std::string sdt = {}, double luongCoBan = 0, double tyLeHoaHong = 0, Date ngayVaoLam = today());
    ~NhanVien() override;
    std::string vaiTro() const override;
    const std::string& chucVu() const noexcept; const std::string& sdt() const noexcept;
    double luongCoBan() const noexcept; double tyLeHoaHong() const noexcept;
    Date ngayVaoLam() const noexcept; bool dangLam() const noexcept;
    void capNhat(std::string ten, std::string chucVu, std::string sdt, double luongCoBan, double tyLeHoaHong);
    void nghiViec() noexcept; void diLamLai() noexcept;
    double hoaHong(const KyLuong& ky) const noexcept;
    // Đa hình: PT cộng thêm thù lao theo buổi dạy.
    virtual double tinhLuong(const KyLuong& ky) const;
};
}
