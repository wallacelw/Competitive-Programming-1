cod areaT2(point &a, point &b, point &c){
    return abs((b-a)^(c-a));
}

typedef struct QuadEdge* Q;
struct QuadEdge {
	int id;
	point o;
	Q rot, nxt;
	bool used;

	QuadEdge(int id_ = -1, point o_ = point(INF, INF)) :
		id(id_), o(o_), rot(nullptr), nxt(nullptr), used(false) {}

	Q rev() const { return rot->rot; }
	Q next() const { return nxt; }
	Q prev() const { return rot->next()->rot; }
	point dest() const { return rev()->o; }
};

Q edge(point from, point to, int id_from, int id_to) {
	Q e1 = new QuadEdge(id_from, from);
	Q e2 = new QuadEdge(id_to, to);
	Q e3 = new QuadEdge;
	Q e4 = new QuadEdge;
	tie(e1->rot, e2->rot, e3->rot, e4->rot) = {e3, e4, e2, e1};
	tie(e1->nxt, e2->nxt, e3->nxt, e4->nxt) = {e1, e2, e4, e3};
	return e1;
}

void splice(Q a, Q b) {
	swap(a->nxt->rot->nxt, b->nxt->rot->nxt);
	swap(a->nxt, b->nxt);
}

void del_edge(Q& e, Q ne) { // delete e and assign e <- ne
	splice(e, e->prev());
	splice(e->rev(), e->rev()->prev());
	delete e->rev()->rot, delete e->rev();
	delete e->rot; delete e;
	e = ne;
}

Q conn(Q a, Q b) {
	Q e = edge(a->dest(), b->o, a->rev()->id, b->id);
	splice(e, a->rev()->prev());
	splice(e->rev(), b);
	return e;
}

bool in_c(point a, point b, point c, point p) { // p ta na circunf. (a, b, c) ?
	__int128 p2 = p*p, A = a*a - p2, B = b*b - p2, C = c*c - p2;
	return areaT2(p, a, b) * C + areaT2(p, b, c) * A + areaT2(p, c, a) * B > 0;
}

pair<Q, Q> build_tr(vector<point>& p, int l, int r) {
	if (r-l+1 <= 3) {
		Q a = edge(p[l], p[l+1], l, l+1), b = edge(p[l+1], p[r], l+1, r);
		if (r-l+1 == 2) return {a, a->rev()};
		splice(a->rev(), b);
		ll ar = areaT2(p[l], p[l+1], p[r]);
		Q c = ar ? conn(b, a) : 0;
		if (ar >= 0) return {a, b->rev()};
		return {c->rev(), c};
	}
	int m = (l+r)/2;
	auto [la, ra] = build_tr(p, l, m);
	auto [lb, rb] = build_tr(p, m+1, r);
	while (true) {
		if (ccw(lb->o, ra->o, ra->dest())) ra = ra->rev()->prev();
		else if (ccw(lb->o, ra->o, lb->dest())) lb = lb->rev()->next();
		else break;
	}
	Q b = conn(lb->rev(), ra);
	auto valid = [&](Q e) { return ccw(e->dest(), b->dest(), b->o); };
	if (ra->o == la->o) la = b->rev();
	if (lb->o == rb->o) rb = b;
	while (true) {
		Q L = b->rev()->next();
		if (valid(L)) while (in_c(b->dest(), b->o, L->dest(), L->next()->dest()))
			del_edge(L, L->next());
		Q R = b->prev();
		if (valid(R)) while (in_c(b->dest(), b->o, R->dest(), R->prev()->dest()))
			del_edge(R, R->prev());
		if (!valid(L) and !valid(R)) break;
		if (!valid(L) or (valid(R) and in_c(L->dest(), L->o, R->o, R->dest())))
			b = conn(R, b->rev());
		else b = conn(b->rev(), L->rev());
	}
	return {la, rb};
}

vector<vector<int>> delaunay(vp v) {
	int n = v.size();
	auto tmp = v;
	vector<int> idx(n);
	iota(idx.begin(), idx.end(), 0);
	sort(idx.begin(), idx.end(), [&](int l, int r) { return v[l] < v[r]; });
	for (int i = 0; i < n; i++) v[i] = tmp[idx[i]];
	assert(unique(v.begin(), v.end()) == v.end());
	vector<vector<int>> g(n);
	bool col = true;
	for (int i = 2; i < n; i++) if (areaT2(v[i], v[i-1], v[i-2])) col = false;
	if (col) {
		for (int i = 1; i < n; i++)
			g[idx[i-1]].push_back(idx[i]), g[idx[i]].push_back(idx[i-1]);
		return g;
	}
	Q e = build_tr(v, 0, n-1).first;
	vector<Q> edg = {e};
	for (int i = 0; i < edg.size(); e = edg[i++]) {
		for (Q at = e; !at->used; at = at->next()) {
			at->used = true;
			g[idx[at->id]].push_back(idx[at->rev()->id]);
			edg.push_back(at->rev());
		}
	}
	return g;
}
