"""
BỘ KIỂM CHỨNG HÌNH THỨC BOUNDED MODEL CHECKING (BMC) CHO CẤU TRÚC KRIPKE
Dựa trên thuật toán Bounded Checker (Giáo trình An toàn phần mềm, Phụ lục A.3.3, Listing A.11)
Đề tài: Kiểm toán an toàn bộ nhớ cho chương trình C mô phỏng nghiệp vụ quản lý kho
Nhóm 04 (Group 04) - Lớp An toàn phần mềm
"""
import json
import os
import sys
from typing import List, Set, Tuple, Dict

sys.stdout.reconfigure(encoding='utf-8')

class BMCResult:
    def __init__(self, violated: bool, trace: List[str], k: int, reached_ct: bool, total_visited: int):
        self.violated = violated
        self.trace = trace
        self.k = k
        self.reached_ct = reached_ct
        self.total_visited = total_visited

class BoundedModelChecker:
    def __init__(self, name: str):
        self.name = name
        self.states: Set[str] = set()
        self.initial_states: Set[str] = set()
        self.transitions: Dict[str, Set[str]] = {}
        self.labels: Dict[str, Set[str]] = {}

    def load_from_json(self, json_path: str):
        with open(json_path, 'r', encoding='utf-8') as f:
            data = json.load(f)
        sm = data.get("state_machine", {})
        
        for s in sm.get("states", []):
            self.states.add(s)
            self.transitions[s] = set()
            self.labels[s] = set(sm.get("labeling", {}).get(s, []))
            
        for s in sm.get("initial", []):
            self.initial_states.add(s)
            
        for src, dst in sm.get("transitions", []):
            self.states.add(src)
            self.states.add(dst)
            if src not in self.transitions:
                self.transitions[src] = set()
            self.transitions[src].add(dst)

    def successors(self, state: str) -> Set[str]:
        return self.transitions.get(state, set())

    def bmc_check(self, bad_label: str = "violation", max_k: int = 50) -> BMCResult:
        bad_states = {s for s in self.states if bad_label in self.labels.get(s, set())}
        
        for s in sorted(self.initial_states):
            if s in bad_states:
                return BMCResult(True, [s], 0, True, 1)

        visited: Set[str] = set(self.initial_states)
        frontier: List[Tuple[str, List[str]]] = [(s, [s]) for s in sorted(self.initial_states)]
        step = 0

        while frontier and step < max_k:
            step += 1
            next_frontier: List[Tuple[str, List[str]]] = []
            
            for state, path in frontier:
                for nxt in sorted(self.successors(state)):
                    if nxt in bad_states:
                        return BMCResult(
                            violated=True,
                            trace=path + [nxt],
                            k=step,
                            reached_ct=False,
                            total_visited=len(visited) + 1
                        )
                    if nxt not in visited:
                        visited.add(nxt)
                        next_frontier.append((nxt, path + [nxt]))
                        
            frontier = next_frontier
            if not frontier:
                return BMCResult(
                    violated=False,
                    trace=[],
                    k=step,
                    reached_ct=True,
                    total_visited=len(visited)
                )

        return BMCResult(
            violated=False,
            trace=[],
            k=step,
            reached_ct=False,
            total_visited=len(visited)
        )

def run_bmc_analysis(json_path: str, model_title: str):
    print("=" * 72)
    print(f" KIỂM CHỨNG HÌNH THỨC BOUNDED MODEL CHECKING (BMC): {model_title}")
    print(f" Tệp nguồn đặc tả: {os.path.basename(json_path)}")
    print("=" * 72)

    checker = BoundedModelChecker(model_title)
    checker.load_from_json(json_path)

    print(f" [1] Quy mô mô hình Kripke: |S| = {len(checker.states)} trạng thái, |S0| = {len(checker.initial_states)}")
    print(f"     Trạng thái khởi tạo: {sorted(list(checker.initial_states))}")
    
    bad_states = [s for s in checker.states if 'violation' in checker.labels.get(s, set())]
    print(f"     Số lượng trạng thái gán nhãn [violation]: {len(bad_states)}")
    if bad_states:
        print(f"     Danh sách trạng thái lỗi: {sorted(bad_states)}")

    print(f"\n [2] Tiến trình kiểm chứng thuộc tính Safety: AG(!violation) với k_max = 50:")
    result = checker.bmc_check(bad_label="violation", max_k=50)

    if result.violated:
        print(f"     ==> KẾT LUẬN: VI PHẠM (SAT - Phát hiện lỗ hổng an toàn tại bước k = {result.k})")
        print(f"     Độ dài vết phản ví dụ: {len(result.trace)} bước chuyển trạng thái")
        print(f"     Vết phản ví dụ (Counterexample Trace):")
        for idx, st in enumerate(result.trace):
            lbls = list(checker.labels.get(st, []))
            print(f"        Bước {idx}: [{st}] (nhãn: {', '.join(lbls)})")
        print(f"     Trạng thái vi phạm đích: {result.trace[-1]}")
    else:
        print(f"     ==> KẾT LUẬN: THỎA MÃN (UNSAT - Không tồn tại bất kỳ đường đi nào tới trạng thái lỗi)")
        if result.reached_ct:
            print(f"     Chứng minh tính đầy đủ: ĐẠT NGƯỠNG ĐẦY ĐỦ (Completeness Threshold - CT) tại bước k = {result.k}")
            print(f"     Toàn bộ {result.total_visited} trạng thái khả đạt từ S0 đã được duyệt đầy đủ.")
            print(f"     Kết luận an toàn đạt cấp độ chứng minh hình thức tuyệt đối (Formal Proof).")
        else:
            print(f"     Chưa đạt CT trong phạm vi k = {result.k}.")

    print("-" * 72)
    return result

if __name__ == '__main__':
    base_dir = os.path.dirname(__file__)
    spec_dir = os.path.join(base_dir, "..", "01_spec")

    p0 = os.path.join(spec_dir, "quanly_khohang_v0.json")
    if os.path.exists(p0):
        run_bmc_analysis(p0, "BẢN V0 (CÀI LỖI CÓ CHỦ ĐÍCH)")

    p1 = os.path.join(spec_dir, "quanly_khohang_v1.json")
    if os.path.exists(p1):
        run_bmc_analysis(p1, "BẢN V1 (BẢN VÁ PHÒNG THỦ AN TOÀN)")
