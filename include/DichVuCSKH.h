#pragma once
#include "DateUtils.h"
namespace crm {
class LichHen;
class Voucher;
class DichVuCSKH {
public:
    static bool laSinhNhat(Date ngaySinh, Date ngayKiemTra = today()) noexcept;
    static bool canNhacGiaHan(Date ngayHetHan, int soNgayTruoc = 7, Date ngayKiemTra = today());
    static std::string taoNhacLichTap(const LichHen& lich);
    static std::string taoThongBaoUuDai(const std::string& maHoiVien,const std::string& tenChuongTrinh,const Voucher& voucher);
};
}
