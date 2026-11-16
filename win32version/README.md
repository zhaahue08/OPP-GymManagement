# Fitness CRM — bản Win32 (Native Windows GUI)

Phần mềm quản lý phòng gym viết bằng C++20. Giao diện dùng **Win32 API thuần** có sẵn trong Windows, không dùng thư viện bên thứ ba.

| Thành phần Windows | Header / thư viện link | Dùng cho |
|---|---|---|
| User32 + GDI | `<windows.h>`, `-luser32 -lgdi32` | Cửa sổ, menu, nút bo góc tự vẽ, sidebar, thẻ KPI, biểu đồ cột, sơ đồ tủ đồ |
| Common Controls v6 | `<commctrl.h>`, `-lcomctl32` | ListView ảo (sắp xếp, tìm kiếm, tô màu dòng), DateTimePicker, StatusBar |
| Common Dialogs | `<commdlg.h>`, `-lcomdlg32` | Hộp thoại mở/lưu file (nhập CSV, xuất CSV/hóa đơn/phiếu lương) |
| UxTheme, Shell32 | `-luxtheme -lshell32` | Theme Explorer cho ListView, mở thư mục dữ liệu |

`res/app.manifest` bật Common Controls v6, khai báo DPI-aware và code page UTF-8. Giao diện tự co giãn theo DPI qua hàm `S()` nên chữ không bị mờ ở màn hình 125–150%.

Form nhập liệu không dùng file `.rc`: lớp **FormDialog** (`moForm` trong `src/Controls.cpp`) tự dựng control bằng `CreateWindowExW` từ một danh sách `Truong` (chữ, số, tiền, ngày, giờ, chọn, checkbox, nhiều dòng, mật khẩu). Mỗi trang nghiệp vụ là một `Trang` khai báo cột, bộ lọc, dữ liệu và thao tác, nên thêm/sửa màn hình không phải động vào cửa sổ chính.

## Build và chạy

Yêu cầu MSYS2 MinGW-w64 (`g++`, `windres` trong PATH).

```bat
cd winversion
build.bat
FitnessCRM.exe
```

Hoặc CMake từ thư mục gốc: `cmake -S . -B build && cmake --build build` → `build/fitness_crm_win.exe`.

Lần chạy đầu, nếu chưa có `winversion/data/`, chương trình nạp bộ dữ liệu giả trong `../sample_data/` rồi lưu bản làm việc vào `winversion/data/` (tự lưu khi đóng, mỗi 5 phút, hoặc Ctrl+S).

### Tài khoản mẫu

| Tên đăng nhập | Mật khẩu | Vai trò |
|---|---|---|
| admin | admin123 | Quản trị — toàn quyền, tài khoản & cài đặt |
| quanly | quanly123 | Quản lý — mọi nghiệp vụ, nhân sự, lương, báo cáo |
| letan | letan123 | Lễ tân — hội viên, check-in, POS, kho, tủ đồ, CSKH |
| sale | sale123 | Sale — hội viên, gói tập & KM, POS, CSKH, báo cáo |
| pt | pt123 | Huấn luyện viên — lịch PT của mình, chấm công |

Đây là tài khoản của dữ liệu mẫu; hãy đổi mật khẩu (Tệp → Đổi mật khẩu) khi dùng thật. Mật khẩu được băm kèm muối, không lưu dạng rõ.

## Chức năng theo yêu cầu

