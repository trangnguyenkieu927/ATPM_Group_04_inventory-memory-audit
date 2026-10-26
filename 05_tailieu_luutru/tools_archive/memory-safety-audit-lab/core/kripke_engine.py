"""
core/kripke_engine.py
Formal Kripke Structure & CTL Model Checking Engine
Designed for cross-platform execution (Windows, macOS, Linux).
Zero external dependencies.
"""

from typing import Dict, Set, List, Tuple, Any, Optional
import collections
import re


class KripkeStructure:
    """
    Kripke Structure M = (S, S0, R, L, AP)
    - S: Finite set of states
    - S0: Set of initial states (subset of S)
    - R: Transition relation (subset of S x S)
    - L: Labeling function mapping state -> set of atomic propositions
    """

    def __init__(self, name: str = "kripke_model"):
        self.name = name
        self.states: Set[str] = set()
        self.initial: Set[str] = set()
        self.transitions: Dict[str, Set[str]] = collections.defaultdict(set)
        self.predecessors: Dict[str, Set[str]] = collections.defaultdict(set)
        self.labels: Dict[str, Set[str]] = collections.defaultdict(set)
        self.state_metadata: Dict[str, Dict[str, Any]] = {}

    def add_state(self, state: str, atoms: Optional[List[str]] = None, is_initial: bool = False, meta: Optional[Dict] = None):
        self.states.add(state)
        if atoms:
            self.labels[state] = set(atoms)
        if is_initial:
            self.initial.add(state)
        if meta:
            self.state_metadata[state] = meta

    def add_transition(self, src: str, dst: str):
        self.states.add(src)
        self.states.add(dst)
        self.transitions[src].add(dst)
        self.predecessors[dst].add(src)

    def validate_totality(self) -> Tuple[bool, List[str]]:
        """
        Kripke requirement: relation R must be total (no deadlock states without successors).
        Returns (is_valid, list_of_deadlock_states).
        """
        deadlocks = [s for s in self.states if not self.transitions[s]]
        return (len(deadlocks) == 0, deadlocks)

    def get_reachable_states(self) -> Set[str]:
        """Returns all states reachable from initial states."""
        reachable = set()
        queue = collections.deque(self.initial)
        while queue:
            curr = queue.popleft()
            if curr not in reachable:
                reachable.add(curr)
                for nxt in self.transitions.get(curr, set()):
                    if nxt not in reachable:
                        queue.append(nxt)
        return reachable

    def find_shortest_path(self, start_states: Set[str], target_states: Set[str]) -> Optional[List[str]]:
        """BFS shortest path from any start state to any target state."""
        queue = collections.deque([[s] for s in start_states if s in self.states])
        visited = set(start_states)
        while queue:
            path = queue.popleft()
            curr = path[-1]
            if curr in target_states:
                return path
            for nxt in self.transitions.get(curr, set()):
                if nxt not in visited:
                    visited.add(nxt)
                    queue.append(path + [nxt])
        return None

    @classmethod
    def from_dict(cls, data: Dict[str, Any]) -> "KripkeStructure":
        sm = data.get("state_machine", {})
        ks = cls(name=data.get("name", "kripke_model"))
        
        states = sm.get("states", [])
        initials = set(sm.get("initial", []))
        labeling = sm.get("labeling", {})
        transitions = sm.get("transitions", [])

        for s in states:
            atoms = labeling.get(s, [])
            ks.add_state(s, atoms=atoms, is_initial=(s in initials))

        for trans in transitions:
            if len(trans) == 2:
                ks.add_transition(trans[0], trans[1])

        return ks

    def to_dict(self) -> Dict[str, Any]:
        return {
            "name": self.name,
            "state_machine": {
                "states": sorted(list(self.states)),
                "initial": sorted(list(self.initial)),
                "labeling": {s: sorted(list(self.labels[s])) for s in self.states},
                "transitions": [[src, dst] for src in self.states for dst in self.transitions[src]]
            }
        }


