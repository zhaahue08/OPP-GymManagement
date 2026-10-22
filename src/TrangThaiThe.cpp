#include "TrangThaiThe.h"
#include "HoiVien.h"
#include <stdexcept>

namespace crm {
void TrangThaiThe::kichHoat(HoiVien&) const { throw std::logic_error("Khong the kich hoat tu trang thai hien tai."); }
void TrangThaiThe::dongBang(HoiVien&) const { throw std::logic_error("Khong the dong bang tu trang thai hien tai."); }
void TrangThaiThe::moBang(HoiVien&) const { throw std::logic_error("Khong the mo bang tu trang thai hien tai."); }
void TrangThaiThe::danhDauHetHan(HoiVien&) const { throw std::logic_error("Khong the chuyen sang het han tu trang thai hien tai."); }
std::string TrangThaiMoi::ten() const { return "Moi tao"; }
void TrangThaiMoi::kichHoat(HoiVien& hv) const { hv.datTrangThai(taoTrangThaiKichHoat()); }
std::string TrangThaiKichHoat::ten() const { return "Kich hoat"; }
void TrangThaiKichHoat::kichHoat(HoiVien&) const {}
void TrangThaiKichHoat::dongBang(HoiVien& hv) const { hv.datTrangThai(taoTrangThaiDongBang()); }
void TrangThaiKichHoat::danhDauHetHan(HoiVien& hv) const { hv.datTrangThai(taoTrangThaiHetHan()); }
bool TrangThaiKichHoat::duocCheckIn() const noexcept { return true; }
std::string TrangThaiDongBang::ten() const { return "Dong bang"; }
void TrangThaiDongBang::moBang(HoiVien& hv) const { hv.datTrangThai(taoTrangThaiKichHoat()); }
void TrangThaiDongBang::danhDauHetHan(HoiVien& hv) const { hv.datTrangThai(taoTrangThaiHetHan()); }
std::string TrangThaiHetHan::ten() const { return "Het han"; }
void TrangThaiHetHan::kichHoat(HoiVien& hv) const { hv.datTrangThai(taoTrangThaiKichHoat()); }
std::unique_ptr<TrangThaiThe> taoTrangThaiMoi() { return std::make_unique<TrangThaiMoi>(); }
std::unique_ptr<TrangThaiThe> taoTrangThaiKichHoat() { return std::make_unique<TrangThaiKichHoat>(); }
std::unique_ptr<TrangThaiThe> taoTrangThaiDongBang() { return std::make_unique<TrangThaiDongBang>(); }
std::unique_ptr<TrangThaiThe> taoTrangThaiHetHan() { return std::make_unique<TrangThaiHetHan>(); }
}
