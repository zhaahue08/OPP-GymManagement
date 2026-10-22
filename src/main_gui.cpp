#define NOMINMAX
#include <windows.h>
#include <windowsx.h>
#include "HethongCRM.h"
#include <filesystem>
#include <memory>
#include <string>

namespace {
crm::HethongCRM g_crm;
std::filesystem::path g_dataDir;
bool g_dataLoaded{};
constexpr COLORREF bg=RGB(13,18,30), panel=RGB(23,31,48), accent=RGB(101,232,183), text=RGB(236,242,250), muted=RGB(152,166,188);
constexpr int idAddMember=101,idCheckIn=102;
HWND addMemberButton{},checkInButton{};
HFONT font(int size,bool bold=false){return CreateFontW(size,0,0,0,bold?FW_BOLD:FW_NORMAL,FALSE,FALSE,FALSE,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,CLEARTYPE_QUALITY,DEFAULT_PITCH,L"Segoe UI");}
void textOut(HDC dc,int x,int y,const std::wstring& s,COLORREF color,int size=18,bool bold=false){SetBkMode(dc,TRANSPARENT);SetTextColor(dc,color);HFONT f=font(size,bold);auto old=SelectObject(dc,f);TextOutW(dc,x,y,s.c_str(),static_cast<int>(s.size()));SelectObject(dc,old);DeleteObject(f);}
void rounded(HDC dc,RECT r,COLORREF color,int radius=16){HBRUSH b=CreateSolidBrush(color);auto old=SelectObject(dc,b);RoundRect(dc,r.left,r.top,r.right,r.bottom,radius,radius);SelectObject(dc,old);DeleteObject(b);}
std::wstring widen(const std::string& value){if(value.empty())return {};const int size=MultiByteToWideChar(CP_UTF8,0,value.data(),static_cast<int>(value.size()),nullptr,0);if(size<=0)return std::wstring(value.begin(),value.end());std::wstring result(static_cast<std::size_t>(size),L'\0');MultiByteToWideChar(CP_UTF8,0,value.data(),static_cast<int>(value.size()),result.data(),size);return result;}
std::filesystem::path locateDataDirectory(){wchar_t exe[MAX_PATH]{};const DWORD n=GetModuleFileNameW(nullptr,exe,MAX_PATH);if(n>0&&n<MAX_PATH){auto dir=std::filesystem::path(exe).parent_path();if(std::filesystem::exists(dir/L"data"))return dir/L"data";if(std::filesystem::exists(dir.parent_path()/L"data"))return dir.parent_path()/L"data";auto root=dir;if(dir.filename()==L"build")root=dir.parent_path();else if(dir.parent_path().filename()==L"build")root=dir.parent_path().parent_path();return root/L"data";}return std::filesystem::current_path()/L"data";}
void positionButtons(HWND hwnd){RECT r{};GetClientRect(hwnd,&r);if(addMemberButton)MoveWindow(addMemberButton,266,392,r.right-318,48,TRUE);if(checkInButton)MoveWindow(checkInButton,266,455,r.right-318,48,TRUE);}
void draw(HWND hwnd,HDC dc){RECT client;GetClientRect(hwnd,&client);HBRUSH b=CreateSolidBrush(bg);FillRect(dc,&client,b);DeleteObject(b);
 textOut(dc,34,26,L"FITNESS CRM",accent,28,true);textOut(dc,35,64,L"QUẢN LÝ HỘI VIÊN & HUẤN LUYỆN",muted,11,true);
 rounded(dc,{28,104,208,client.bottom-24},panel);textOut(dc,52,132,L"KHÔNG GIAN",muted,12,true);textOut(dc,52,177,L"Tổng quan",text,17,true);textOut(dc,52,225,L"Hội viên",muted,16);textOut(dc,52,267,L"Huấn luyện viên",muted,16);textOut(dc,52,309,L"Lịch tập",muted,16);textOut(dc,52,351,L"Check-in",muted,16);textOut(dc,52,393,L"Tài chính",muted,16);
 textOut(dc,242,115,L"Xin chào, Admin",text,28,true);textOut(dc,243,153,L"Dữ liệu trung tâm được cập nhật trực tiếp.",muted,15);
 int count=static_cast<int>(g_crm.hoiViens().size()), coaches=static_cast<int>(g_crm.pts().size()), visits=0;const auto today=crm::formatDate(crm::today());for(const auto& [id,h]:g_crm.hoiViens()){(void)id;for(const auto& date:h->lichSuCheckIn())if(date==today)++visits;}
 const wchar_t* labels[]={L"HỘI VIÊN",L"HUẤN LUYỆN VIÊN",L"LƯỢT CHECK-IN"};const int values[]={count,coaches,visits};int x=242;
 for(int i=0;i<3;++i){rounded(dc,{x,196,x+220,310},panel);textOut(dc,x+20,218,labels[i],muted,11,true);textOut(dc,x+20,252,std::to_wstring(values[i]),text,32,true);textOut(dc,x+20,286,i==0?L"Hồ sơ hội viên" : i==1?L"Đội ngũ huấn luyện":L"Hoạt động hôm nay",muted,13);x+=238;}
 rounded(dc,{242,332,client.right-28,client.bottom-152},panel);textOut(dc,266,352,L"Thao tác nhanh",text,18,true);
 textOut(dc,244,client.bottom-116,L"HỘI VIÊN GẦN ĐÂY",muted,12,true);int yy=client.bottom-86;for(const auto& [id,h]:g_crm.hoiViens()){textOut(dc,244,yy,widen(id+"   "+h->ten()+"   |   "+h->trangThai()),text,14);yy+=22;if(yy>client.bottom-30)break;}
}
void popupError(HWND hwnd,const std::exception& e){auto s=widen(e.what());MessageBoxW(hwnd,s.c_str(),L"Fitness CRM",MB_ICONWARNING|MB_OK);}
void addSampleMember(HWND hwnd){try{int next=1;std::string code;do{code="HV";code+=(next<10?"00":next<100?"0":"");code+=std::to_string(next++);}while(g_crm.hoiViens().contains(code));g_crm.themHoiVien(std::make_shared<crm::HoiVien>(code,"Hoi vien moi","0900000000",crm::today(),false));InvalidateRect(hwnd,nullptr,TRUE);}catch(const std::exception& e){popupError(hwnd,e);}}
LRESULT CALLBACK windowProc(HWND hwnd,UINT msg,WPARAM wp,LPARAM lp){switch(msg){
 case WM_ERASEBKGND:return 1;
 case WM_CREATE:{try{g_dataDir=locateDataDirectory();const auto sampleDir=g_dataDir.parent_path()/L"sample_data";if(std::filesystem::exists(g_dataDir/L"hoi_vien.csv"))g_crm.docCSV(g_dataDir);else if(std::filesystem::exists(sampleDir/L"hoi_vien.csv"))g_crm.docCSV(sampleDir);g_dataLoaded=true;}catch(const std::exception& e){popupError(hwnd,e);}addMemberButton=CreateWindowExW(0,L"BUTTON",L"Thêm hội viên mẫu",WS_CHILD|WS_VISIBLE|WS_TABSTOP|BS_PUSHBUTTON,0,0,0,0,hwnd,reinterpret_cast<HMENU>(static_cast<INT_PTR>(idAddMember)),GetModuleHandleW(nullptr),nullptr);checkInButton=CreateWindowExW(0,L"BUTTON",L"Check-in hội viên HV001",WS_CHILD|WS_VISIBLE|WS_TABSTOP|BS_PUSHBUTTON,0,0,0,0,hwnd,reinterpret_cast<HMENU>(static_cast<INT_PTR>(idCheckIn)),GetModuleHandleW(nullptr),nullptr);positionButtons(hwnd);return 0;}
 case WM_SIZE:positionButtons(hwnd);return 0;
 case WM_COMMAND:if(HIWORD(wp)==BN_CLICKED){try{if(LOWORD(wp)==idAddMember)addSampleMember(hwnd);else if(LOWORD(wp)==idCheckIn){g_crm.checkIn("HV001");MessageBoxW(hwnd,L"Check-in thành công. Buổi tập đã được cập nhật.",L"Fitness CRM",MB_OK|MB_ICONINFORMATION);InvalidateRect(hwnd,nullptr,TRUE);}}catch(const std::exception& e){popupError(hwnd,e);}return 0;}break;
 case WM_PAINT:{PAINTSTRUCT ps;HDC dc=BeginPaint(hwnd,&ps);draw(hwnd,dc);EndPaint(hwnd,&ps);return 0;}
 case WM_DESTROY:try{if(g_dataLoaded&&!g_dataDir.empty())g_crm.luuCSV(g_dataDir);}catch(const std::exception& e){popupError(hwnd,e);}PostQuitMessage(0);return 0;
 }return DefWindowProcW(hwnd,msg,wp,lp);}
}
int WINAPI WinMain(HINSTANCE instance,HINSTANCE,LPSTR,int show){const wchar_t cls[]=L"FitnessCRMWindow";WNDCLASSW wc{};wc.lpfnWndProc=windowProc;wc.hInstance=instance;wc.lpszClassName=cls;wc.hCursor=LoadCursor(nullptr,IDC_ARROW);wc.hbrBackground=reinterpret_cast<HBRUSH>(GetStockObject(BLACK_BRUSH));RegisterClassW(&wc);HWND hwnd=CreateWindowExW(0,cls,L"Fitness CRM | Dashboard",WS_OVERLAPPEDWINDOW|WS_VISIBLE,CW_USEDEFAULT,CW_USEDEFAULT,1060,720,nullptr,nullptr,instance,nullptr);if(!hwnd)return 1;ShowWindow(hwnd,show);UpdateWindow(hwnd);MSG msg{};while(GetMessageW(&msg,nullptr,0,0)>0){TranslateMessage(&msg);DispatchMessageW(&msg);}return static_cast<int>(msg.wParam);}
