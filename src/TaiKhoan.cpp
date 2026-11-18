#include "TaiKhoan.h"
#include <chrono>
#include <cstdint>
#include <functional>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <utility>
namespace crm {
std::string tenVaiTro(VaiTro v) {
    switch (v) { case VaiTro::Admin: return "Quản trị"; case VaiTro::QuanLy: return "Quản lý"; case VaiTro::LeTan: return "Lễ tân"; case VaiTro::Sale: return "Sale"; case VaiTro::PT: return "Huấn luyện viên"; }
    return "Lễ tân";
}
VaiTro vaiTroTuTen(const std::string& ten) {
    if (ten == "Quản trị" || ten == "Admin") return VaiTro::Admin;
    if (ten == "Quản lý") return VaiTro::QuanLy;
    if (ten == "Sale") return VaiTro::Sale;
    if (ten == "Huấn luyện viên" || ten == "PT") return VaiTro::PT;
    return VaiTro::LeTan;
}
namespace {
std::string taoMuoi(const std::string& seed) {
    const auto t = static_cast<std::uint64_t>(std::chrono::steady_clock::now().time_since_epoch().count());
    std::ostringstream out; out << std::hex << std::setw(16) << std::setfill('0') << (t ^ std::hash<std::string>{}(seed));
    return out.str();
}
}
TaiKhoan::TaiKhoan(std::string ten, const std::string& matKhau, VaiTro vaiTro, std::string maNV)
    : tenDangNhap_(std::move(ten)), maNhanVien_(std::move(maNV)), vaiTro_(vaiTro) {
    if (tenDangNhap_.empty() || tenDangNhap_.find_first_of(" ,\"") != std::string::npos) throw std::invalid_argument("Tên đăng nhập không hợp lệ (không chứa khoảng trắng, dấu phẩy).");
    doiMatKhau(matKhau);
}
TaiKhoan TaiKhoan::tuDuLieu(std::string ten, std::string muoi, std::string bam, VaiTro vaiTro, std::string maNV, bool khoa) {
    TaiKhoan tk(std::move(ten), "tam-thoi", vaiTro, std::move(maNV));
    if (muoi.empty() || bam.empty()) throw std::invalid_argument("Dữ liệu mật khẩu rỗng.");
    tk.muoi_ = std::move(muoi); tk.bam_ = std::move(bam); tk.khoa_ = khoa;
    return tk;
}
std::string TaiKhoan::bamMatKhau(const std::string& matKhau, const std::string& muoi) {
    std::uint64_t h = 1469598103934665603ULL;
    const std::string input = muoi + ':' + matKhau;
    for (int round = 0; round < 5000; ++round)
        for (unsigned char c : input) { h ^= c; h *= 1099511628211ULL; h ^= static_cast<std::uint64_t>(round); }
    std::ostringstream out; out << std::hex << std::setw(16) << std::setfill('0') << h;
    return out.str();
}
const std::string& TaiKhoan::tenDangNhap() const noexcept { return tenDangNhap_; } const std::string& TaiKhoan::muoi() const noexcept { return muoi_; }
const std::string& TaiKhoan::bam() const noexcept { return bam_; } const std::string& TaiKhoan::maNhanVien() const noexcept { return maNhanVien_; }
VaiTro TaiKhoan::vaiTro() const noexcept { return vaiTro_; } bool TaiKhoan::khoa() const noexcept { return khoa_; }
bool TaiKhoan::kiemTra(const std::string& matKhau) const { return !khoa_ && bamMatKhau(matKhau, muoi_) == bam_; }
void TaiKhoan::doiMatKhau(const std::string& matKhau) {
    if (matKhau.size() < 4) throw std::invalid_argument("Mật khẩu cần ít nhất 4 ký tự.");
    muoi_ = taoMuoi(tenDangNhap_ + matKhau); bam_ = bamMatKhau(matKhau, muoi_);
}
void TaiKhoan::datVaiTro(VaiTro vaiTro, std::string maNV) { vaiTro_ = vaiTro; maNhanVien_ = std::move(maNV); }
void TaiKhoan::datKhoa(bool khoa) noexcept { khoa_ = khoa; }
}
