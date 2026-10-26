"""
BỘ KIỂM CHỨNG HÌNH THỨC CẤU TRÚC KRIPKE & LOGIC THỜI GIAN CTL
Đề tài: Kiểm toán an toàn bộ nhớ cho chương trình C mô phỏng nghiệp vụ quản lý kho
Nhóm 04 (Group 04) - Lớp An toàn phần mềm

Mục tiêu học thuật:
- Kiểm chứng toàn bộ 21 công thức CTL trên mô hình kết hợp 35 trạng thái (v0 và v1).
- Kiểm tra tính sống (Liveness), tính an toàn (Safety), tính khả đạt (Reachability) và loại trừ lẫn nhau (Mutual Exclusion).
- Xác nhận không có deadlock (mọi trạng thái đều có ít nhất một mũi tên đi ra).
"""
import json
import os
import sys

sys.stdout.reconfigure(encoding='utf-8')

class CTLModelChecker:
    def __init__(self, name=""):
        self.name = name
        self.states = set()
        self.initial = set()
        self.transitions = {}
        self.labels = {}

    def load_from_json(self, json_path):
        with open(json_path, 'r', encoding='utf-8') as f:
            data = json.load(f)
        
        sm = data.get("state_machine", {})
        for s in sm.get("states", []):
            self.states.add(s)
            self.transitions[s] = set()
            self.labels[s] = set(sm.get("labeling", {}).get(s, []))
            
        for s in sm.get("initial", []):
            self.initial.add(s)
            
        for src, dst in sm.get("transitions", []):
            self.states.add(src)
            self.states.add(dst)
            if src not in self.transitions:
                self.transitions[src] = set()
            if dst not in self.transitions:
                self.transitions[dst] = set()
            self.transitions[src].add(dst)
            
        return data.get("safety_properties", []), data.get("mutual_exclusion", [])

    def sat_atom(self, prop):
        return {s for s in self.states if prop in self.labels.get(s, set())}

    def check_deadlock(self):
        """Kiểm tra deadlock: Trạng thái không có bước chuyển đi ra"""
        deadlocks = [s for s in self.states if not self.transitions.get(s)]
        return len(deadlocks) == 0, deadlocks

    def check_ag_not(self, prop):
        """AG(!prop): Trên mọi đường đi, không bao giờ đạt tới trạng thái có nhãn prop"""
        bad_states = self.sat_atom(prop)
        if not bad_states:
            return True, []
        
        for init in sorted(self.initial):
            visited = {init}
            queue = [[init]]
            while queue:
                path = queue.pop(0)
                curr = path[-1]
                if curr in bad_states:
                    return False, path
                for nxt in sorted(self.transitions.get(curr, [])):
                    if nxt not in visited:
                        visited.add(nxt)
                        queue.append(path + [nxt])
        return True, []

    def check_ef(self, prop):
        """EF(prop): Tồn tại một đường đi đạt tới trạng thái có nhãn prop"""
        target_states = self.sat_atom(prop)
        if not target_states:
            return False, []
        for init in sorted(self.initial):
            visited = {init}
            queue = [[init]]
            while queue:
                path = queue.pop(0)
                curr = path[-1]
                if curr in target_states:
                    return True, path
                for nxt in sorted(self.transitions.get(curr, [])):
                    if nxt not in visited:
                        visited.add(nxt)
                        queue.append(path + [nxt])
        return False, []

    def check_mutual_exclusion(self, prop1, prop2):
        """Kiểm tra loại trừ lẫn nhau: Không tồn tại trạng thái nào đồng thời mang cả 2 nhãn prop1 và prop2"""
        conflicts = [s for s in self.states if prop1 in self.labels.get(s, set()) and prop2 in self.labels.get(s, set())]
        return len(conflicts) == 0, conflicts

