#include <iostream>
#include <vector>
#include <MathGeoLib.h>

using namespace std;
using namespace math;

int main()
{
    vec a = {1,2,3};
    vec b = {4,5,6};
    vec c = {7,8,9};
    LineSegment ls{a,b};
    Triangle tr{a,b,c};
    cout << ls << endl;
    cout << tr << endl;
    vector<int> v = {1,2,3};
    for (auto x:v) cout << x << ',';
    cout << endl;
}
