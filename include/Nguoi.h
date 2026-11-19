// Lớp cơ sở trừu tượng cho hệ phân cấp con người.
// vaiTro() thuần ảo buộc lớp con cung cấp hành vi. Destructor ảo hỗ trợ hủy đối tượng qua con trỏ cơ sở.

#pragma once
#include <string>
namespace crm {
class Nguoi {
protected:
    std::string ma_, ten_;
    Nguoi(std::string ma, std::string ten);
    void doiTen(std::string ten);
public:
    virtual ~Nguoi();
    const std::string& ma() const noexcept;
    const std::string& ten() const noexcept;
    virtual std::string vaiTro() const = 0;
};
}
