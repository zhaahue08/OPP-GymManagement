# Fitness CRM

[Tiếng Việt](./README.md)

Fitness CRM helps gym staff manage memberships, check members in and out, book personal training sessions, record sales, and view reports.

## Download Console and Winver

[Download Fitness CRM for 64-bit Windows (Console and Winver)](./FitnessCRM-Windows-x64.zip)

Extract the entire ZIP file. Keep `sample_data` next to both `.exe` files so the apps can load the demo data on first launch. Choose one of the two apps:

- **Console** — open `FitnessCRMConsole.exe` to use the command-line menu.
- **Winver** — open `FitnessCRM.exe` and sign in with the demo account `admin` / `admin123`.

Both apps save to the same `data` folder. Run one app at a time to avoid overwriting changes.

## Console features and instructions

The menu is displayed in Vietnamese. Enter a menu number and press **Enter**. Enter `0` to go back from a submenu or exit from the main menu.

| Menu option | What you can do |
| --- | --- |
| `1` — Member management | Add or view members, register a training plan, apply a voucher, check members in, and freeze or reactivate memberships. |
| `2` — Personal trainers and scheduling | Add trainers, book sessions, and record completed sessions and commissions. |
| `3` — Finance and sales | Sell products, record expenses, and review revenue and expenses. |
| `4` — Equipment and cleaning | Add equipment, log cleaning, and review equipment due for maintenance. |
| `5` — Reports and customer care | Check birthdays, upcoming sessions, memberships due for renewal, and financial reports. |
| `6` — Save CSV data | Select this option to save your changes before exiting. |

## Winver features and instructions

The sidebar labels below match the Vietnamese interface.

| Sidebar item | How to use it |
| --- | --- |
| Hội viên | Select **Thêm hội viên** to create a member profile. Select a member, then choose **Đăng ký / Gia hạn** to select a plan, apply discounts, and record payment. You can also freeze memberships and review training history. |
| Check-in / Ra vào | Select **Quét thẻ (vào / ra)**, enter a card or member ID, and confirm. Use the same action when the member leaves. |
| Gói tập & KM | View or create training plans, manage promotions, and review registration history. |
| Lịch PT | Select **Đặt lịch PT**, choose the member, trainer, and date, then select an available time. Update the appointment status after the session. |
| Bán hàng (POS) and Kho hàng | Add products and quantities to the cart, then check out to create an invoice. Open **Kho hàng** to receive stock or record an inventory count. |
| Thu chi & công nợ | Record income, expenses, and payments toward outstanding balances. |
| Nhân sự and Chấm công & lương | Manage staff records, clock in or out, and view monthly payroll according to your account permissions. |
| Thiết bị & vệ sinh and Tủ đồ | Track equipment maintenance, log cleaning, and manage locker availability. |
| Chăm sóc KH and Báo cáo | Find members with upcoming renewals or birthdays. Select a report to view revenue, expenses, or training plan registrations. |
| Tài khoản & cài đặt | Administrators can create staff accounts and assign roles. Sidebar items depend on your account permissions. |

Press **Ctrl+S** to save. Working data is stored in the `data` folder next to `FitnessCRM.exe`. To change your password, open **Tệp → Đổi mật khẩu**.