class CTLParser:
    """
    Recursive Descent parser for CTL formulas.
    Supported Syntax:
      - Atoms: identifier (e.g. buffer_safe, mem_violation)
      - Unary: ! (NOT)
      - Binary logic: & (AND), | (OR), -> (IMPLIES)
      - Temporal CTL:
          EX(p), AX(p)
          EF(p), AF(p)
          EG(p), AG(p)
          E[p U q], A[p U q]
    """

    def __init__(self, formula_str: str):
        self.raw = formula_str.strip()
        self.tokens = self._tokenize(self.raw)
        self.pos = 0

    def _tokenize(self, text: str) -> List[Tuple[str, str]]:
        token_spec = [
            ("IMPLIES", r"->"),
            ("AND", r"&"),
            ("OR", r"\|"),
            ("NOT", r"!"),
            ("LPAREN", r"\("),
            ("RPAREN", r"\)"),
            ("LBRACKET", r"\["),
            ("RBRACKET", r"\]"),
            ("U", r"\bU\b"),
            ("OP_CTL", r"\b(EX|AX|EF|AF|EG|AG)\b"),
            ("PATH_OP", r"\b(E|A)\b"),
            ("ATOM", r"[a-zA-Z0-9_]+"),
            ("WS", r"\s+"),
        ]
        tok_regex = "|".join(f"(?P<{pair[0]}>{pair[1]})" for pair in token_spec)
        tokens = []
        for mo in re.finditer(tok_regex, text):
            kind = mo.lastgroup
            val = mo.group()
            if kind == "WS":
                continue
            tokens.append((kind, val))
        return tokens

    def current(self) -> Optional[Tuple[str, str]]:
        if self.pos < len(self.tokens):
            return self.tokens[self.pos]
        return None

    def consume(self, expected_kind: Optional[str] = None) -> Tuple[str, str]:
        tok = self.current()
        if not tok:
            raise ValueError("Unexpected end of formula")
        if expected_kind and tok[0] != expected_kind:
            raise ValueError(f"Expected token {expected_kind}, got {tok[0]} ('{tok[1]}')")
        self.pos += 1
        return tok

    def parse(self) -> Dict[str, Any]:
        node = self.parse_implies()
        if self.pos < len(self.tokens):
            extra = self.tokens[self.pos]
            raise ValueError(f"Extra unexpected token: '{extra[1]}'")
        return node

    def parse_implies(self) -> Dict[str, Any]:
        left = self.parse_or()
        while self.current() and self.current()[0] == "IMPLIES":
            self.consume("IMPLIES")
            right = self.parse_implies()
            left = {"type": "->", "left": left, "right": right}
        return left

    def parse_or(self) -> Dict[str, Any]:
        left = self.parse_and()
        while self.current() and self.current()[0] == "OR":
            self.consume("OR")
            right = self.parse_and()
            left = {"type": "|", "left": left, "right": right}
        return left

    def parse_and(self) -> Dict[str, Any]:
        left = self.parse_unary()
        while self.current() and self.current()[0] == "AND":
            self.consume("AND")
            right = self.parse_unary()
            left = {"type": "&", "left": left, "right": right}
        return left

    def parse_unary(self) -> Dict[str, Any]:
        tok = self.current()
        if not tok:
            raise ValueError("Unexpected end of formula in expression")

        # NOT operator: !p
        if tok[0] == "NOT":
            self.consume("NOT")
            child = self.parse_unary()
            return {"type": "!", "child": child}

        # CTL unary operators: EX, AX, EF, AF, EG, AG
        if tok[0] == "OP_CTL":
            op_name = self.consume("OP_CTL")[1]
            self.consume("LPAREN")
            child = self.parse_implies()
            self.consume("RPAREN")
            return {"type": op_name, "child": child}

        # Until CTL operators: E[p U q], A[p U q]
        if tok[0] == "PATH_OP":
            quantifier = self.consume("PATH_OP")[1]
            self.consume("LBRACKET")
            left = self.parse_implies()
            self.consume("U")
            right = self.parse_implies()
            self.consume("RBRACKET")
            return {"type": f"{quantifier}U", "left": left, "right": right}

        # Parenthesized expression: ( p )
        if tok[0] == "LPAREN":
            self.consume("LPAREN")
            expr = self.parse_implies()
            self.consume("RPAREN")
            return expr

        # Atom
        if tok[0] == "ATOM":
            atom_val = self.consume("ATOM")[1]
            return {"type": "ATOM", "name": atom_val}

        raise ValueError(f"Unrecognized syntax at token '{tok[1]}'")


