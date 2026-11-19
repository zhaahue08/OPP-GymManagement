// Khai báo lớp giao diện dòng lệnh.
// crm_ là tham chiếu đến hệ thống do main tạo, vì vậy menu thao tác trên cùng dữ liệu đang được nạp.

#pragma once
#include "HethongCRM.h"
#include <string>

namespace crm {
class MenuConsole {
    HethongCRM& crm_;
    static std::string docDong(const std::string& nhan);
    static int docSoNguyen(const std::string& nhan);
    static double docSoTien(const std::string& nhan);
    static void inTieuDe(const std::string& nhom);
    void menuHoiVien();
    void menuPT();
    void menuTaiChinh();
    void menuCoSoVatChat();
    void menuBaoCaoCSKH();
    void themHoiVien();
    void dangKyGoi();
    void checkIn();
    void xemHoiVien() const;
    void doiTrangThaiThe();
    void themPT();
    void datLichPT();
    void ghiNhanCaPT();
    void banHang();
    void ghiNhanChiPhi();
    void baoCaoTaiChinh() const;
    void themThietBi();
    void kiemTraVeSinh();
    void xemThietBiCanBaoTri() const;
    void baoCaoSinhNhat() const;
    void baoCaoGiaHan() const;
    void nhacLichTap() const;
public:
    explicit MenuConsole(HethongCRM& crm);
    void run();
};
}
