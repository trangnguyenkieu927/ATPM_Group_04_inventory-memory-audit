"""
Tạo sơ đồ trạng thái Kripke cho riêng Module M2 (Heap Lifecycle)
Bao gồm:
- Graphviz DOT
- LaTeX TikZ
- Mermaid code
"""
import os
import sys

sys.stdout.reconfigure(encoding='utf-8')

def generate_m2_diagrams():
    out_dir = os.path.join(os.path.dirname(__file__), "..", "04_results", "so_do_kripke")
    os.makedirs(out_dir, exist_ok=True)

    # 1. DOT FILE
    dot_content = """digraph M2_Heap_Lifecycle {
    rankdir=LR;
    node [shape=box, style="rounded,filled", fontname="Calibri", fontsize=11];
    edge [fontname="Calibri", fontsize=10];

    // Trạng thái ban đầu
    "m2_khoi_tao" [label="m2_khoi_tao\\n[initialized]", fillcolor="#D6EAF8", color="#2980B9", penwidth=2.0];

    // Trạng thái chung
    "m2_danh_sach_day" [label="m2_danh_sach_day\\n[list_full]", fillcolor="#FCF3CF", color="#B7950B", penwidth=1.5];

    // Nhánh v0 (Lỗi có chủ đích)
    "m2_realloc_goi" [label="m2_realloc_goi\\n[reallocating]", fillcolor="#FCF3CF", color="#B7950B", penwidth=1.5];
    "m2_oom_null" [label="m2_oom_null\\n[oom_occurred]", fillcolor="#FCF3CF", color="#B7950B", penwidth=1.5];
    "m2_mat_con_tro_leak" [label="m2_mat_con_tro_leak\\n[memory_leak, cwe_401, violation]", fillcolor="#FADBD8", color="#C0392B", penwidth=2.0];
    "m2_giai_phong_xong" [label="m2_giai_phong_xong\\n[freed_without_null]", fillcolor="#FCF3CF", color="#B7950B", penwidth=1.5];
    "m2_doc_con_tro_treo_uaf" [label="m2_doc_con_tro_treo_uaf\\n[use_after_free, cwe_416, violation]", fillcolor="#FADBD8", color="#C0392B", penwidth=2.0];

    // Nhánh v1 (Bản vá an toàn)
    "m2_realloc_tam" [label="m2_realloc_tam\\n[realloc_temp]", fillcolor="#D4EFDF", color="#27AE60", penwidth=1.5];
    "m2_oom_bao_toan_ram" [label="m2_oom_bao_toan_ram\\n[owner_preserved, safe_exit, done]", fillcolor="#D4EFDF", color="#27AE60", penwidth=2.0];
    "m2_realloc_thanh_cong" [label="m2_realloc_thanh_cong\\n[realloc_ok]", fillcolor="#D4EFDF", color="#27AE60", penwidth=1.5];
    "m2_giai_phong_an_toan" [label="m2_giai_phong_an_toan\\n[pointer_nullified, zero_leak, done]", fillcolor="#D4EFDF", color="#27AE60", penwidth=2.0];

    // Chuyển trạng thái
    "m2_khoi_tao" -> "m2_danh_sach_day" [label="them 16 san pham"];
    
    // v0 Transitions (Màu đỏ nét đứt khi dẫn tới lỗi)
    "m2_danh_sach_day" -> "m2_realloc_goi" [label="v0: goi realloc truc tiep", color="#C0392B", style=dashed];
    "m2_realloc_goi" -> "m2_oom_null" [label="he thong het RAM (NULL)", color="#C0392B", style=dashed];
    "m2_oom_null" -> "m2_mat_con_tro_leak" [label="ghi de mat con tro goc", color="#C0392B", style=dashed, penwidth=2.0];
    "m2_mat_con_tro_leak" -> "m2_mat_con_tro_leak" [label="loop error", color="#C0392B", style=dashed];

    "m2_realloc_goi" -> "m2_realloc_thanh_cong" [label="cap phat RAM moi thanh cong", color="#27AE60"];
    "m2_realloc_thanh_cong" -> "m2_giai_phong_xong" [label="v0: free() khong set NULL", color="#C0392B", style=dashed];
    "m2_giai_phong_xong" -> "m2_doc_con_tro_treo_uaf" [label="doc lai con tro treo", color="#C0392B", style=dashed, penwidth=2.0];
    "m2_doc_con_tro_treo_uaf" -> "m2_doc_con_tro_treo_uaf" [label="loop error", color="#C0392B", style=dashed];

    // v1 Transitions (Màu xanh nét liền an toàn)
    "m2_danh_sach_day" -> "m2_realloc_tam" [label="v1: dung con tro tam tmp", color="#27AE60", penwidth=2.0];
    "m2_realloc_tam" -> "m2_oom_bao_toan_ram" [label="tmp == NULL: giu con tro cu", color="#27AE60", penwidth=2.0];
    "m2_oom_bao_toan_ram" -> "m2_oom_bao_toan_ram" [label="safe exit", color="#27AE60"];
    "m2_realloc_tam" -> "m2_realloc_thanh_cong" [label="tmp != NULL: list = tmp", color="#27AE60", penwidth=2.0];
    "m2_realloc_thanh_cong" -> "m2_giai_phong_an_toan" [label="v1: cache val, free, ptr=NULL", color="#27AE60", penwidth=2.0];
    "m2_giai_phong_an_toan" -> "m2_giai_phong_an_toan" [label="safe exit", color="#27AE60"];
}
"""
    dot_file = os.path.join(out_dir, "so_do_m2_heap_lifecycle.dot")
    with open(dot_file, "w", encoding="utf-8") as f:
        f.write(dot_content)

    # 2. TIKZ FILE
    tikz_content = r"""% Sơ đồ Kripke chuyên sâu Module M2: Heap Lifecycle
% Sinh bởi ATPM_Group_04 - Nguyễn Kiều Trang (23010495)
\begin{tikzpicture}[
    >=stealth,
    node distance=2.2cm and 2.5cm,
    state/.style={rectangle, rounded corners, draw=black, thick, fill=yellow!15, align=center, inner sep=6pt, font=\small},
    init/.style={state, fill=blue!15, draw=blue!70, thick},
    safe/.style={state, fill=green!20, draw=green!60!black, thick},
    violation/.style={state, fill=red!20, draw=red!70!black, double, thick},
    trans/.style={->, thick},
    v0edge/.style={->, dashed, thick, draw=red!80!black},
    v1edge/.style={->, solid, very thick, draw=green!60!black}
]

% Nodes
\node[init] (init) {$\mathbf{m2\_khoi\_tao}$\\\footnotesize\{initialized\}};
\node[state, right=of init] (full) {$\mathbf{m2\_danh\_sach\_day}$\\\footnotesize\{list\_full\}};

% v0 branch
\node[state, above right=1.2cm and 1.8cm of full] (call) {$\mathbf{m2\_realloc\_goi}$\\\footnotesize\{reallocating\}};
\node[state, right=1.8cm of call] (oom) {$\mathbf{m2\_oom\_null}$\\\footnotesize\{oom\_occurred\}};
\node[violation, right=1.8cm of oom] (leak) {$\mathbf{m2\_mat\_con\_tro\_leak}$\\\footnotesize\{memory\_leak, cwe\_401, violation\}};

\node[state, below right=1.2cm and 1.8cm of call] (freed_bad) {$\mathbf{m2\_giai\_phong\_xong}$\\\footnotesize\{freed\_without\_null\}};
\node[violation, right=1.8cm of freed_bad] (uaf) {$\mathbf{m2\_doc\_con\_tro\_treo\_uaf}$\\\footnotesize\{use\_after\_free, cwe\_416, violation\}};

% v1 branch
\node[safe, below right=1.2cm and 1.8cm of full] (tmp) {$\mathbf{m2\_realloc\_tam}$\\\footnotesize\{realloc\_temp\}};
\node[safe, below=1.5cm of tmp] (oom_ok) {$\mathbf{m2\_oom\_bao\_toan\_ram}$\\\footnotesize\{owner\_preserved, safe\_exit, done\}};
\node[safe, right=2.0cm of tmp] (ok) {$\mathbf{m2\_realloc\_thanh\_cong}$\\\footnotesize\{realloc\_ok\}};
\node[safe, right=2.0cm of ok] (freed_ok) {$\mathbf{m2\_giai\_phong\_an\_toan}$\\\footnotesize\{pointer\_nullified, zero\_leak, done\}};

% Edges
\draw[trans] (init) -- node[above]{\footnotesize Nạp 16 SP} (full);

% v0 edges
\draw[v0edge] (full) |- node[above, sloped]{\footnotesize v0: Gán đè trực tiếp} (call);
\draw[v0edge] (call) -- node[above]{\footnotesize Hết RAM} (oom);
\draw[v0edge] (oom) -- node[above]{\footnotesize Gán NULL mất con trỏ} (leak);
\draw[v0edge] (leak) to[loop right] node[right]{\footnotesize Lỗi lặp} (leak);

\draw[trans] (call) -- node[above, sloped]{\footnotesize Thành công} (ok);
\draw[v0edge] (ok) -- node[above, sloped]{\footnotesize v0: free không set NULL} (freed_bad);
\draw[v0edge] (freed_bad) -- node[above]{\footnotesize Đọc con trỏ treo} (uaf);
\draw[v0edge] (uaf) to[loop right] node[right]{\footnotesize Lỗi lặp} (uaf);

% v1 edges
\draw[v1edge] (full) |- node[below, sloped]{\footnotesize v1: Con trỏ tạm tmp} (tmp);
\draw[v1edge] (tmp) -- node[left]{\footnotesize tmp == NULL (giữ ptr gốc)} (oom_ok);
\draw[v1edge] (oom_ok) to[loop below] node[below]{\footnotesize An toàn (done)} (oom_ok);
\draw[v1edge] (tmp) -- node[above]{\footnotesize tmp != NULL (list = tmp)} (ok);
\draw[v1edge] (ok) -- node[above]{\footnotesize Cache -> free -> ptr=NULL} (freed_ok);
\draw[v1edge] (freed_ok) to[loop right] node[right]{\footnotesize An toàn (done)} (freed_ok);

\end{tikzpicture}
"""
    tikz_file = os.path.join(out_dir, "so_do_m2_heap_lifecycle.tikz")
    with open(tikz_file, "w", encoding="utf-8") as f:
        f.write(tikz_content)

    print(f"Đã xuất sơ đồ DOT: {dot_file}")
    print(f"Đã xuất sơ đồ TikZ: {tikz_file}")

if __name__ == '__main__':
    generate_m2_diagrams()
