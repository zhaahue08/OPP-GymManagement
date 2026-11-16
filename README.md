# Fitness CRM

Ứng dụng quản lý trung tâm thể hình, từ đăng ký gói tập và ghi nhận lượt ra vào đến đặt lịch huấn luyện viên, bán hàng và báo cáo.

## Tải Console và Winver

[Tải Fitness CRM cho Windows 64-bit (Console và Winver)](./FitnessCRM-Windows-x64.zip)

Giải nén toàn bộ tệp ZIP, giữ `sample_data` cùng cấp với hai tệp `.exe`. Mở một trong hai bản sau:

- **Console** — mở `FitnessCRMConsole.exe` để vào menu dòng lệnh.
- **Winver** — mở `FitnessCRM.exe` và đăng nhập bằng tài khoản mẫu `admin` / `admin123`.

Hai bản dùng chung dữ liệu CSV trong thư mục `data`. Chạy từng bản một để tránh ghi đè dữ liệu.

## Chức năng và cách sử dụng Console

Nhập số rồi nhấn **Enter** để chọn mục. Nhập `0` trong menu con để quay lại hoặc nhập `0` ở menu chính để thoát.

| Mục | Chức năng |
| --- | --- |
| `1` — Quản lý hội viên | Thêm hoặc xem hội viên, đăng ký gói và áp dụng voucher, check-in, bảo lưu hoặc mở bảo lưu thẻ. |
| `2` — Huấn luyện viên và lịch tập | Thêm huấn luyện viên, đặt lịch, ghi nhận ca dạy và hoa hồng. |
| `3` — Tài chính và bán hàng | Bán hàng, ghi nhận chi phí, xem báo cáo doanh thu và chi phí. |
| `4` — Thiết bị và vệ sinh | Thêm thiết bị, ghi biên bản vệ sinh và xem thiết bị cần bảo trì. |
| `5` — Báo cáo và chăm sóc khách hàng | Xem sinh nhật, lịch sắp tới, hội viên cần gia hạn và báo cáo tài chính. |
| `6` — Lưu dữ liệu CSV | Chọn mục này để lưu thay đổi trước khi thoát. |

## Chức năng và cách sử dụng Winver

| Mục trên thanh bên | Cách sử dụng |
| --- | --- |
| Hội viên | Chọn **Thêm hội viên** để tạo hồ sơ. Chọn một hội viên rồi bấm **Đăng ký / Gia hạn** để chọn gói, áp dụng ưu đãi và ghi nhận thanh toán. Tại đây cũng có thể bảo lưu thẻ và xem lịch sử tập. |
| Check-in / Ra vào | Chọn **Quét thẻ (vào / ra)**, nhập mã thẻ hoặc mã hội viên và xác nhận. Dùng cùng thao tác khi hội viên ra về. |
| Gói tập & KM | Xem hoặc tạo gói tập, quản lý chương trình khuyến mãi và tra lịch sử đăng ký. |
| Lịch PT | Chọn **Đặt lịch PT**, chọn hội viên, huấn luyện viên và ngày tập, rồi chọn một khung giờ trống. Cập nhật trạng thái lịch sau buổi tập. |
| Bán hàng (POS) và Kho hàng | Chọn sản phẩm và số lượng, thêm vào giỏ rồi thanh toán để lập hóa đơn. Vào **Kho hàng** để nhập hàng hoặc kiểm kê. |
| Thu chi & công nợ | Ghi nhận khoản thu, chi và thanh toán công nợ. |
| Nhân sự, Chấm công & lương | Quản lý hồ sơ nhân viên, ghi giờ vào hoặc ra ca và xem bảng lương tháng theo quyền tài khoản. |
| Thiết bị & vệ sinh, Tủ đồ | Theo dõi bảo trì, ghi biên bản vệ sinh và quản lý tình trạng tủ. |
| Chăm sóc KH và Báo cáo | Tra hội viên sắp hết hạn hoặc có sinh nhật. Chọn loại báo cáo để xem doanh thu, chi phí hoặc đăng ký gói. |
| Tài khoản & cài đặt | Quản trị viên tạo tài khoản nhân viên và gán vai trò. Các mục trên thanh bên hiển thị theo quyền đăng nhập. |

Nhấn **Ctrl+S** để lưu. Dữ liệu làm việc nằm trong thư mục `data` cạnh `FitnessCRM.exe`. Có thể đổi mật khẩu tại **Tệp → Đổi mật khẩu**.
