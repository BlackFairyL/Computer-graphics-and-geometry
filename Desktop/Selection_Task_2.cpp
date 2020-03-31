#include <iostream>
#include <vector>
#include <algorithm>
#include <iomanip>
#include <cmath>
#define ll long long

class point {
public:
    ll x, y;
    point() :x(0), y(0) {}
    point(ll x, ll y): x(x), y(y) {}
    friend std::istream& operator>>(std::istream& in, point& item);
    bool operator<(const point& item) const{
        return y < item.y;
    }
 };  

std::istream& operator>>(std::istream& in, point& item) {
    return in >> item.x >> item.y;
}

ll dist(point& a, point& b) {
    return (a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y);
}

void updateAns(point& a, point& b, ll& ans) {
    ll newDist = dist(a, b);
    if (ans > newDist) {
        ans = newDist;
    }
}

void merge(int l, int r, std::vector<point>& setPoint, std::vector<point>& mergePoint) {
    
    int m = (l + r) / 2;
    int i = l, j = m + 1, k = 0;

    while (i <= m && j <= r) {
        if (setPoint[i] < setPoint[j])
            mergePoint[k++] = setPoint[i++];
        else 
            mergePoint[k++] = setPoint[j++];
    }

    while (i <= m) {
        mergePoint[k++] = setPoint[i++];
    }

    while (j <= r) {
        mergePoint[k++] = setPoint[j++];
    }

    for (int i = l; i <= r; i++)
        setPoint[i] = mergePoint[i - l];
}

void search(int l, int r, ll& ans, std::vector<point>& setPoint) {
    if (r - l <= 3) {
        for (int i = l; i <= r; i++) {
            for (int j = i + 1; j <= r; j++) {
                updateAns(setPoint[i], setPoint[j], ans);
            }
        }
        std::sort(setPoint.begin() + l, setPoint.begin() + r + 1, [](point& a, point& b) { return a.y < b.y; });
        return;
    }

    int m = (l + r) / 2;
    ll x = setPoint[m].x;
    search(l, m, ans, setPoint);
    search(m + 1, r, ans, setPoint);
    std::vector<point>mergePoint(r - l + 1);
    merge(l, r, setPoint, mergePoint);

    int k = 0;
    for (int i = l; i <= r; i++) {
        if ((setPoint[i].x - x) * (setPoint[i].x - x) < ans)
            mergePoint[k++] = setPoint[i];
    }

    for (int i = 0; i < k; i++) {
        int cnt = 7, j = i + 1;
        while (cnt >= 0 && j < k) {
            updateAns(mergePoint[i], mergePoint[j], ans);
            cnt--;
            j++;
        }
    }
    return;
}

int main() {
    int n;
    std::cin >> n;
    std::vector<point>setPoint(n);
    for (int i = 0; i < n; i++) {
        std::cin >> setPoint[i];
    }
    std::sort(setPoint.begin(), setPoint.end(), [](point& a, point& b) { return a.x < b.x || a.x == b.x && a.y < b.y; } );
    
    ll ans = 1e18;
    search(0, n - 1, ans, setPoint);
    std::cout << ans;
    return 0;
}
