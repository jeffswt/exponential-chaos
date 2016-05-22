
//	"triple_pair.h" -*- C++ -*-
//	Created on: 30 April 2016
//	Triple pairs, functionally similar to std::pair.
//
//	Copyright (C) 2016  Geoffrey Tang.
//
//	This library is distributed in the hope that it will be useful,
//	but WITHOUT ANY WARRANTY; without even the implied warranty of
//	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//	GNU General Public License for more details.
//
//	This library is distributed in the hope that it will be useful,
//	but WITHOUT ANY WARRANTY; without even the implied warranty of
//	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//	GNU General Public License for more details.
//
//	You should have received a copy of the GNU General Public License
//	along with this program.  If not, see <http://www.gnu.org/licenses/>.

#ifndef ALGO_TRIPLE_PAIR_H_
#define ALGO_TRIPLE_PAIR_H_

template <typename _T1, typename _T2, typename _T3>
struct	triple_pair
{
public:
	_T1 first;
	_T2 second;
	_T3 third;
	triple_pair(_T1 __x, _T2 __y, _T3 __z)
		{ first = __x, second = __y, third = __z; }
	triple_pair()
		{ first = 0, second = 0, third = 0; }
};

template <typename _T1, typename _T2, typename _T3>
triple_pair<_T1, _T2, _T3> make_triple_pair(_T1 __x, _T2 __y, _T3 __z)
	{ return triple_pair<_T1, _T2, _T3>(__x, __y, __z); }

template <typename _T1, typename _T2, typename _T3>
bool operator == (triple_pair<_T1, _T2, _T3> __x, triple_pair<_T1, _T2, _T3> __y)
	{ return __x.first == __y.first && __x.second == __y.second && __x.third == __y.third; }

template <typename _T1, typename _T2, typename _T3>
bool operator < (triple_pair<_T1, _T2, _T3> __x, triple_pair<_T1, _T2, _T3> __y)
	{ if (__x.first < __y.first) return true; if (__x.first > __y.first) return false;
	  if (__x.second < __y.second) return true; if (__x.second > __y.second) return false;
	  if (__x.third < __y.third) return true; if (__x.third > __y.third) return false;
	  return false; }

template <typename _T1, typename _T2, typename _T3>
bool operator > (triple_pair<_T1, _T2, _T3> __x, triple_pair<_T1, _T2, _T3> __y)
	{ return !(__x < __y) && !(__x == __y); }

template <typename _T1, typename _T2, typename _T3>
bool operator != (triple_pair<_T1, _T2, _T3> __x, triple_pair<_T1, _T2, _T3> __y)
	{ return __x.first != __y.first || __x.second != __y.second || __x.third != __y.third; }

template <typename _T1, typename _T2, typename _T3>
bool operator <= (triple_pair<_T1, _T2, _T3> __x, triple_pair<_T1, _T2, _T3> __y)
	{ return __x < __y || __x == __y; }

template <typename _T1, typename _T2, typename _T3>
bool operator >= (triple_pair<_T1, _T2, _T3> __x, triple_pair<_T1, _T2, _T3> __y)
	{ return !(__x < __y); }

#endif /* ALGO_TRIPLE_PAIR_H_ */
