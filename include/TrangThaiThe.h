#pragma once
#include <memory>
#include <string>

namespace crm {
class HoiVien;
class TrangThaiThe {
public:
    virtual ~TrangThaiThe() = default;
    virtual std::string ten() const = 0;
    virtual void kichHoat(HoiVien& hv) const;
    virtual void dongBang(HoiVien& hv) const;
    virtual void moBang(HoiVien& hv) const;
    virtual void danhDauHetHan(HoiVien& hv) const;
    virtual bool duocCheckIn() const noexcept { return false; }
};
class TrangThaiMoi final : public TrangThaiThe {
public:
    std::string ten() const override;
    void kichHoat(HoiVien& hv) const override;
};
class TrangThaiKichHoat final : public TrangThaiThe {
public:
    std::string ten() const override;
    void kichHoat(HoiVien& hv) const override;
    void dongBang(HoiVien& hv) const override;
    void danhDauHetHan(HoiVien& hv) const override;
    bool duocCheckIn() const noexcept override;
};
class TrangThaiDongBang final : public TrangThaiThe {
public:
    std::string ten() const override;
    void kichHoat(HoiVien& hv) const override;
    void moBang(HoiVien& hv) const override;
    void danhDauHetHan(HoiVien& hv) const override;
};
class TrangThaiHetHan final : public TrangThaiThe {
public:
    std::string ten() const override;
    void kichHoat(HoiVien& hv) const override;
};
std::unique_ptr<TrangThaiThe> taoTrangThaiMoi();
std::unique_ptr<TrangThaiThe> taoTrangThaiKichHoat();
std::unique_ptr<TrangThaiThe> taoTrangThaiDongBang();
std::unique_ptr<TrangThaiThe> taoTrangThaiHetHan();
// Nhận cả tên có dấu lẫn tên không dấu của dữ liệu CSV cũ ("Kich hoat", "Dong bang"...).
std::unique_ptr<TrangThaiThe> taoTrangThaiTheoTen(const std::string& ten);
}
