#pragma once

#include <algorithm>
#include <queue>
#include <cstdio>
#include "image.hpp"

typedef std::pair<int,int> pii;
#define mp std::make_pair
#define X first
#define Y second

class Element {
public:
    virtual void draw(Image &img) = 0;
    virtual ~Element() = default;
};

class Line : public Element {
public:
    int xA, yA;
    int xB, yB;
    Vector3f color;
    
    void draw_rev(int x, int y, Image &img, bool rev) {
        rev ?  0 <= y && y < img.w && 0 <= x && x < img.h ? img.set(y,x,color),0 : 0
            : (0 <= x && x < img.w && 0 <= y && y < img.h ? img.set(x,y,color),0 : 0);
    }

    void draw(Image &img) override {
        int xS = xA, yS = yA, xT = xB, yT = yB; bool rev = (yT>yS ? yT-yS : yS-yT) > (xT>xS ? xT-xS : xS-xT);
        rev ? xS^=yS^=xS^=yS, xT^=yT^=xT^=yT : 0; xS > xT ? xS^=xT^=xS^=xT, yS^=yT^=yS^=yT : 0; bool d = yT>=yS;
        for(int x = xS, y = yS, e = xS-xT; x <= xT; draw_rev(x++, (e+=(d?yT-yS:yS-yT)<<1)>=0 ? e+=xS-xT<<1, d?y++:y-- : y, img, rev));
    }
};

class Circle : public Element {
public:
    int cx, cy;
    int radius;
    Vector3f color;
    
    void draw_8(int dx, int dy, Image &img) {
        0 <= dx+cx &&  dx+cx < img.w && 0 <=  dy+cy &&  dy+cy < img.h ?  img.set( dx+cx, dy+cy, color),0 : 0;
        0 <=-dx+cx && -dx+cx < img.w && 0 <=  dy+cy &&  dy+cy < img.h ?  img.set(-dx+cx, dy+cy, color),0 : 0;
        0 <= dx+cx &&  dx+cx < img.w && 0 <= -dy+cy && -dy+cy < img.h ?  img.set( dx+cx,-dy+cy, color),0 : 0;
        0 <=-dx+cx && -dx+cx < img.w && 0 <= -dy+cy && -dy+cy < img.h ?  img.set(-dx+cx,-dy+cy, color),0 : 0;
        0 <= dy+cx &&  dy+cx < img.w && 0 <=  dx+cy &&  dx+cy < img.h ?  img.set( dy+cx, dx+cy, color),0 : 0;
        0 <=-dy+cx && -dy+cx < img.w && 0 <=  dx+cy &&  dx+cy < img.h ?  img.set(-dy+cx, dx+cy, color),0 : 0;
        0 <= dy+cx &&  dy+cx < img.w && 0 <= -dx+cy && -dx+cy < img.h ?  img.set( dy+cx,-dx+cy, color),0 : 0;
        0 <=-dy+cx && -dy+cx < img.w && 0 <= -dx+cy && -dx+cy < img.h ?  img.set(-dy+cx,-dx+cy, color),0 : 0;
    }

    void draw(Image &img) override {
        draw_8(0,radius,img); for(int x = 0, y = radius, d = 5-(y<<2); x <= y; d += d>=0 ? (x-y--<<3)+20 : (x<<3)+12, draw_8(++x,y,img));
    }
};

class Fill : public Element {
public:
    int cx, cy;
    Vector3f color;
    void draw(Image &img) override {
        std::queue<pii> q; q.push(mp(cx, cy)); Vector3f c = img.get(cx, cy); if(c==color) return;
        for(int l, r, x, y; !q.empty() && (img.set(x=q.front().X,y=q.front().Y,color), true); q.pop()){
            for(r = -~x; r< img.w && img.get(r,y)==c; img.set(r++,y,color)); r--;
            for(l = ~-x; l>=  0   && img.get(l,y)==c; img.set(l--,y,color)); l++;
            for(x = l; y>    0    && x <= r; img.get(x,~-y)==c&&(x==r||img.get(-~x,~-y)!=c) ? q.push(mp(x,~-y)),0 : 0, x++);
            for(x = l; y< ~-img.h && x <= r; img.get(x,-~y)==c&&(x==r||img.get(-~x,-~y)!=c) ? q.push(mp(x,-~y)),0 : 0, x++);
        }
    }
};