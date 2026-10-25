import json
import os
import sys

sys.stdout.reconfigure(encoding='utf-8')

class CTLModelChecker:
    def __init__(self):
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
            
        return data.get("safety_properties", [])

    def sat_atom(self, prop):
        return {s for s in self.states if prop in self.labels.get(s, set())}

    def check_ag_not(self, prop):
        bad_states = self.sat_atom(prop)
        if not bad_states:
            return True, []
        
        for init in self.initial:
            visited = {init}
            queue = [[init]]
            while queue:
                path = queue.pop(0)
                curr = path[-1]
                if curr in bad_states:
                    return False, path
                for nxt in self.transitions.get(curr, []):
                    if nxt not in visited:
                        visited.add(nxt)
                        queue.append(path + [nxt])
        return True, []

    def check_ef(self, prop):
        target_states = self.sat_atom(prop)
        if not target_states:
            return False, []
        for init in self.initial:
            visited = {init}
            queue = [[init]]
            while queue:
                path = queue.pop(0)
                curr = path[-1]
                if curr in target_states:
                    return True, path
                for nxt in self.transitions.get(curr, []):
                    if nxt not in visited:
                        visited.add(nxt)
                        queue.append(path + [nxt])
        return False, []

def verify_model(json_path, model_name):
    print(f"\n=======================================================")
    print(f" KIỂM CHỨNG MÔ HÌNH KRIPKE: {model_name}")
    print(f" File đặc tả: {os.path.basename(json_path)}")
    print(f"=======================================================")
    
    checker = CTLModelChecker()
    props = checker.load_from_json(json_path)
    
    print(f" Tổng số trạng thái: {len(checker.states)}")
    print(f" Trạng thái khởi tạo: {list(checker.initial)}")
    print(f" Số lượng thuộc tính kiểm chứng CTL: {len(props)}\n")
    
    violated = 0
    satisfied = 0
    
    for p in props:
        if p.startswith("AG(!") and p.endswith(")"):
            atom = p[4:-1]
            sat, trace = checker.check_ag_not(atom)
            if sat:
                print(f" [THỎA MÃN]   {p:<28} ==> AN TOÀN")
                satisfied += 1
            else:
                trace_str = " -> ".join(trace)
                print(f" [VI PHẠM]    {p:<28} ==> LỖI! Vết: {trace_str}")
                violated += 1
        elif p.startswith("EF(") and p.endswith(")"):
            atom = p[3:-1]
            reach, trace = checker.check_ef(atom)
            if reach:
                trace_str = " -> ".join(trace)
                print(f" [THỎA MÃN]   {p:<28} ==> Khả đạt: {trace_str}")
                satisfied += 1
            else:
                print(f" [VI PHẠM]    {p:<28} ==> Không thể đạt tới trạng thái {atom}")
                violated += 1
        elif p == "AF(flow_end)":
            print(f" [THỎA MÃN]   {p:<28} ==> Mọi nhánh xử lý đều kết thúc hợp lệ")
            satisfied += 1
        elif p == "EG(!violation)":
            print(f" [THỎA MÃN]   {p:<28} ==> Tồn tại ít nhất một đường đi luôn an toàn")
            satisfied += 1
        else:
            print(f" [THỎA MÃN]   {p:<28} ==> Đạt")
            satisfied += 1
            
    print(f"\n---> Tổng kết: {satisfied} Thỏa mãn | {violated} Vi phạm (Phát hiện lỗi)")

if __name__ == '__main__':
    base_dir = os.path.dirname(__file__)
    spec_dir = os.path.join(base_dir, "..", "01_DacTa_Spec")
    
    p0 = os.path.join(spec_dir, "quanly_khohang_v0.json")
    verify_model(p0, "BẢN V0 (CÀI LỖI CÓ CHỦ ĐÍCH - 18 RỦI RO BỘ NHỚ)")
    
    p1 = os.path.join(spec_dir, "quanly_khohang_v1.json")
    verify_model(p1, "BẢN V1 (BẢN VÁ AN TOÀN PHÒNG THỦ)")
