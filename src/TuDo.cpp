#include "TuDo.h"
#include <stdexcept>
#include <utility>
namespace crm {
TuDo::TuDo(std::string ma, std::string khuVuc) : ma_(std::move(ma)), khuVuc_(std::move(khuVuc)) {
    if (ma_.empty()) throw std::invalid_argument("Mã tủ đồ rỗng.");
    if (khuVuc_.empty()) khuVuc_ = "Chung";
}
const std::string& TuDo::ma() const noexcept { return ma_; } const std::string& TuDo::khuVuc() const noexcept { return khuVuc_; }
const std::string& TuDo::trangThai() const noexcept { return trangThai_; } const std::string& TuDo::maHoiVien() const noexcept { return maHoiVien_; }
DateTime TuDo::batDau() const noexcept { return batDau_; } Date TuDo::hetHanThue() const noexcept { return hetHanThue_; }
bool TuDo::trong() const noexcept { return trangThai_ == "Trống"; }
void TuDo::ganTheoLuot(std::string maHV, DateTime luc) {
    if (!trong()) throw std::logic_error("Tủ " + ma_ + " không trống.");
    if (maHV.empty()) throw std::invalid_argument("Mã hội viên rỗng.");
    trangThai_ = "Đang dùng"; maHoiVien_ = std::move(maHV); batDau_ = luc; hetHanThue_ = Date{};
}
void TuDo::choThue(std::string maHV, Date hetHan) {
    if (!trong() && !(trangThai_ == "Thuê tháng" && maHoiVien_ == maHV)) throw std::logic_error("Tủ " + ma_ + " không trống.");
    if (maHV.empty() || !hetHan.ok()) throw std::invalid_argument("Thông tin thuê tủ không hợp lệ.");
    trangThai_ = "Thuê tháng"; maHoiVien_ = std::move(maHV); batDau_ = now(); hetHanThue_ = hetHan;
}
void TuDo::traTu() {
    if (trangThai_ != "Đang dùng" && trangThai_ != "Thuê tháng") throw std::logic_error("Tủ " + ma_ + " đang không có người dùng.");
    trangThai_ = "Trống"; maHoiVien_.clear(); batDau_ = {}; hetHanThue_ = Date{};
}
void TuDo::baoHong() { trangThai_ = "Hỏng"; maHoiVien_.clear(); batDau_ = {}; hetHanThue_ = Date{}; }
void TuDo::suaXong() { if (trangThai_ != "Hỏng") throw std::logic_error("Tủ không ở trạng thái hỏng."); trangThai_ = "Trống"; }
void TuDo::napTrangThai(std::string tt, std::string hv, DateTime bd, Date hh) {
    if (tt != "Trống" && tt != "Đang dùng" && tt != "Thuê tháng" && tt != "Hỏng") throw std::invalid_argument("Trạng thái tủ không hợp lệ.");
    trangThai_ = std::move(tt); maHoiVien_ = std::move(hv); batDau_ = bd; hetHanThue_ = hh;
}
}
