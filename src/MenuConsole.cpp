// Giao diện dòng lệnh nhận lựa chọn, đọc dữ liệu và gọi các phương thức nghiệp vụ.
// Đọc theo luồng main.cpp, run(), menu nhóm, hàm thao tác, rồi đến HethongCRM.
// Ngoại lệ từ hàm nhập hoặc lớp nghiệp vụ được bắt tại vòng lặp menu để hiển thị lỗi.

#include "MenuConsole.h"
#include <algorithm>
#include <chrono>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <limits>
#include <memory>
#include <sstream>
#include <utility>
#include <vector>
#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#endif

namespace crm {
namespace {
bool g_mauConsole = false;
constexpr const char* xanhDam = "\033[38;2;0;45;55m";
constexpr const char* xanhNgoc = "\033[38;2;24;107;99m";
constexpr const char* xamBien = "\033[38;2;166;181;180m";
constexpr const char* nauCat = "\033[38;2;140;115;97m";
constexpr const char* catSang = "\033[38;2;210;175;148m";
constexpr const char* nenXanhDam = "\033[48;2;0;45;55;38;2;230;240;238m";
constexpr const char* resetMau = "\033[0m";

std::string toMau(const std::string& text, const char* color) {
    return g_mauConsole ? std::string(color) + text + resetMau : text;
}

// Bật chuỗi màu ANSI nếu đầu ra Windows là console có hỗ trợ.
// CP_UTF8 giúp đầu ra hiển thị các chuỗi UTF-8 và ký tự khung.
void batMauConsole() {
#ifdef _WIN32
    HANDLE output = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD mode = 0;
    if (output != INVALID_HANDLE_VALUE && GetConsoleMode(output, &mode)) {
        g_mauConsole = SetConsoleMode(output, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING) != 0;
        SetConsoleOutputCP(CP_UTF8);
    }
#else
    g_mauConsole = true;
#endif
}

// Đếm theo ký tự UTF-8 (không theo byte) để bảng có chữ tiếng Việt vẫn thẳng cột.
std::size_t doDai(const std::string& s) { std::size_t n = 0; for (unsigned char c : s) if ((c & 0xC0) != 0x80) ++n; return n; }
// Chuẩn bị nội dung cho một ô của bảng, đệm khoảng trắng hoặc rút gọn chuỗi dài.
std::string catCot(const std::string& value, std::size_t width) {
    if (doDai(value) <= width) return value + std::string(width - doDai(value), ' ');
    std::string out; std::size_t n = 0;
    for (std::size_t i = 0; i < value.size() && n + 3 < width; ++n) {
        const unsigned char c = static_cast<unsigned char>(value[i]);
        const std::size_t len = c < 0x80 ? 1 : (c >> 5) == 6 ? 2 : (c >> 4) == 14 ? 3 : 4;
        out += value.substr(i, len); i += len;
    }
    return out + std::string(width - n, '.');
}
}

// Giữ tham chiếu đến đối tượng hệ thống đã được main khởi tạo.
MenuConsole::MenuConsole(HethongCRM& crm):crm_(crm){}
// getline đọc cả dòng để tên hoặc nội dung có khoảng trắng vẫn được giữ.
// Dòng rỗng hoặc luồng nhập đóng làm hàm ném ngoại lệ về menu đang gọi.
std::string MenuConsole::docDong(const std::string& label){std::cout<<toMau(label,catSang)<<'\n';std::string value;if(!std::getline(std::cin,value))throw std::runtime_error("Da dong luong nhap.");if(value.empty())throw std::invalid_argument("Truong nay khong duoc de trong.");return value;}
// stoi chuyển chuỗi sang int và used cho biết bao nhiêu ký tự được đọc.
// Yêu cầu đọc hết chuỗi để từ chối dữ liệu như 12abc hoặc 1.5.
int MenuConsole::docSoNguyen(const std::string& label){const auto input=docDong(label);std::size_t used=0;int value;try{value=std::stoi(input,&used);}catch(...){throw std::invalid_argument("Vui long nhap so nguyen hop le.");}if(used!=input.size())throw std::invalid_argument("Vui long nhap so nguyen hop le.");return value;}
// stod đọc số thực, sau đó kiểm tra đọc hết chuỗi, giá trị hữu hạn và không âm.
// Quy tắc số tiền có được bằng 0 hay không còn do từng lớp nghiệp vụ kiểm tra.
double MenuConsole::docSoTien(const std::string& label){const auto input=docDong(label);std::size_t used=0;double value;try{value=std::stod(input,&used);}catch(...){throw std::invalid_argument("So tien khong dung dinh dang.");}if(used!=input.size()||!std::isfinite(value)||value<0)throw std::invalid_argument("So tien khong dung dinh dang hoac khong hop le.");return value;}
// Dựng khung tiêu đề và chỉ thêm mã màu khi đầu ra đã bật hỗ trợ.
void MenuConsole::inTieuDe(const std::string& group){const std::string edge(58,'=');const std::size_t padding=group.size()<56?56-group.size():0;const std::string title="║  "+group+std::string(padding,' ')+"║";std::cout<<'\n'<<toMau("╔"+edge+"╗",xanhNgoc)<<'\n'<<toMau(title,nenXanhDam)<<'\n'<<toMau("╚"+edge+"╝",xanhNgoc)<<'\n';}
// Vòng lặp menu chính gọi menu con bằng switch theo số vừa nhập.
// Mục 6 ghi bộ dữ liệu hiện tại ra data. Mục 0 kết thúc run để trở lại main.
// catch hiển thị lỗi rồi lặp lại, riêng EOF kết thúc menu để đầu vào đóng không gây lặp vô hạn.
void MenuConsole::run(){batMauConsole();for(;;){inTieuDe("FITNESS CRM  /  MENU CHINH");std::cout<<toMau("  1",xanhNgoc)<<". Quan ly hoi vien\n"<<toMau("  2",xanhNgoc)<<". Huan luyen vien & lich tap\n"<<toMau("  3",xanhNgoc)<<". Tai chinh & ban hang\n"<<toMau("  4",xanhNgoc)<<". Thiet bi & ve sinh\n"<<toMau("  5",xanhNgoc)<<". Bao cao & cham soc khach hang\n"<<toMau("  6",xanhNgoc)<<". Luu du lieu CSV\n"<<toMau("  0",catSang)<<". Thoat\n";try{switch(docSoNguyen("Chon nhom: ")){case 0:return;case 1:menuHoiVien();break;case 2:menuPT();break;case 3:menuTaiChinh();break;case 4:menuCoSoVatChat();break;case 5:menuBaoCaoCSKH();break;case 6:crm_.luuCSV("data");std::cout<<toMau("Da luu du lieu vao data/*.csv",xanhNgoc)<<'\n';break;default:std::cout<<toMau("Nhom khong ton tai.",nauCat)<<'\n';}}catch(const std::exception& e){if(std::cin.eof())return;std::cout<<toMau(std::string("Loi: ")+e.what(),catSang)<<'\n';}}}
// Menu hội viên lặp cho đến khi chọn 0. Mỗi lựa chọn gọi một thao tác riêng.
// Ngoại lệ được bắt tại menu này để người dùng có thể chọn lại sau khi nhập sai.
void MenuConsole::menuHoiVien(){for(;;){inTieuDe("QUAN LY HOI VIEN");std::cout<<toMau("  1",xanhNgoc)<<". Them hoi vien\n"<<toMau("  2",xanhNgoc)<<". Dang ky goi / voucher\n"<<toMau("  3",xanhNgoc)<<". Check-in\n"<<toMau("  4",xanhNgoc)<<". Danh sach hoi vien\n"<<toMau("  5",xanhNgoc)<<". Dong bang / mo bang the\n"<<toMau("  0",catSang)<<". Quay lai menu chinh\n";try{switch(docSoNguyen("Chon thao tac: ")){case 0:return;case 1:themHoiVien();break;case 2:dangKyGoi();break;case 3:checkIn();break;case 4:xemHoiVien();break;case 5:doiTrangThaiThe();break;default:std::cout<<toMau("Thao tac khong ton tai.",nauCat)<<'\n';}}catch(const std::exception& e){if(std::cin.eof())return;std::cout<<toMau(std::string("Loi: ")+e.what(),catSang)<<'\n';}}}
// Điều hướng các thao tác tạo PT, đặt lịch và ghi nhận ca dạy.
// Các điều kiện về ca làm và lịch trùng được kiểm tra trong các lớp được gọi.
void MenuConsole::menuPT(){for(;;){inTieuDe("HUAN LUYEN VIEN & LICH TAP");std::cout<<toMau("  1",xanhNgoc)<<". Them PT\n"<<toMau("  2",xanhNgoc)<<". Dat lich PT\n"<<toMau("  3",xanhNgoc)<<". Ghi nhan ca day / hoa hong\n"<<toMau("  0",catSang)<<". Quay lai menu chinh\n";try{switch(docSoNguyen("Chon thao tac: ")){case 0:return;case 1:themPT();break;case 2:datLichPT();break;case 3:ghiNhanCaPT();break;default:std::cout<<toMau("Thao tac khong ton tai.",nauCat)<<'\n';}}catch(const std::exception& e){if(std::cin.eof())return;std::cout<<toMau(std::string("Loi: ")+e.what(),catSang)<<'\n';}}}
// Điều hướng việc tạo hóa đơn nhập tay, ghi chi phí và xem tổng thu chi.
void MenuConsole::menuTaiChinh(){for(;;){inTieuDe("TAI CHINH & BAN HANG");std::cout<<toMau("  1",xanhNgoc)<<". Ban hang hoa\n"<<toMau("  2",xanhNgoc)<<". Ghi nhan chi phi\n"<<toMau("  3",xanhNgoc)<<". Bao cao doanh thu / chi phi\n"<<toMau("  0",catSang)<<". Quay lai menu chinh\n";try{switch(docSoNguyen("Chon thao tac: ")){case 0:return;case 1:banHang();break;case 2:ghiNhanChiPhi();break;case 3:baoCaoTaiChinh();break;default:std::cout<<toMau("Thao tac khong ton tai.",nauCat)<<'\n';}}catch(const std::exception& e){if(std::cin.eof())return;std::cout<<toMau(std::string("Loi: ")+e.what(),catSang)<<'\n';}}}
// Điều hướng các nghiệp vụ thiết bị, biên bản vệ sinh và danh sách cần bảo trì.
void MenuConsole::menuCoSoVatChat(){for(;;){inTieuDe("THIET BI & VE SINH");std::cout<<toMau("  1",xanhNgoc)<<". Them thiet bi\n"<<toMau("  2",xanhNgoc)<<". Ghi bien ban ve sinh\n"<<toMau("  3",xanhNgoc)<<". Xem thiet bi can bao tri\n"<<toMau("  0",catSang)<<". Quay lai menu chinh\n";try{switch(docSoNguyen("Chon thao tac: ")){case 0:return;case 1:themThietBi();break;case 2:kiemTraVeSinh();break;case 3:xemThietBiCanBaoTri();break;default:std::cout<<toMau("Thao tac khong ton tai.",nauCat)<<'\n';}}catch(const std::exception& e){if(std::cin.eof())return;std::cout<<toMau(std::string("Loi: ")+e.what(),catSang)<<'\n';}}}
// Lấy các danh sách sinh nhật, sắp hết hạn hoặc lịch sắp tới rồi hiển thị kết quả.
void MenuConsole::menuBaoCaoCSKH(){for(;;){inTieuDe("BAO CAO & CHAM SOC KHACH HANG");std::cout<<toMau("  1",xanhNgoc)<<". Sinh nhat hoi vien hom nay\n"<<toMau("  2",xanhNgoc)<<". Nhac gia han trong 7 ngay\n"<<toMau("  3",xanhNgoc)<<". Lich PT trong 24 gio\n"<<toMau("  4",xanhNgoc)<<". Bao cao tai chinh\n"<<toMau("  0",catSang)<<". Quay lai menu chinh\n";try{switch(docSoNguyen("Chon thao tac: ")){case 0:return;case 1:baoCaoSinhNhat();break;case 2:baoCaoGiaHan();break;case 3:nhacLichTap();break;case 4:baoCaoTaiChinh();break;default:std::cout<<toMau("Thao tac khong ton tai.",nauCat)<<'\n';}}catch(const std::exception& e){if(std::cin.eof())return;std::cout<<toMau(std::string("Loi: ")+e.what(),catSang)<<'\n';}}}
// Đọc hồ sơ, kiểm tra cờ sinh viên chỉ nhận 0 hoặc 1, rồi tạo HoiVien bằng make_shared.
// HethongCRM kiểm tra mã và giữ shared_ptr trong danh sách hội viên.
void MenuConsole::themHoiVien(){const auto id=docDong("Ma hoi vien: ");const auto name=docDong("Ho va ten: ");const auto phone=docDong("So dien thoai: ");const auto birth=parseDate(docDong("Ngay sinh (YYYY-MM-DD): "));const int student=docSoNguyen("La sinh vien? (1=Co, 0=Khong): ");if(student!=0&&student!=1)throw std::invalid_argument("Chi nhap 0 hoac 1.");crm_.themHoiVien(std::make_shared<HoiVien>(id,name,phone,birth,student==1));std::cout<<"Da them hoi vien.\n";}
// Số chọn 1 đến 5 được đổi thành GoiTap bằng cách trừ 1 vì enum bắt đầu từ 0.
// Voucher là unique_ptr cục bộ, chỉ được tạo khi người dùng chọn áp dụng.
// voucher.get() truyền con trỏ để tính phí. HethongCRM cập nhật thẻ và lập hóa đơn gói.
void MenuConsole::dangKyGoi(){const auto id=docDong("Ma hoi vien: ");std::cout<<"  1. 1 ngay - 36.000 VND\n  2. 1 thang - 339.000 VND (SV 240.000)\n  3. 3 thang - 899.000 VND\n  4. 6 thang - 1.499.000 VND\n  5. 13 thang - 2.999.000 VND\n";const int selection=docSoNguyen("Chon goi: ");if(selection<1||selection>5)throw std::invalid_argument("Goi tap phai tu 1 den 5.");std::unique_ptr<Voucher> voucher;const int use=docSoNguyen("Ap dung voucher? (1=Co, 0=Khong): ");if(use!=0&&use!=1)throw std::invalid_argument("Chi nhap 0 hoac 1.");if(use){const auto code=docDong("Ma voucher: ");const int type=docSoNguyen("Loai giam (1=phan tram, 2=tien mat): ");if(type!=1&&type!=2)throw std::invalid_argument("Loai voucher phai la 1 hoac 2.");const double amount=docSoTien("Gia tri giam: ");voucher=std::make_unique<Voucher>(code,type==1?LoaiGiamGia::PhanTram:LoaiGiamGia::TienMat,amount);}const double fee=crm_.dangKyGoi(id,static_cast<GoiTap>(selection-1),voucher.get());std::cout<<"Tong phi: "<<std::fixed<<std::setprecision(0)<<fee<<" VND\n";}
// HethongCRM kiểm tra lượt đang mở và gọi toán tử ++ của hội viên.
// Sau khi thành công, menu đọc lại số buổi còn lại để thông báo cho người dùng.
void MenuConsole::checkIn(){const auto id=docDong("Ma hoi vien: ");crm_.checkIn(id);const auto hv=crm_.timHoiVien(id);std::cout<<"Check-in thanh cong. ";if(hv->gioiHanBuoi())std::cout<<"So buoi con lai: "<<hv->soBuoiConLai();else std::cout<<"Goi khong gioi han buoi.";std::cout<<'\n';}
// Duyệt map hội viên và tạo từng dòng gồm mã, hồ sơ, trạng thái, hạn và số lượt đã dùng.
// Các lambda dựng viền và hàng giúp các cột dùng chung chiều rộng.
void MenuConsole::xemHoiVien()const{if(crm_.hoiViens().empty()){std::cout<<"Chua co hoi vien.\n";return;}const int widths[]={10,23,13,13,12,15,9};const std::string headers[]={"MA HV","HO VA TEN","SO DIEN THOAI","TRANG THAI","HET HAN","BUOI CON LAI","CHECK-IN"};auto border=[&](){std::string line="+";for(int width:widths)line+=std::string(static_cast<std::size_t>(width+2),'-')+"+";return line;};auto printRow=[&](const std::string values[]){std::ostringstream line;line<<'|';for(std::size_t i=0;i<7;++i)line<<' '<<catCot(values[i],static_cast<std::size_t>(widths[i]))<<" |";return line.str();};std::cout<<toMau(border(),xanhNgoc)<<'\n'<<toMau(printRow(headers),nenXanhDam)<<'\n'<<toMau(border(),xanhNgoc)<<'\n';for(const auto& [id,h]:crm_.hoiViens()){std::string expiry=h->ngayHetHan().ok()?formatDate(h->ngayHetHan()):"--";std::string sessions=h->gioiHanBuoi()?std::to_string(h->soBuoiConLai()):"Khong gioi han";const std::string values[]={id,h->ten(),h->sdt(),h->trangThai(),expiry,sessions,std::to_string(h->lichSuCheckIn().size())};std::cout<<toMau(printRow(values),xamBien)<<'\n';}std::cout<<toMau(border(),xanhNgoc)<<'\n';}
// Console gọi trực tiếp dongBang hoặc moBang của HoiVien, các hàm này đi tới baoLuu hoặc moBaoLuu.
// Đường gọi này xử lý trạng thái, hạn và số buổi ở hội viên. HethongCRM::baoLuu còn có bước kiểm tra lượt đang mở và hủy lịch sắp tới.
void MenuConsole::doiTrangThaiThe(){const auto id=docDong("Ma hoi vien: ");auto hv=crm_.timHoiVien(id);std::cout<<"Trang thai hien tai: "<<hv->trangThai()<<"\n  1. Dong bang\n  2. Mo bang\n";const int action=docSoNguyen("Chon thao tac: ");if(action==1)hv->dongBang();else if(action==2)hv->moBang();else throw std::invalid_argument("Thao tac phai la 1 hoac 2.");std::cout<<"Trang thai moi: "<<hv->trangThai()<<'\n';}
// make_unique tạo PT rồi chuyển quyền sở hữu vào HethongCRM qua tham số unique_ptr.
void MenuConsole::themPT(){const auto id=docDong("Ma PT: ");const auto name=docDong("Ho va ten: ");const auto specialty=docDong("Chuyen mon: ");crm_.themPT(std::make_unique<PT>(id,name,specialty));std::cout<<"Da them PT.\n";}
// Ghép ngày, giờ và phút thành một thời điểm chrono, sau đó cộng thời lượng để có giờ kết thúc.
// Console tạo LichHen với mã nhập tay và gọi datLich. Lõi kiểm tra thẻ, thời lượng, ca làm và xung đột lịch.
void MenuConsole::datLichPT(){const auto code=docDong("Ma lich hen: ");const auto member=docDong("Ma hoi vien: ");const auto trainer=docDong("Ma PT: ");const auto date=docDong("Ngay tap (YYYY-MM-DD): ");const int hour=docSoNguyen("Gio bat dau (0-23): ");const int minute=docSoNguyen("Phut bat dau (0-59): ");const int duration=docSoNguyen("Thoi luong (phut): ");if(duration<=0||hour<0||hour>23||minute<0||minute>59)throw std::invalid_argument("Gio hoac thoi luong khong hop le.");const auto start=std::chrono::time_point_cast<std::chrono::seconds>(std::chrono::sys_days{parseDate(date)})+std::chrono::hours{hour}+std::chrono::minutes{minute};crm_.datLich(LichHen(code,member,trainer,start,start+std::chrono::minutes{duration}));std::cout<<"Da dat lich.\n";}
// timPT trả tham chiếu tới PT đang được hệ thống quản lý.
// Ghi nhận doanh thu và ca dạy, tăng số lần chấm công rồi in hoa hồng từ dữ liệu tích lũy.
void MenuConsole::ghiNhanCaPT(){const auto id=docDong("Ma PT: ");const auto amount=docSoTien("Doanh thu buoi huan luyen: ");auto& pt=crm_.timPT(id);pt.ghiNhanBuoiDay(amount);pt.chamCong();std::cout<<"So ca: "<<pt.soCaDaDay()<<" | cong: "<<pt.soBuoiChamCong()<<" | hoa hong (10%): "<<pt.tinhHoaHong()<<" VND\n";}
// Nhập các dòng hàng vào danh sách cục bộ trước khi đưa hóa đơn vào hệ thống.
// Console hiện gộp các dòng cùng tên và đơn giá, kiểm tra tràn int trước khi cộng số lượng.
// Luồng này nhận tên và giá nhập tay rồi gọi banHang, không gọi nghiệp vụ banLe trừ tồn kho theo mã sản phẩm.
void MenuConsole::banHang() {
    const auto code = docDong("Ma hoa don: ");
    const auto member = docDong("Ma hoi vien: ");
    HoaDon invoice(code, member, formatDate(today()));
    std::vector<DongHang> lines;
    for (;;) {
        const auto name = docDong("Ten hang: ");
        const int quantity = docSoNguyen("So luong: ");
        const double price = docSoTien("Don gia (VND): ");
        DongHang line(name, quantity, price);
        const auto existing = std::find_if(lines.begin(), lines.end(), [&](const DongHang& item) {
            return item.ten == line.ten && item.donGia == line.donGia;
        });
        if (existing == lines.end()) {
            lines.push_back(std::move(line));
        } else {
            if (quantity > std::numeric_limits<int>::max() - existing->soLuong) {
                throw std::invalid_argument("Tong so luong mat hang vuot gioi han.");
            }
            existing->soLuong += quantity;
        }
        const int more = docSoNguyen("Them mat hang nua? (1=Co, 0=Khong): ");
        if (more == 0) break;
        if (more != 1) throw std::invalid_argument("Chi nhap 0 hoac 1.");
    }
    // Đưa các dòng đã kiểm tra vào hóa đơn, rồi chuyển hóa đơn sang bộ sưu tập của HethongCRM.
    for (const auto& line : lines) invoice.themHang(line.ten, line.soLuong, line.donGia);
    crm_.banHang(std::move(invoice));
    std::cout << "Da ghi nhan hoa don.\n";
}
// Tạo một GiaoDich loại Chi với ngày hiện tại, sau đó ghi nhận vào hệ thống.
void MenuConsole::ghiNhanChiPhi(){const auto id=docDong("Ma chi phi: ");const auto description=docDong("Noi dung chi: ");const auto amount=docSoTien("So tien (VND): ");crm_.ghiNhanChi(GiaoDich(id,description,today(),LoaiGiaoDich::Chi,amount));std::cout<<"Da ghi nhan chi phi.\n";}
// Đọc các tổng đã được HethongCRM tính từ hóa đơn và chi phí, rồi định dạng tiền khi in.
void MenuConsole::baoCaoTaiChinh()const{std::cout<<std::fixed<<std::setprecision(0)<<"Doanh thu: "<<crm_.tongDoanhThu()<<" VND\nChi phi: "<<crm_.tongChi()<<" VND\nSo du: "<<crm_.soDu()<<" VND\n";}
// Đọc và kiểm tra ngày bảo trì trước khi tạo thiết bị, hệ thống kiểm tra trùng mã khi thêm.
void MenuConsole::themThietBi(){const auto id=docDong("Ma thiet bi: ");const auto name=docDong("Ten thiet bi: ");const auto date=parseDate(docDong("Ngay bao tri tiep theo (YYYY-MM-DD): "));crm_.themThietBi(ThietBi(id,name,date));std::cout<<"Da them thiet bi.\n";}
// Cờ kết quả chỉ nhận 0 hoặc 1. Biên bản gắn ngày hiện tại với người kiểm tra và khu vực.
void MenuConsole::kiemTraVeSinh(){const auto area=docDong("Khu vuc: ");const auto staff=docDong("Nguoi kiem tra: ");const int clean=docSoNguyen("Dat yeu cau ve sinh? (1=Co, 0=Khong): ");if(clean!=0&&clean!=1)throw std::invalid_argument("Chi nhap 0 hoac 1.");const auto note=docDong("Ghi chu: ");crm_.ghiNhanVeSinh(KiemTraVeSinh(area,staff,today(),clean==1,note));std::cout<<"Da ghi bien ban ve sinh.\n";}
// Duyệt danh sách thiết bị và chỉ in những phần tử được canBaoTri xác định là đến hạn hoặc hỏng.
void MenuConsole::xemThietBiCanBaoTri()const{bool found=false;for(const auto& item:crm_.thietBis())if(item.canBaoTri()){std::cout<<item.ma()<<" | "<<item.ten()<<" | han bao tri "<<formatDate(item.ngayBaoTri())<<"\n";found=true;}if(!found)std::cout<<"Khong co thiet bi den han bao tri.\n";}
// HethongCRM so ngày và tháng sinh với hôm nay, menu in tên các hội viên phù hợp.
void MenuConsole::baoCaoSinhNhat()const{const auto list=crm_.nhacSinhNhat();if(list.empty())std::cout<<"Hom nay khong co hoi vien sinh nhat.\n";else for(const auto& name:list)std::cout<<"- "<<name<<'\n';}
// Bộ lọc mặc định chọn thẻ đang kích hoạt, còn hạn từ hôm nay đến hết ngày thứ 7.
void MenuConsole::baoCaoGiaHan()const{const auto list=crm_.nhacGiaHan();if(list.empty())std::cout<<"Khong co the can nhac gia han trong 7 ngay.\n";else for(const auto& name:list)std::cout<<"- "<<name<<'\n';}
// Lấy các lịch đang đặt trong khoảng mặc định từ hiện tại đến 24 giờ tiếp theo.
void MenuConsole::nhacLichTap()const{const auto list=crm_.nhacLichTap();if(list.empty())std::cout<<"Khong co lich PT trong 24 gio toi.\n";else for(const auto& event:list)std::cout<<"- "<<event<<'\n';}
}
