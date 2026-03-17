# Team Workflow and Git Convention

## 1. Mục tiêu của workflow này

Workflow này giúp nhóm làm việc theo cách an toàn, rõ ràng và dễ review:

- Không code trực tiếp vào `main`
- Không dùng branch cố định theo tên người để làm việc hằng ngày
- Mỗi công việc có một branch riêng
- Mọi thay đổi quan trọng đi qua Pull Request
- Code phải build được trước khi merge

---

## 2. Các branch chính trong repository

### `main`
Branch ổn định nhất, dùng cho:
- bản chuẩn bị nộp
- bản demo
- release chính thức

Quy tắc:
- không push trực tiếp
- chỉ merge bằng Pull Request
- phải pass CI trước khi merge

### `develop`
Branch tích hợp chung của cả nhóm.

Quy tắc:
- mọi feature hoàn thành sẽ merge vào `develop`
- đây là branch cả nhóm đồng bộ thường xuyên
- nên tạo branch mới từ `develop`

### `feature/*`
Branch cho từng chức năng hoặc task cụ thể.

Ví dụ:
- `feature/linked-list`
- `feature/hash-table-search`
- `feature/avl-rotation`
- `feature/graph-dijkstra`
- `feature/ui-step-control`

### `fix/*`
Branch sửa lỗi.

Ví dụ:
- `fix/build-error`
- `fix/graph-delete-bug`

### `docs/*`
Branch cho tài liệu.

Ví dụ:
- `docs/team-workflow`
- `docs/readme-update`

---

## 3. "Mỗi task có một branch riêng" nghĩa là gì?

Điều này có nghĩa là **không dùng một branch lâu dài cho mỗi người**.

### Cách cũ
- Anh làm trên branch `anh`
- Dũng làm trên branch `dung`
- Long làm trên branch `long`
- Khoa làm trên branch `khoa`

Cách này dễ gặp vấn đề:
- branch chứa quá nhiều việc khác nhau
- khó review vì không biết mỗi commit thuộc task nào
- dễ conflict khi merge
- khó rollback nếu chỉ một task bị lỗi

### Cách đúng hơn
Mỗi khi có một việc mới, tạo **một branch mới chỉ cho việc đó**.

Ví dụ:
- bạn làm cấu trúc AVL Tree → tạo branch `feature/avl-tree`
- bạn sửa lỗi build → tạo branch `fix/build-error`
- bạn cập nhật README → tạo branch `docs/readme-update`

Như vậy:
- mỗi branch chỉ phục vụ một mục tiêu
- dễ review
- dễ kiểm tra lỗi
- merge xong có thể xóa branch luôn

### Ví dụ thực tế
Nếu Long làm AVL Tree, Long sẽ không làm trên branch `long`.
Thay vào đó Long làm như sau:
1. checkout `develop`
2. pull code mới nhất
3. tạo branch `feature/avl-tree`
4. code trên branch đó
5. push branch đó
6. tạo Pull Request vào `develop`

Lần sau Long làm Graph thì Long lại tạo branch khác:
- `feature/graph-core`

Tức là **branch gắn với công việc, không gắn với con người**.

---

## 4. Quy trình làm việc chuẩn cho mỗi thành viên

### Bước 1: cập nhật branch `develop`
```bash
git checkout develop
git pull origin develop
```

### Bước 2: tạo branch mới cho task
```bash
git checkout -b feature/ten-task
```

Ví dụ:
```bash
git checkout -b feature/hash-table
```

### Bước 3: code và commit
```bash
git add .
git commit -m "feat: add hash table core structure"
```

### Bước 4: push branch lên GitHub
```bash
git push origin feature/hash-table
```

### Bước 5: tạo Pull Request
Trên GitHub:
- base: `develop`
- compare: `feature/hash-table`

### Bước 6: chờ CI chạy
GitHub Actions sẽ tự build project.
Nếu build fail thì sửa tiếp trên chính branch đó rồi push lại.

### Bước 7: review và merge
Khi CI pass và có review, merge vào `develop`.

---

## 5. Quy ước đặt tên branch

### Feature branch
```text
feature/<ten-task>
```

Ví dụ:
- `feature/linked-list`
- `feature/hash-table-search`
- `feature/avl-tree`
- `feature/graph-visualization`

### Fix branch
```text
fix/<ten-loi>
```

Ví dụ:
- `fix/build-error`
- `fix/graph-render`
- `fix/hash-table-delete`

### Docs branch
```text
docs/<ten-tai-lieu>
```

Ví dụ:
- `docs/team-workflow`
- `docs/readme-update`

### Quy tắc đặt tên
- dùng chữ thường
- dùng dấu gạch ngang `-`
- tên ngắn nhưng rõ nghĩa
- không dùng khoảng trắng
- không đặt theo tên người

---

## 6. Quy ước commit cho nhóm

Nhóm nên thống nhất commit message theo format:

```text
<type>: <mo-ta-ngan-gon>
```

### Các type nên dùng

#### `feat`
Thêm chức năng mới

