#pragma once
#include <string>
namespace crm {
enum class VaiTro { Admin, QuanLy, LeTan, Sale, PT };
std::string tenVaiTro(VaiTro vaiTro);
VaiTro vaiTroTuTen(const std::string& ten);
// Tài khoản đăng nhập phần mềm. Mật khẩu được băm kèm muối (FNV-1a lặp) - đủ cho bài tập,
// không thay thế được thuật toán chuyên dụng như bcrypt/argon2 trong sản phẩm thật.
class TaiKhoan {
    std::string tenDangNhap_, muoi_, bam_, maNhanVien_;
    VaiTro vaiTro_{VaiTro::LeTan};
    bool khoa_{};
public:
    TaiKhoan(std::string tenDangNhap, const std::string& matKhau, VaiTro vaiTro, std::string maNhanVien = {});
    static TaiKhoan tuDuLieu(std::string tenDangNhap, std::string muoi, std::string bam, VaiTro vaiTro, std::string maNhanVien, bool khoa);
    static std::string bamMatKhau(const std::string& matKhau, const std::string& muoi);
    const std::string& tenDangNhap() const noexcept; const std::string& muoi() const noexcept; const std::string& bam() const noexcept;
    const std::string& maNhanVien() const noexcept; VaiTro vaiTro() const noexcept; bool khoa() const noexcept;
    bool kiemTra(const std::string& matKhau) const;
    void doiMatKhau(const std::string& matKhau);
    void datVaiTro(VaiTro vaiTro, std::string maNhanVien);
    void datKhoa(bool khoa) noexcept;
};
}
