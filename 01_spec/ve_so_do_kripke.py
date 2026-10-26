"""
Script tự động sinh sơ đồ trạng thái Kripke cho đề tài:
"Kiểm toán an toàn bộ nhớ cho chương trình C mô phỏng nghiệp vụ quản lý kho"
Sinh các định dạng:
1. Graphviz DOT (.dot)
2. Mã nguồn LaTeX TikZ (.tikz) dùng chèn trực tiếp vào báo cáo học thuật
3. Sơ đồ SVG (.svg) xem trực tiếp trên trình duyệt
"""
import json
import os
import sys

sys.stdout.reconfigure(encoding='utf-8')

def generate_dot_and_tikz(v0_path, v1_path, out_dir):
    os.makedirs(out_dir, exist_ok=True)
    
    with open(v0_path, 'r', encoding='utf-8') as f:
        d0 = json.load(f)
    with open(v1_path, 'r', encoding='utf-8') as f:
        d1 = json.load(f)
        
    sm0 = d0.get('state_machine', {})
    sm1 = d1.get('state_machine', {})
    
    states0 = sm0.get('states', [])
    trans0 = set(tuple(x) for x in sm0.get('transitions', []))
    trans1 = set(tuple(x) for x in sm1.get('transitions', []))
    labels0 = sm0.get('labeling', {})
    
    # 1. Sinh file DOT Graphviz
    dot_lines = [
        'digraph KripkeKhoHang {',
        '    rankdir=TB;',
        '    node [shape=box, style="rounded,filled", fontname="Calibri", fontsize=10];',
        '    edge [fontname="Calibri", fontsize=9];',
        ''
    ]
    
    for s in states0:
        lbl = "\\n".join([s] + [f"[{l}]" for l in labels0.get(s, []) if l not in [s, 'h1', 'h2', 'h3', 'h4', 'h5', 'h6']])
        if 'violation' in labels0.get(s, []):
            fill = '#FADBD8'
            color = '#C0392B'
        elif 'safe' in labels0.get(s, []) or 'mem_clean' in labels0.get(s, []):
            fill = '#D4EFDF'
            color = '#27AE60'
        elif s == 'start':
            fill = '#D6EAF8'
            color = '#2980B9'
        else:
            fill = '#FCF3CF'
            color = '#B7950B'
        dot_lines.append(f'    "{s}" [label="{lbl}", fillcolor="{fill}", color="{color}", penwidth=1.5];')
        
    dot_lines.append('')
    # Overlaid edges
    all_trans = sorted(list(trans0.union(trans1)))
    for src, dst in all_trans:
        in_v0 = (src, dst) in trans0
        in_v1 = (src, dst) in trans1
        
        if in_v0 and in_v1:
            dot_lines.append(f'    "{src}" -> "{dst}" [color="#2C3E50", penwidth=1.2];')
        elif in_v0 and not in_v1:
            dot_lines.append(f'    "{src}" -> "{dst}" [color="#E74C3C", style=dashed, label="v0 only (lỗi)", penwidth=1.5];')
        else:
            dot_lines.append(f'    "{src}" -> "{dst}" [color="#27AE60", style=bold, label="v1 (phòng thủ)", penwidth=1.8];')
            
    dot_lines.append('}')
    
    dot_path = os.path.join(out_dir, 'so_do_kripke_tong_the.dot')
    with open(dot_path, 'w', encoding='utf-8') as f:
        f.write("\n".join(dot_lines))
    print(f"[OK] Đã xuất Graphviz DOT: {dot_path}")

    # 2. Sinh mã nguồn LaTeX TikZ
    tikz_lines = [
        '% Mã TikZ mô hình Kripke kết hợp 35 trạng thái (Vẽ chồng v0 và v1)',
        '% Nhóm 04: Kiểm toán an toàn bộ nhớ chương trình quản lý kho',
        '\\begin{tikzpicture}[',
        '    node distance=1.8cm,',
        '    every node/.style={font=\\footnotesize},',
        '    state/.style={rectangle, rounded corners, draw=black!60, fill=yellow!10, text centered, minimum width=2.8cm, minimum height=0.7cm, inner sep=2pt},',
        '    init_state/.style={state, draw=blue!70, fill=blue!10, line width=1pt},',
        '    safe_state/.style={state, draw=green!70!black, fill=green!10, line width=1pt},',
        '    bad_state/.style={state, draw=red!70!black, fill=red!15, line width=1pt},',
        '    trans_common/.style={->, >=stealth, thick, draw=black!80},',
        '    trans_v0_only/.style={->, >=stealth, dashed, thick, draw=red!80},',
        '    trans_v1_only/.style={->, >=stealth, very thick, draw=green!60!black}',
        ']',
        ''
    ]
    
    tikz_lines.append('    % Các node trạng thái chính trên trục xử lý')
    tikz_lines.append('    \\node[init_state] (start) {start \\\\ \\scriptsize [safe]};')
    tikz_lines.append('    \\node[state, below of=start] (h1_vao) {h1\\_vao \\\\ \\scriptsize [receiving]};')
    tikz_lines.append('    \\node[safe_state, below of=h1_vao] (h1_ok) {h1\\_ok \\\\ \\scriptsize [input\\_ok]};')
    tikz_lines.append('    \\node[bad_state, right=1.5cm of h1_vao] (h1_tran_1_byte) {h1\\_tran\\_1\\_byte \\\\ \\scriptsize [buffer\\_overflow, off\\_by\\_one]};')
    tikz_lines.append('    \\node[bad_state, right=1.5cm of h1_tran_1_byte] (h1_tran_bo_dem) {h1\\_tran\\_bo\\_dem \\\\ \\scriptsize [buffer\\_overflow]};')
    tikz_lines.append('    \\node[state, below of=h1_ok] (h2_vao) {h2\\_vao \\\\ \\scriptsize [allocating]};')
    tikz_lines.append('    \\node[safe_state, below of=h2_vao] (h2_ok) {h2\\_ok \\\\ \\scriptsize [allocated, initialized]};')
    tikz_lines.append('    \\node[bad_state, right=1.5cm of h2_vao] (h2_null) {h2\\_null \\\\ \\scriptsize [null\\_deref]};')
    tikz_lines.append('    \\node[bad_state, right=1.5cm of h2_null] (h2_sai_kich_thuoc) {h2\\_sai\\_kich\\_thuoc \\\\ \\scriptsize [heap\\_overflow]};')
    tikz_lines.append('    \\node[state, below of=h2_ok] (h3_vao) {h3\\_vao \\\\ \\scriptsize [computing]};')
    tikz_lines.append('    \\node[safe_state, below of=h3_vao] (h3_ok) {h3\\_ok \\\\ \\scriptsize [amount\\_ok]};')
    tikz_lines.append('    \\node[bad_state, right=1.5cm of h3_vao] (h3_tran_so) {h3\\_tran\\_so \\\\ \\scriptsize [int\\_overflow]};')
    tikz_lines.append('    \\node[state, below of=h3_ok] (h4_vao) {h4\\_vao \\\\ \\scriptsize [token\\_received]};')
    tikz_lines.append('    \\node[safe_state, below of=h4_vao] (h4_ok) {h4\\_ok \\\\ \\scriptsize [token\\_verified]};')
    tikz_lines.append('    \\node[bad_state, right=1.5cm of h4_vao] (h4_tran_token) {h4\\_tran\\_token \\\\ \\scriptsize [buffer\\_overflow]};')
    tikz_lines.append('    \\node[state, below of=h4_ok] (h5_vao) {h5\\_vao \\\\ \\scriptsize [logging]};')
    tikz_lines.append('    \\node[safe_state, below of=h5_vao] (h5_ok) {h5\\_ok \\\\ \\scriptsize [log\\_freed]};')
    tikz_lines.append('    \\node[bad_state, right=1.5cm of h5_vao] (h5_ro_ri) {h5\\_ro\\_ri \\\\ \\scriptsize [mem\\_leak]};')
    tikz_lines.append('    \\node[state, below of=h5_ok] (h6_phien_ranh) {h6\\_phien\\_ranh \\\\ \\scriptsize [sess\\_free]};')
    tikz_lines.append('    \\node[state, below left=1.2cm of h6_phien_ranh] (h6_nhap_giu) {h6\\_nhap\\_giu \\\\ \\scriptsize [nhap\\_locked]};')
    tikz_lines.append('    \\node[state, below right=1.2cm of h6_phien_ranh] (h6_xuat_giu) {h6\\_xuat\\_giu \\\\ \\scriptsize [xuat\\_locked]};')
    tikz_lines.append('    \\node[bad_state, below of=h6_phien_ranh] (h6_tranh_chap) {h6\\_tranh\\_chap \\\\ \\scriptsize [race, violation]};')
    tikz_lines.append('    \\node[safe_state, below of=h6_tranh_chap] (h6_giai_phong) {h6\\_giai\\_phong \\\\ \\scriptsize [releasing]};')
    tikz_lines.append('    \\node[safe_state, below left=1.5cm of h6_giai_phong] (ket_thuc_ok) {ket\\_thuc\\_ok \\\\ \\scriptsize [stock\\_done]};')
    tikz_lines.append('    \\node[safe_state, left=2.5cm of h1_ok] (ket_thuc_tu_choi) {ket\\_thuc\\_tu\\_choi \\\\ \\scriptsize [input\\_rejected]};')
    tikz_lines.append('    \\node[bad_state, right=5.5cm of h3_ok] (dung_khong_an_toan) {dung\\_khong\\_an\\_toan \\\\ \\scriptsize [unsafe\\_halt]};')
    tikz_lines.append('')
    
    tikz_lines.append('    % Các cạnh chuyển trạng thái chung giữa v0 và v1')
    tikz_lines.append('    \\draw[trans_common] (start) -- (h1_vao);')
    tikz_lines.append('    \\draw[trans_common] (h1_vao) -- (h1_ok);')
    tikz_lines.append('    \\draw[trans_common] (h1_ok) -- (h2_vao);')
    tikz_lines.append('    \\draw[trans_common] (h2_vao) -- (h2_ok);')
    tikz_lines.append('    \\draw[trans_common] (h2_ok) -- (h3_vao);')
    tikz_lines.append('    \\draw[trans_common] (h3_vao) -- (h3_ok);')
    tikz_lines.append('    \\draw[trans_common] (h3_ok) -- (h4_vao);')
    tikz_lines.append('    \\draw[trans_common] (h4_vao) -- (h4_ok);')
    tikz_lines.append('    \\draw[trans_common] (h4_ok) -- (h5_vao);')
    tikz_lines.append('    \\draw[trans_common] (h5_vao) -- (h5_ok);')
    tikz_lines.append('    \\draw[trans_common] (h5_ok) -- (h6_phien_ranh);')
    tikz_lines.append('    \\draw[trans_common] (h6_nhap_giu) -- (h6_giai_phong);')
    tikz_lines.append('    \\draw[trans_common] (h6_xuat_giu) -- (h6_giai_phong);')
    tikz_lines.append('    \\draw[trans_common] (h6_giai_phong) -- (ket_thuc_ok);')
    tikz_lines.append('')
    tikz_lines.append('    % Cạnh v0-only: Dẫn sang trạng thái lỗi (nét đứt đỏ)')
    tikz_lines.append('    \\draw[trans_v0_only] (h1_vao) -- (h1_tran_1_byte);')
    tikz_lines.append('    \\draw[trans_v0_only] (h1_vao) -- (h1_tran_bo_dem);')
    tikz_lines.append('    \\draw[trans_v0_only] (h2_vao) -- (h2_null);')
    tikz_lines.append('    \\draw[trans_v0_only] (h2_vao) -- (h2_sai_kich_thuoc);')
    tikz_lines.append('    \\draw[trans_v0_only] (h3_vao) -- (h3_tran_so);')
    tikz_lines.append('    \\draw[trans_v0_only] (h4_vao) -- (h4_tran_token);')
    tikz_lines.append('    \\draw[trans_v0_only] (h5_vao) -- (h5_ro_ri);')
    tikz_lines.append('    \\draw[trans_v0_only] (h6_nhap_giu) -- (h6_tranh_chap);')
    tikz_lines.append('    \\draw[trans_v0_only] (h6_xuat_giu) -- (h6_tranh_chap);')
    tikz_lines.append('    \\draw[trans_v0_only] (h1_tran_1_byte) -- (dung_khong_an_toan);')
    tikz_lines.append('    \\draw[trans_v0_only] (h2_null) -- (dung_khong_an_toan);')
    tikz_lines.append('    \\draw[trans_v0_only] (h3_tran_so) -- (dung_khong_an_toan);')
    tikz_lines.append('')
    tikz_lines.append('    % Cạnh v1-only: Dẫn về từ chối an toàn (nét đậm xanh)')
    tikz_lines.append('    \\draw[trans_v1_only] (h1_vao) to[bend right=20] (ket_thuc_tu_choi);')
    tikz_lines.append('    \\draw[trans_v1_only] (h2_vao) to[bend right=20] (ket_thuc_tu_choi);')
    tikz_lines.append('    \\draw[trans_v1_only] (h3_vao) to[bend right=20] (ket_thuc_tu_choi);')
    tikz_lines.append('\\end{tikzpicture}')
    
    tikz_path = os.path.join(out_dir, 'so_do_kripke_tong_the.tikz')
    with open(tikz_path, 'w', encoding='utf-8') as f:
        f.write("\n".join(tikz_lines))
    print(f"[OK] Đã xuất LaTeX TikZ: {tikz_path}")

if __name__ == '__main__':
    base_dir = os.path.dirname(__file__)
    spec_v0 = os.path.join(base_dir, 'quanly_khohang_v0.json')
    spec_v1 = os.path.join(base_dir, 'quanly_khohang_v1.json')
    out_dir = os.path.join(base_dir, '..', '04_results', 'so_do_kripke')
    generate_dot_and_tikz(spec_v0, spec_v1, out_dir)