| # | Chức năng | Ở đâu trong app |
|---|---|---|
| 1 | Thông tin cá nhân (họ tên, tuổi, nghề nghiệp, sinh nhật, hạng thành viên) | Hội viên → Thêm / Sửa; hạng tự nâng theo tổng chi tiêu (Bạc 2tr, Vàng 5tr, Kim cương 10tr) |
| 2 | Thẻ tập & mã thẻ (thẻ từ, mã vạch, vân tay, khuôn mặt) | Hội viên → Gán thẻ; Check-in → Quét thẻ |
| 3 | Lịch sử sử dụng dịch vụ & tập luyện | Hội viên → Chi tiết & lịch sử (ra vào, gói, PT, InBody, nhật ký tập, hóa đơn) |
| 4 | Cảnh báo sắp hết hạn, nhắc gia hạn | Tổng quan (cảnh báo), CSKH → Sắp hết hạn, cảnh báo khi quét thẻ |
| 5 | Gia hạn, bảo lưu, chuyển nhượng | Hội viên → Đăng ký/Gia hạn (nối tiếp hạn cũ), Bảo lưu/Mở bảo lưu (cộng bù ngày), Chuyển nhượng |
| 6 | Theo dõi số buổi còn lại | Cột Buổi còn / Buổi PT; tự trừ khi check-in và khi hoàn thành buổi PT |
| 7 | Chúc mừng sinh nhật, tặng quà | CSKH → Sinh nhật → Tặng quà (tạo voucher riêng 1 lần dùng) + soạn tin nhắn |
| 8 | Phản hồi, đánh giá nhân viên/PT | Phản hồi & đánh giá (chấm sao, xử lý, tổng hợp điểm theo nhân sự) |
| 9 | Đa dạng gói tập (ngày, tháng, năm, số buổi, kèm PT) | Gói tập & KM → Tạo gói tập |
| 10 | Khuyến mãi, tự áp dụng khi thanh toán | Gói tập & KM → Khuyến mãi (tự động/nhập mã, theo gói, số lượt, chỉ HV mới); ưu đãi theo hạng (Strategy) |
| 11 | Phân công PT cho hội viên | Hội viên → Phân công PT |
| 12 | Lịch làm việc PT, tránh trùng lịch | Lịch PT; chặn trùng PT và trùng hội viên, kiểm tra ca làm của PT |
| 13 | Đặt lịch theo khung giờ trống thực tế | Lịch PT → Đặt lịch (bước 2 chỉ liệt kê giờ trống), Xem lịch trống PT |
| 14 | Số buổi huấn luyện & thu nhập PT | Hoàn thành buổi → cộng ca dạy; Báo cáo → Thu nhập PT; phiếu lương |
| 15 | Nhân viên & phân quyền | Nhân sự; Tài khoản & cài đặt (5 vai trò, sidebar/nút theo quyền) |
| 16 | Chấm công, tính lương | Chấm công & lương → vào/ra ca, bảng lương tháng, phiếu lương |
| 17 | Thu - chi, dòng tiền, công nợ | Thu chi & công nợ (thu nợ từng phần, chi phí, thu khác, KPI dòng tiền) |
| 18 | Hóa đơn, đa phương thức thanh toán | Tiền mặt / chuyển khoản / thẻ / ví; xem, sao chép, lưu hóa đơn ra file |
| 19 | Bán hàng kèm theo | Bán hàng (POS) với giỏ hàng, khách lẻ hoặc hội viên |
| 20 | Hoa hồng nhân viên | % hoa hồng trên doanh số gói + bán lẻ, có trong bảng lương và báo cáo năng suất |
| 21 | Nhập hàng, kiểm kê, báo cáo bán hàng | Kho hàng (giá vốn bình quân, phiếu nhập/xuất/kiểm kê); Báo cáo → Bán hàng theo sản phẩm |
| 22 | Doanh thu theo ngày/tuần/tháng/quý/năm | Báo cáo (5 kỳ, có biểu đồ) |
| 23 | Đăng ký mới, gia hạn, số lần gia hạn | Báo cáo → Đăng ký mới & gia hạn; Gói tập & KM → Lịch sử đăng ký |
| 24 | Chi phí & lợi nhuận theo giai đoạn | Báo cáo doanh thu (cột chi phí, lợi nhuận), Chi phí theo danh mục |
| 25 | Năng suất đội ngũ sale | Báo cáo → Năng suất nhân viên |
| 26 | Khách quay lại | Báo cáo → Khách quay lại (tỷ lệ, phân bố số lần mua) |
| 27 | Công suất phòng tập, giờ cao điểm | Tổng quan, Check-in, Báo cáo → theo giờ / theo thứ |
| 28 | Trang thiết bị (mã, ngày nhập, bảo hành, tình trạng) | Thiết bị & vệ sinh (bảo trì ghi chi phí, biên bản vệ sinh) |
| 29 | Tủ đồ thông minh, kiểm soát ra vào | Tủ đồ (sơ đồ màu, thuê tháng, báo hỏng); quét thẻ tự gán/trả tủ |

Thêm nữa: tìm kiếm tức thì (Ctrl+F), sắp xếp theo cột, xuất mọi danh sách ra CSV mở bằng Excel, nhập hội viên từ CSV, sao lưu dữ liệu, cảnh báo dòng CSV lỗi khi nạp.

## Cấu trúc

```
winversion/
├─ build.bat              build bằng MinGW (windres + g++, link tĩnh)
├─ res/                   app.manifest, app.rc, app.ico
├─ src/
│  ├─ App.h               khai báo dùng chung: chuỗi UTF-8 ↔ UTF-16, DPI, màu, Truong/Trang
│  ├─ Controls.cpp        FormDialog, hộp xem văn bản, vẽ KPI/biểu đồ, file dialog, clipboard
│  ├─ Main.cpp            cửa sổ chính, sidebar, ListView ảo, menu, phím tắt, đăng nhập
│  └─ Pages*.cpp          các trang nghiệp vụ (mỗi file một nhóm)
└─ tools/TaoDuLieuMau.cpp sinh lại bộ dữ liệu giả trong sample_data/
```

Logic nghiệp vụ nằm trong lõi chung `../include`, `../src` (lớp `HethongCRM`, `BaoCao`...); giao diện chỉ gọi lõi, không tự tính toán nghiệp vụ.

Sinh lại dữ liệu mẫu:

```bat
cd winversion\tools
g++ -std=c++20 -I ..\..\include TaoDuLieuMau.cpp ..\..\src\DateUtils.cpp ..\..\src\TaiKhoan.cpp -o TaoDuLieuMau.exe
TaoDuLieuMau.exe ..\..\sample_data
```
