// Application prints wrong output (5.07.. twice instead of 6.09.. twice)
// on VS2015 release x86
//
#include <vector>
#include<iostream>

using std::vector;
const int M = 2;
const int D = 2;

//In practice, we ensure that m_data has size M and m_data[i] has size pow(D,i+1)
class Signature {
public:
  vector<vector<double>> m_data;
};

//This function gives a signature object whose members are initialised as nice numbers
//e.g. make(7) -> [[7.0101, 7.0102], [7.0201, 7.0202, 7.0203, 7.0204]]
Signature make(double start) {
  Signature o;
  o.m_data.resize(M);
  int length = 1;
  for (int mm = 0; mm < M; ++mm) {
    length *= D;
    for (int i = 0; i < length; ++i)
      o.m_data[mm].push_back(start + 0.01 * (1 + mm) + 0.0001 * (1 + i));
  }
  return o;
}

//This function modifies s in an obscure way, 
//But VS 2015 in release mode x86 seems to do this wrong.
//Adding an output statement fixes it.
void backToSegment(int d, int m, const Signature& x, Signature& s) {
  const auto& segment = x.m_data[0];
  auto& dSegment = s.m_data[0];
  int level = M;
  auto i = s.m_data[1].begin();
  for (size_t j = 0; j < D; ++j)
    for (int dd = 0; dd < d; ++dd, ++i) {
      //std::cout<<" "; //uncomment this to fix
      s.m_data[0][j] += segment[dd] * (1.0 / level) * *i;
      //alternatively, replace the following with s.m_data[0][j] += ... to fix
      dSegment[dd] += x.m_data[level - 2][j] * (1.0 / level) * *i;
    }
}

int main() {
  auto s1 = make(1);
  auto s2 = make(2);
  backToSegment(D, M, s1, s2);
  for (auto f : s2.m_data[0])
    std::cout << f << "\n";
}

/*In this case, the call to backToSegment looks like this
  const auto segment = [1.0101, 1.0102];
  int level = 2;
    auto i = [2.0201, 2.0202, 2.0203, 2.0204].begin();
    for (size_t j=0u; j<2u; ++j)
      for(int dd = 0; dd<2; ++dd) {
        s[0][j]+=segment[dd] * 0.5 * *i;
        s[0][dd]+=segment[j] * 0.5 * *i;
        ++i;
      }
In other words:
s[0] begins as [2.0101, 2.0102]
We get
s[0][0]+=1.0101*0.5*2.0201 (1)
s[0][0]+=1.0101*0.5*2.0201 (1)
s[0][0]+=1.0102*0.5*2.0202
s[0][1]+=1.0101*0.5*2.0202
s[0][1]+=1.0101*0.5*2.0203
s[0][0]+=1.0102*0.5*2.0203
s[0][1]+=1.0102*0.5*2.0204 (2)
s[0][1]+=1.0102*0.5*2.0204 (2)
s[0] should end as [6.09145956, 6.091862605]
I get [5.07121,5.07136] which is compatible with only one of the lines labelled
(1) and only one of the (2) actually happening.
*/