class CTLChecker:
    """
    Fixed-point Model Checking Algorithm for CTL (Computation Tree Logic).
    """

    def __init__(self, model: KripkeStructure):
        self.model = model

    def check_formula(self, formula_str: str) -> Dict[str, Any]:
        try:
            parser = CTLParser(formula_str)
            ast = parser.parse()
            sat_states = self.eval_ast(ast)
            
            # Global satisfaction check: does every initial state satisfy formula?
            is_satisfied = bool(self.model.initial) and self.model.initial.issubset(sat_states)
            
            # Counterexample / witness path search
            path, path_type = self._find_evidence_path(formula_str, ast, is_satisfied)

            sat_percentage = (len(sat_states) / len(self.model.states) * 100) if self.model.states else 0.0

            return {
                "formula": formula_str,
                "satisfied": is_satisfied,
                "sat_states": sorted(list(sat_states)),
                "initial_states": sorted(list(self.model.initial)),
                "violating_initial": sorted(list(self.model.initial - sat_states)),
                "sat_percentage": round(sat_percentage, 1),
                "path": path,
                "path_type": path_type,
                "error": None
            }
        except Exception as e:
            return {
                "formula": formula_str,
                "satisfied": False,
                "sat_states": [],
                "error": str(e),
                "path": None,
                "path_type": None
            }

    def eval_ast(self, ast: Dict[str, Any]) -> Set[str]:
        t = ast["type"]

        if t == "ATOM":
            atom_name = ast["name"]
            return {s for s in self.model.states if atom_name in self.model.labels.get(s, set())}

        if t == "!":
            return self.model.states - self.eval_ast(ast["child"])

        if t == "&":
            return self.eval_ast(ast["left"]) & self.eval_ast(ast["right"])

        if t == "|":
            return self.eval_ast(ast["left"]) | self.eval_ast(ast["right"])

        if t == "->":
            left_sat = self.eval_ast(ast["left"])
            right_sat = self.eval_ast(ast["right"])
            return (self.model.states - left_sat) | right_sat

        if t == "EX":
            target_sat = self.eval_ast(ast["child"])
            return self._pre_exists(target_sat)

        if t == "AX":
            target_sat = self.eval_ast(ast["child"])
            return self._pre_all(target_sat)

        if t == "EF":
            target_sat = self.eval_ast(ast["child"])
            return self._sat_EF(target_sat)

        if t == "AF":
            target_sat = self.eval_ast(ast["child"])
            return self._sat_AF(target_sat)

        if t == "EG":
            target_sat = self.eval_ast(ast["child"])
            return self._sat_EG(target_sat)

        if t == "AG":
            # AG(p) = !EF(!p)
            child_sat = self.eval_ast(ast["child"])
            bad_sat = self.model.states - child_sat
            reach_bad = self._sat_EF(bad_sat)
            return self.model.states - reach_bad

        if t == "EU":
            p_sat = self.eval_ast(ast["left"])
            q_sat = self.eval_ast(ast["right"])
            return self._sat_EU(p_sat, q_sat)

        if t == "AU":
            p_sat = self.eval_ast(ast["left"])
            q_sat = self.eval_ast(ast["right"])
            return self._sat_AU(p_sat, q_sat)

        raise ValueError(f"Unknown AST operator: {t}")

    def _pre_exists(self, target: Set[str]) -> Set[str]:
        """Pre_exists(Y) = { s | exists s' in Y with (s, s') in R }"""
        res = set()
        for t in target:
            res.update(self.model.predecessors.get(t, set()))
        return res

    def _pre_all(self, target: Set[str]) -> Set[str]:
        """Pre_all(Y) = { s | forall s', (s, s') in R implies s' in Y } and out-degree > 0"""
        res = set()
        for s in self.model.states:
            succ = self.model.transitions.get(s, set())
            if succ and succ.issubset(target):
                res.add(s)
        return res

    def _sat_EF(self, target: Set[str]) -> Set[str]:
        """Least fixed point of Y = target | Pre_exists(Y)"""
        curr = set(target)
        while True:
            nxt = curr | self._pre_exists(curr)
            if nxt == curr:
                return curr
            curr = nxt

    def _sat_AF(self, target: Set[str]) -> Set[str]:
        """Least fixed point of Y = target | Pre_all(Y)"""
        curr = set(target)
        while True:
            nxt = curr | self._pre_all(curr)
            if nxt == curr:
                return curr
            curr = nxt

    def _sat_EG(self, target: Set[str]) -> Set[str]:
        """Greatest fixed point of Y = target & Pre_exists(Y)"""
        curr = set(target)
        while True:
            nxt = curr & self._pre_exists(curr)
            if nxt == curr:
                return curr
            curr = nxt

    def _sat_EU(self, p: Set[str], q: Set[str]) -> Set[str]:
        """Least fixed point of Y = q | (p & Pre_exists(Y))"""
        curr = set(q)
        while True:
            nxt = curr | (p & self._pre_exists(curr))
            if nxt == curr:
                return curr
            curr = nxt

    def _sat_AU(self, p: Set[str], q: Set[str]) -> Set[str]:
        """Least fixed point of Y = q | (p & Pre_all(Y))"""
        curr = set(q)
        while True:
            nxt = curr | (p & self._pre_all(curr))
            if nxt == curr:
                return curr
            curr = nxt

    def _find_evidence_path(self, raw_formula: str, ast: Dict[str, Any], is_satisfied: bool) -> Tuple[Optional[List[str]], Optional[str]]:
        op = ast["type"]

        # 1. AG(!bad): If violated, find path to bad state
        if op == "AG":
            child = ast["child"]
            if child["type"] == "!":
                bad_states = self.eval_ast(child["child"])
                if not is_satisfied:
                    path = self.model.find_shortest_path(self.model.initial, bad_states)
                    return path, "counterexample"

            not_p = self.model.states - self.eval_ast(child)
            if not is_satisfied and not_p:
                path = self.model.find_shortest_path(self.model.initial, not_p)
                return path, "counterexample"

        # 2. EF(goal): If satisfied, find witness path from initial to goal
        if op == "EF":
            goal_states = self.eval_ast(ast["child"])
            if is_satisfied and goal_states:
                path = self.model.find_shortest_path(self.model.initial, goal_states)
                return path, "witness"

        # 3. AG(p -> AF(q)): Liveness / Response property
        if op == "AG" and ast["child"]["type"] == "->":
            p_states = self.eval_ast(ast["child"]["left"])
            q_states = self.eval_ast(ast["child"]["right"])
            if not is_satisfied:
                af_q = self._sat_AF(q_states)
                violation_candidates = p_states - af_q
                if violation_candidates:
                    path = self.model.find_shortest_path(self.model.initial, violation_candidates)
                    return path, "counterexample"
            else:
                p_path = self.model.find_shortest_path(self.model.initial, p_states)
                if p_path:
                    q_path = self.model.find_shortest_path({p_path[-1]}, q_states)
                    if q_path:
                        return p_path + q_path[1:], "witness"

        return None, None
