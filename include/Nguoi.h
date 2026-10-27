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
