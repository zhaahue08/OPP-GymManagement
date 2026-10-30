#include "CheckIn.h"
#include <stdexcept>
#include <utility>
namespace crm {
CheckIn::CheckIn(std::string ma):CheckIn(std::move(ma),now(),"Thủ công"){}
CheckIn::CheckIn(std::string ma,DateTime vao,std::string cach,DateTime ra,std::string tu):maHoiVien_(std::move(ma)),vao_(vao),ra_(ra),phuongThuc_(std::move(cach)),maTu_(std::move(tu)){if(maHoiVien_.empty())throw std::invalid_argument("Mã hội viên rỗng.");if(daRa()&&ra_<vao_)throw std::invalid_argument("Giờ ra phải sau giờ vào.");if(phuongThuc_.empty())phuongThuc_="Thủ công";}
const std::string& CheckIn::maHoiVien()const noexcept{return maHoiVien_;} std::chrono::system_clock::time_point CheckIn::thoiGian()const noexcept{return vao_;}
DateTime CheckIn::vao()const noexcept{return vao_;} DateTime CheckIn::ra()const noexcept{return ra_;} bool CheckIn::daRa()const noexcept{return ra_.time_since_epoch().count()!=0;}
int CheckIn::soPhut(DateTime den)const{const auto end=daRa()?ra_:den;return end<vao_?0:static_cast<int>(std::chrono::duration_cast<std::chrono::minutes>(end-vao_).count());}
const std::string& CheckIn::phuongThuc()const noexcept{return phuongThuc_;}
const std::string& CheckIn::maTu()const noexcept{return maTu_;} void CheckIn::ganTu(std::string tu){maTu_=std::move(tu);}
void CheckIn::checkOut(DateTime luc){if(daRa())throw std::logic_error("Lượt tập này đã check-out.");if(luc<vao_)throw std::invalid_argument("Giờ ra phải sau giờ vào.");ra_=luc;}
}
