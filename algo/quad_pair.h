
//	"quad_pair.h" -*- C++ -*-
//	Created on: 30 April 2016
//	Quad pair, functionally similar to std::pair.
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

#ifndef ALGO_QUAD_PAIR_H_
#define ALGO_QUAD_PAIR_H_

template <typename _T1, typename _T2, typename _T3, typename _T4>
struct quad_pair
{
public:
	_T1 first;
	_T2 second;
	_T3 third;
	_T4 fourth;
	quad_pair(void)
		{ first = 0; second = 0; third = 0; fourth = 0; }
	quad_pair(_T1 __x, _T2 __y, _T3 __z, _T4 __w)
		{ first = __x; second = __y; third = __z; fourth = __w; }
};

template <typename _T1, typename _T2, typename _T3, typename _T4>
quad_pair	make_quad_pair(_T1 __x, _T2 __y, _T3 __z, _T4 __w)
	{ return quad_pair<_T1, _T2, _T3, _T4>(__x, __y, __z, __w); }

#endif /* ALGO_QUAD_PAIR_H_ */
