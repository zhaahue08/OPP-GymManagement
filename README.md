# Fitness CRM (C++20)

Đồ án OOP C++ quản lý trung tâm thể hình: hội viên, thẻ & gói tập, khuyến mãi, PT & lịch tập, check-in/ra vào, tủ đồ, bán hàng & kho, hóa đơn & công nợ, nhân sự & lương, thiết bị, chăm sóc khách hàng, phản hồi và báo cáo thống kê.

## Giao diện Win32 (khuyên dùng) — `winversion/`

Bản desktop đầy đủ dùng **Win32 API thuần** (User32, GDI, Common Controls v6, Common Dialogs), không cần cài raylib, Qt hay thư viện bên thứ ba. Có đăng nhập & phân quyền 5 vai trò, sidebar 16 màn hình, dashboard KPI + biểu đồ, form nhập liệu tự dựng, ListView ảo có tìm kiếm/sắp xếp/xuất CSV. Hướng dẫn chi tiết và bảng 29 chức năng: [winversion/README.md](winversion/README.md).

```bat
cd winversion
build.bat
FitnessCRM.exe        (tài khoản mẫu: admin / admin123)
```

## Giao diện Web — `webversion/`

Bản web dùng máy chủ C++20 và giao diện JavaScript, tái sử dụng lõi CRM và dữ liệu CSV. Hiện có đăng nhập, dashboard, tra cứu/lọc/xuất CSV hội viên, check-in và check-out. Hướng dẫn build/chạy: [webversion/README.md](webversion/README.md). Các nghiệp vụ chưa có API web vẫn dùng bản Win32 đầy đủ.

```bat
cd webversion
build.bat
build\FitnessCRMWeb.exe
```

Mở `http://127.0.0.1:8080` rồi đăng nhập `admin / admin123`.

## Menu console

Menu console được chia theo nhóm nghiệp vụ: Hội viên, PT & lịch tập, Tài chính & bán hàng, Thiết bị & vệ sinh, Báo cáo & CSKH. Menu và bảng hội viên dùng màu Dark Ocean; màu tự tắt nếu terminal không hỗ trợ ANSI. Chọn `0` trong menu con để quay lại menu nhóm, `0` ở menu chính để thoát. Console dùng chung lõi với bản Win32 nên đọc được cùng bộ dữ liệu.

## Build bằng CMake

Yêu cầu compiler C++20 (MSYS2 MinGW-w64, Visual Studio hoặc tương đương) và CMake 3.20+.

```powershell
cmake -S . -B build
cmake --build build --config Release
```

Trên Windows tạo `build/fitness_crm_win.exe` (bản Win32 đầy đủ) và `build/fitness_crm.exe` (console). Tắt bản Win32: `-DCRM_BUILD_WIN32=OFF`; bật lại dashboard tối giản cũ: `-DCRM_BUILD_GUI=ON`.

Không dùng CMake? Build console với MSYS2 PowerShell:

```powershell
$sources = Get-ChildItem src -Filter *.cpp | Where-Object { $_.Name -ne 'main_gui.cpp' } | ForEach-Object { $_.FullName }
g++ -std=c++20 -Wall -Wextra -Wpedantic -I include $sources -o fitness_crm.exe
```

## Dữ liệu

Dữ liệu lưu thành các file CSV UTF-8 trong `data/` (bản Win32: `winversion/data/`). Khi chưa có dữ liệu riêng, chương trình nạp bộ dữ liệu giả trong `sample_data/` (180 hội viên, ~650 hóa đơn, ~2.800 lượt ra vào, lịch PT, kho, chấm công...). Ghi file theo kiểu ghi ra `.tmp` rồi thay thế nên không hỏng dữ liệu nếu lỗi giữa chừng; dòng CSV hỏng được bỏ qua và báo lại thay vì làm hỏng cả lần nạp. Định dạng CSV cũ (10 cột hội viên, 3 cột PT, 6 cột hóa đơn) vẫn đọc được. Không đưa dữ liệu hội viên thật lên Git.

## Những lỗi đã sửa trong lõi