Ví dụ:
```text
feat: add singly linked list insert operation
feat: implement AVL left rotation
feat: add graph traversal visualization
```

#### `fix`
Sửa lỗi

Ví dụ:
```text
fix: correct hash table collision handling
fix: resolve build error in graph module
```

#### `docs`
Cập nhật tài liệu

Ví dụ:
```text
docs: update team workflow guide
docs: rewrite project README
```

#### `refactor`
Chỉnh lại cấu trúc code nhưng không thêm tính năng mới

Ví dụ:
```text
refactor: separate AVL logic from rendering
refactor: move utility functions to core module
```

#### `test`
Thêm hoặc sửa test

Ví dụ:
```text
test: add linked list insert test cases
```

#### `ci`
Chỉnh GitHub Actions hoặc quy trình CI

Ví dụ:
```text
ci: add github actions build workflow
ci: update cmake build step
```

#### `chore`
Việc phụ trợ, dọn dẹp, tạo cấu trúc ban đầu

Ví dụ:
```text
chore: restructure repository layout
chore: add gitignore and cmake files
```

---

## 7. Những commit message nên tránh

Không nên commit kiểu:
- `update`
- `fix bug`
- `code`
- `123`
- `done`
- `final`
- `nộp bài`

Lý do:
- không ai biết commit đó làm gì
- khó đọc git history
- khó copy commit list vào report

---

## 8. Ví dụ workflow thực tế

### Ví dụ 1: Dũng làm Hash Table
```bash
git checkout develop
git pull origin develop
git checkout -b feature/hash-table
```

Sau khi code xong:
```bash
git add .
git commit -m "feat: add hash table insert and search"
git push origin feature/hash-table
```

Sau đó tạo PR vào `develop`.

### Ví dụ 2: Khoa sửa lỗi build
```bash
git checkout develop
git pull origin develop
git checkout -b fix/build-error
```

Sau khi sửa:
```bash
git add .
git commit -m "fix: resolve cmake build issue"
git push origin fix/build-error
```

### Ví dụ 3: Anh cập nhật tài liệu
```bash
git checkout develop
git pull origin develop
git checkout -b docs/team-workflow
```

Sau khi cập nhật:
```bash
git add .
git commit -m "docs: add team workflow and git convention"
git push origin docs/team-workflow
```

---

## 9. Quy tắc làm việc nhóm nên thống nhất

- Không push trực tiếp vào `main`
- Không push trực tiếp vào `develop` nếu task chưa rõ ràng
- Mỗi task tạo một branch riêng
- Mỗi PR nên chỉ chứa một nhóm thay đổi liên quan
- Trước khi bắt đầu task mới luôn pull `develop`
- Trước khi merge phải kiểm tra CI
- Khi branch đã merge xong có thể xóa branch đó

---

## 10. Khi nào dùng `main`, khi nào dùng `develop`?

### Dùng `develop`
- làm việc hằng ngày
- tích hợp code từ các thành viên
- kiểm tra chung toàn project

### Dùng `main`
- chuẩn bị demo
- chuẩn bị release
- chuẩn bị nộp bài
- bản ổn định cuối cùng

---

## 11. Mẫu checklist cho từng thành viên

Trước khi làm:
- [ ] Đã checkout `develop`
- [ ] Đã pull code mới nhất
- [ ] Đã tạo branch mới đúng tên task

Trước khi push:
- [ ] Code build được trên máy local
- [ ] Commit message đúng format
- [ ] Không có file rác hoặc file build bị add nhầm

Trước khi merge:
- [ ] Đã tạo PR vào `develop`
- [ ] GitHub Actions pass
- [ ] Có ít nhất 1 người review nếu nhóm yêu cầu

---

## 12. Đề xuất cho nhóm hiện tại

Với repo hiện tại, nhóm nên dùng như sau:

### Branch chính
- `main`
- `develop`

### Không dùng nữa
- `anh`
- `dung`
- `long`
- `khoa`

Có thể giữ tạm các branch này một thời gian, nhưng không dùng làm workflow chính.

### Từ giờ mỗi người tạo branch theo task
Ví dụ:
- `feature/linked-list`
- `feature/hash-table`
- `feature/avl-tree`
- `feature/graph`
- `docs/team-workflow`

---

## 13. Gợi ý phân chia task ban đầu

Ví dụ:
- Thành viên 1: `feature/linked-list`
- Thành viên 2: `feature/hash-table`
- Thành viên 3: `feature/avl-tree`
- Thành viên 4: `feature/graph`
- Người phụ trách repo/CI có thể thêm:
  - `ci/setup-build`
  - `docs/team-workflow`

---

## 14. Tóm tắt ngắn gọn

### Điều quan trọng nhất
- branch theo task, không theo người
- commit message phải rõ ràng
- mọi thay đổi quan trọng nên qua Pull Request
- CI phải pass trước khi merge

### Công thức nhớ nhanh
- `main` = bản ổn định
- `develop` = nơi tích hợp chung
- `feature/*` = làm chức năng
- `fix/*` = sửa lỗi
- `docs/*` = tài liệu
