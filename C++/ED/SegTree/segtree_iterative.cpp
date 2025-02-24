// Segment Tree Iterativa - Max

struct Segtree{
    vector<int> t;
    int n;

    Segtree(int _n){
        n = _n;
        t.assign(2*n, 0);
    }

    int merge(int a, int b){
        return max(a, b);
    }

    void build(){
        for(int i=n-1; i>0; i--)
            t[i] = merge(t[i<<1], t[i<<1|1]);
    }

    int query(int l, int r){ // [l, r]
        int resl=-INF, resr=-INF;
        for(l+=n, r+=n+1; l<r; l>>=1, r>>=1){
            if(l&1) resl = merge(resl, t[l++]);
            if(r&1) resr = merge(t[--r], resr);
        }
        return merge(resl, resr);
    }

    void update(int p, int value){
        for(t[p+=n]=value; p >>= 1;)
            t[p] = merge(t[p<<1], t[p<<1|1]);
    }

};