- `Date` (`std::chrono::year_month_day`) không khởi tạo chứa giá trị rác: `HoiVien::ngayHetHan_` có thể "có hạn" dù chưa mua gói. Mọi thành viên `Date` giờ khởi tạo `{}`.
- `today()` dùng giờ UTC nên từ 0h–7h sáng (giờ VN) bị lùi một ngày; nay quy đổi giờ địa phương.
- Ghi CSV làm mất độ chính xác số tiền (`2999000` → `2.999e+06`); nay ghi đủ chữ số.
- Thẻ hết hạn ngay trong ngày hết hạn; gói 1 tháng dư 1 ngày; gia hạn khi đang còn hạn bị chồng 1 ngày — đã chuẩn hóa hạn thẻ tính trọn ngày.
- Lịch PT, check-in, thiết bị, biên bản vệ sinh, thống kê PT trước đây không được lưu; nay lưu đầy đủ.
- Mã hóa đơn tự sinh không đồng nhất (`HD7` lẫn `HD0007`) và không kiểm tra trùng; check-in hai lần liên tiếp không bị chặn; lịch trùng của cùng hội viên không bị chặn.
- Bảng console lệch cột với chữ có dấu (đếm theo byte) — nay đếm theo ký tự UTF-8.

## Hồ sơ kiến trúc và báo cáo

- PlantUML: `docs/uml/` (class, use case, State, chuỗi tính phí).
- LaTeX: `docs/report/main.tex`, chương con tại `docs/report/chuong/`.
- Phân công 15 issue Jira và quy tắc Git: `JIRA_GIT.md`.

## Phân công mã nguồn cho nhóm 5 thành viên

Các file bên dưới nằm trong `src/`; file khai báo tương ứng nằm trong `include/`. Mỗi thành viên phụ trách một module và phối hợp qua các header đã thống nhất.

| Thành viên | Phạm vi | File phụ trách |
|---|---|---|
| **TV1 — Hội viên & State Pattern** | Hồ sơ hội viên, thẻ, vòng đời thẻ (Mới tạo, Kích hoạt, Bảo lưu, Hết hạn), bảo lưu cộng bù ngày. | `Nguoi.cpp`, `HoiVien.cpp`, `TrangThaiThe.cpp` |
| **TV2 — Dịch vụ, tài chính & thanh toán** | Gói tập linh hoạt, chiến lược giá (Strategy), voucher & khuyến mãi, thu/chi, hóa đơn & công nợ, hàng hóa & kho. | `GoiDichVu.cpp`, `Voucher.cpp`, `KhuyenMai.cpp`, `GiaoDich.cpp`, `HoaDon.cpp`, `DongHang.cpp`, `SanPham.cpp` |
| **TV3 — Nhân sự, PT, booking & thời gian** | Nhân viên/PT (kế thừa, tính lương đa hình), lịch hẹn không trùng, tiện ích ngày giờ `<chrono>`. | `NhanVien.cpp`, `PT.cpp`, `LichHen.cpp`, `DateUtils.cpp` |
| **TV4 — Check-in, InBody, thiết bị & CSKH** | Ra vào, tủ đồ, InBody, thiết bị & bảo trì, vệ sinh, CSKH, các bản ghi phụ (đăng ký, chấm công, phản hồi...). | `CheckIn.cpp`, `TuDo.cpp`, `InBody.cpp`, `ThietBi.cpp`, `KiemTraVeSinh.cpp`, `DichVuCSKH.cpp`, `BanGhi.cpp` |
| **TV5 — Tích hợp & ứng dụng** | Điều phối nghiệp vụ, lưu trữ CSV, báo cáo, tài khoản, menu console và giao diện Win32. | `HethongCRM.cpp`, `LuuTru.cpp`, `BaoCao.cpp`, `TaiKhoan.cpp`, `MenuConsole.cpp`, `main.cpp`, `winversion/` |

### Quy ước phối hợp

- Chỉnh sửa giao diện công khai của class thì cập nhật cả header trong `include/` và phần định nghĩa trong `src/`.
- TV1–TV4 giữ logic nghiệp vụ trong module tương ứng; TV5 tích hợp các module qua `HethongCRM`, không sao chép logic nghiệp vụ vào giao diện.
- `main.cpp` chạy menu console. `winversion/src/` là giao diện Win32; `src/main_gui.cpp` là dashboard tối giản cũ (tùy chọn `CRM_BUILD_GUI`).
- Tạo nhánh theo quy ước và nhận Jira task theo hướng dẫn tại `JIRA_GIT.md`; tránh cùng lúc sửa một file tích hợp nếu chưa trao đổi với người phụ trách.
