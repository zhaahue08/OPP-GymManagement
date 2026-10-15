# TV4 (Huệ) - InBody & Check-in (Toán tử)

## Jira
- **Task**: Xây dựng logic Check-in & Toán tử  (key: CRM-__, điền sau khi tạo)
- **Sub-task gợi ý và log work**

| Sub-task | Ước lượng | Tiêu chí hoàn thành |
|---|---|---|
| Cài `LichSuCheckIn` (`++`, `<<`, `>>`, thống kê tháng) | 3h | `tests/test_tracking.cpp` đạt, không cảnh báo `-Wall -Wextra -Wpedantic` |
| Cài `PhieuInBody` (`-`, `<`, `<<`, `>>`) và ràng buộc `InBody` | 3h | Dữ liệu sai bị từ chối đúng ngoại lệ/failbit |
| UML: use case + sequence check-in + class | 2h | 3 file `.puml` mở được, kèm ảnh render PNG |
| Báo cáo mục "Nạp chồng toán tử & ràng buộc InBody" | 2h | Biên dịch được trong Overleaf, có trích dẫn |

Trạng thái: To Do -> In Progress -> In Review -> Done. Đính kèm vào issue: log chạy test (`45/45 kiem tra dat`) và ảnh UML.

## Git
```bash
git checkout -b feat/tracking-checkin
git add include/LichSuCheckIn.h src/LichSuCheckIn.cpp include/PhieuInBody.h src/PhieuInBody.cpp \
        include/InBody.h src/InBody.cpp tests/test_tracking.cpp CMakeLists.txt
git commit -m "feat(tracking): add check-in history and InBody ticket with operators [CRM-__]"
git add docs/uml docs/report docs/jira
git commit -m "docs(tracking): add use case, sequence, class diagrams and report section [CRM-__]"
git push -u origin feat/tracking-checkin
```
(`JIRA_GIT.md` quy ước tên nhánh `<type>/CRM-<id>-<slug>`, bảng phân công ghi `feat/tracking-checkin`; nên thống nhất với nhóm.)

## Pull request - mô tả mẫu
- **Thay đổi**: thêm `LichSuCheckIn`, `PhieuInBody`; `InBody` thêm getter, `phanLoaiBMI()` và siết ràng buộc dữ liệu; thêm target `test_tracking`.
- **Cách kiểm tra**: `cmake -S . -B build && cmake --build build && build/test_tracking`.
- **Ảnh hưởng đến code người khác**: không sửa `HoiVien`, `HethongCRM`, `MenuConsole`. `InBody` giữ nguyên API cũ.
