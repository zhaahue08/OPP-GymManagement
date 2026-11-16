@echo off
rem Build Fitness CRM (Win32) bang MinGW-w64 (MSYS2). Chay tu thu muc winversion.
setlocal
cd /d "%~dp0"
if not exist build mkdir build

echo [1/3] Bien dich tai nguyen (icon, manifest)...
windres -I res res\app.rc -O coff -o build\app_res.o || goto :fail

echo [2/3] Bien dich ma nguon...
set CORE=..\src\BaoCao.cpp ..\src\BanGhi.cpp ..\src\CheckIn.cpp ..\src\DateUtils.cpp ..\src\DichVuCSKH.cpp ..\src\DongHang.cpp ..\src\GiaoDich.cpp ..\src\GoiDichVu.cpp ^
 ..\src\HethongCRM.cpp ..\src\HoaDon.cpp ..\src\HoiVien.cpp ..\src\InBody.cpp ..\src\KhuyenMai.cpp ..\src\KiemTraVeSinh.cpp ..\src\LichHen.cpp ..\src\LuuTru.cpp ^
 ..\src\Nguoi.cpp ..\src\NhanVien.cpp ..\src\PT.cpp ..\src\SanPham.cpp ..\src\TaiKhoan.cpp ..\src\ThietBi.cpp ..\src\TrangThaiThe.cpp ..\src\TuDo.cpp ..\src\Voucher.cpp
set GUI=src\Main.cpp src\Controls.cpp src\PagesBaoCao.cpp src\PagesHoiVien.cpp src\PagesDichVu.cpp src\PagesNhanSu.cpp src\PagesTaiChinh.cpp src\PagesVanHanh.cpp src\PagesHeThong.cpp

echo [3/3] Lien ket...
g++ -std=c++20 -O2 -Wall -Wextra -Wno-missing-field-initializers -municode -mwindows -I ..\include -I src %CORE% %GUI% build\app_res.o ^
 -o FitnessCRM.exe -static -lcomctl32 -lcomdlg32 -lgdi32 -luser32 -luxtheme -lshell32 || goto :fail

echo.
echo Xong! Chay: FitnessCRM.exe   (tai khoan mac dinh: admin / admin123)
exit /b 0
:fail
echo.
echo Build that bai.
exit /b 1