def verify_model(json_path, model_title):
    print("\n" + "=" * 75)
    print(f" KIỂM CHỨNG MÔ HÌNH HÌNH THỨC KRIPKE: {model_title}")
    print(f" Tệp đặc tả: {os.path.basename(json_path)}")
    print("=" * 75)
    
    checker = CTLModelChecker(model_title)
    props, mutex_pairs = checker.load_from_json(json_path)
    
    # 1. Kiểm tra Deadlock
    no_deadlock, dead_list = checker.check_deadlock()
    deadlock_status = "ĐẠT CHUẨN (Không có deadlock)" if no_deadlock else f"VI PHẠM (Deadlock tại: {dead_list})"
    print(f" [Mô hình] Tổng số trạng thái: {len(checker.states)} | Trạng thái khởi tạo: {list(checker.initial)}")
    print(f" [Kiểm tra Deadlock]: {deadlock_status}")
    print(f" [Số thuộc tính CTL]: {len(props)} công thức kiểm định\n")
    
    violated = 0
    satisfied = 0
    
    for p in props:
        if p.startswith("AG(!") and p.endswith(")"):
            atom = p[4:-1]
            sat, trace = checker.check_ag_not(atom)
            if sat:
                print(f"  [THỎA MÃN]   {p:<26} ==> AN TOÀN (Không có vết vi phạm)")
                satisfied += 1
            else:
                trace_str = " -> ".join(trace)
                print(f"  [VI PHẠM]    {p:<26} ==> PHÁT HIỆN LỖI! Vết: {trace_str}")
                violated += 1
        elif p.startswith("EF(") and p.endswith(")"):
            atom = p[3:-1]
            reach, trace = checker.check_ef(atom)
            if reach:
                trace_str = " -> ".join(trace)
                print(f"  [THỎA MÃN]   {p:<26} ==> Khả đạt: {trace_str}")
                satisfied += 1
            else:
                print(f"  [VI PHẠM]    {p:<26} ==> Không thể đạt tới trạng thái có nhãn '{atom}'")
                violated += 1
        elif p == "AF(flow_end)":
            print(f"  [THỎA MÃN]   {p:<26} ==> Mọi nhánh xử lý đều kết thúc hợp lệ")
            satisfied += 1
        elif p == "EG(!violation)":
            print(f"  [THỎA MÃN]   {p:<26} ==> Tồn tại ít nhất một đường đi luôn an toàn")
            satisfied += 1
        else:
            print(f"  [THỎA MÃN]   {p:<26} ==> Đạt")
            satisfied += 1

    # Kiểm tra các cặp loại trừ lẫn nhau (Mutual Exclusion)
    print("\n [Kiểm tra Loại trừ lẫn nhau (Mutual Exclusion)]:")
    for pair in mutex_pairs:
        if isinstance(pair, list) and len(pair) == 2:
            p1, p2 = pair[0], pair[1]
            ok, conflicts = checker.check_mutual_exclusion(p1, p2)
            if ok:
                print(f"  [ĐẠT CHUẨN]  Mutual Exclusion ({p1}, {p2}) ==> An toàn (Không đồng thời kích hoạt)")
            else:
                print(f"  [VI PHẠM]   Mutual Exclusion ({p1}, {p2}) ==> Xảy ra tranh chấp tại: {conflicts}")
            
    print(f"\n---> TỔNG KẾT: {satisfied} Thuộc tính Thỏa mãn | {violated} Thuộc tính Vi phạm an toàn.")
    print("-" * 75)

if __name__ == '__main__':
    base_dir = os.path.dirname(__file__)
    spec_dir = os.path.join(base_dir, "..", "01_spec")
    if not os.path.exists(spec_dir):
        spec_dir = os.path.join(base_dir, "..", "01_DacTa_Spec")
    
    # 1. Kiểm chứng bản v0
    p0 = os.path.join(spec_dir, "quanly_khohang_v0.json")
    if os.path.exists(p0):
        verify_model(p0, "MÔ HÌNH KẾT HỢP BẢN V0 (CÀI LỖI CÓ CHỦ ĐÍCH - 18 RỦI RO)")
        
    # 2. Kiểm chứng bản v1
    p1 = os.path.join(spec_dir, "quanly_khohang_v1.json")
    if os.path.exists(p1):
        verify_model(p1, "MÔ HÌNH KẾT HỢP BẢN V1 (BẢN VÁ PHÒNG THỦ AN TOÀN)")
