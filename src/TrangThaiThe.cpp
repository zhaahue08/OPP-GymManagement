// State Pattern biểu diễn hành vi của từng trạng thái thẻ bằng một lớp riêng.
// HoiVien giữ một trạng thái qua unique_ptr và chuyển lời gọi cho trạng thái đó.
// Các chuyển đổi không được lớp trạng thái hỗ trợ sẽ sử dụng hàm cơ sở ném ngoại lệ.

#include "TrangThaiThe.h"
#include "HoiVien.h"
#include <stdexcept>

namespace crm {
// Các hành vi mặc định dưới đây ném lỗi. Lớp trạng thái con chỉ mở những chuyển đổi hợp lệ.
void TrangThaiThe::kichHoat(HoiVien&) const { throw std::logic_error("Không thể kích hoạt thẻ từ trạng thái hiện tại."); }
void TrangThaiThe::dongBang(HoiVien&) const { throw std::logic_error("Chỉ thẻ đang kích hoạt mới được bảo lưu."); }
void TrangThaiThe::moBang(HoiVien&) const { throw std::logic_error("Thẻ không ở trạng thái bảo lưu."); }
void TrangThaiThe::danhDauHetHan(HoiVien&) const { throw std::logic_error("Không thể chuyển sang hết hạn từ trạng thái hiện tại."); }
std::string TrangThaiMoi::ten() const { return "Mới tạo"; }
void TrangThaiMoi::kichHoat(HoiVien& hv) const { hv.datTrangThai(taoTrangThaiKichHoat()); }
std::string TrangThaiKichHoat::ten() const { return "Kích hoạt"; }
void TrangThaiKichHoat::kichHoat(HoiVien&) const {}
// Tạo State mới rồi chuyển quyền sở hữu cho HoiVien, hành vi lần gọi sau phụ thuộc State mới.
void TrangThaiKichHoat::dongBang(HoiVien& hv) const { hv.datTrangThai(taoTrangThaiDongBang()); }
void TrangThaiKichHoat::danhDauHetHan(HoiVien& hv) const { hv.datTrangThai(taoTrangThaiHetHan()); }
bool TrangThaiKichHoat::duocCheckIn() const noexcept { return true; }
std::string TrangThaiDongBang::ten() const { return "Bảo lưu"; }
void TrangThaiDongBang::kichHoat(HoiVien&) const { throw std::logic_error("Thẻ đang bảo lưu, hãy mở bảo lưu trước khi gia hạn."); }
void TrangThaiDongBang::moBang(HoiVien& hv) const { hv.datTrangThai(taoTrangThaiKichHoat()); }
void TrangThaiDongBang::danhDauHetHan(HoiVien& hv) const { hv.datTrangThai(taoTrangThaiHetHan()); }
std::string TrangThaiHetHan::ten() const { return "Hết hạn"; }
void TrangThaiHetHan::kichHoat(HoiVien& hv) const { hv.datTrangThai(taoTrangThaiKichHoat()); }
std::unique_ptr<TrangThaiThe> taoTrangThaiMoi() { return std::make_unique<TrangThaiMoi>(); }
std::unique_ptr<TrangThaiThe> taoTrangThaiKichHoat() { return std::make_unique<TrangThaiKichHoat>(); }
std::unique_ptr<TrangThaiThe> taoTrangThaiDongBang() { return std::make_unique<TrangThaiDongBang>(); }
std::unique_ptr<TrangThaiThe> taoTrangThaiHetHan() { return std::make_unique<TrangThaiHetHan>(); }
// Ánh xạ tên đọc từ CSV về đúng lớp State, có hỗ trợ một số tên cũ của trạng thái bảo lưu.
std::unique_ptr<TrangThaiThe> taoTrangThaiTheoTen(const std::string& ten) {
    if (ten == "Kích hoạt" || ten == "Kich hoat") return taoTrangThaiKichHoat();
    if (ten == "Bảo lưu" || ten == "Đóng băng" || ten == "Dong bang") return taoTrangThaiDongBang();
    if (ten == "Hết hạn" || ten == "Het han") return taoTrangThaiHetHan();
    if (ten.empty() || ten == "Mới tạo" || ten == "Moi tao") return taoTrangThaiMoi();
    throw std::invalid_argument("Trạng thái thẻ không xác định: " + ten);
}
}
