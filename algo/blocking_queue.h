
//	"blocking_queue.h" -*- C++ -*-
//	Created on: 15 May 2016
//	An implementation of blocking queues.
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

#ifndef ALGO_BLOCKING_QUEUE
#define ALGO_BLOCKING_QUEUE

#include <queue>
#include <mutex>

template <typename _T>
class	blocking_queue
{
protected:
	std::queue<_T>	Queue;
	std::mutex		Lock;
	std::mutex		IsEmpty;
	bool			Released;
public:
	_T&	front(void)
	{
		IsEmpty.lock();
		IsEmpty.unlock();
		Lock.lock();
		_T&	Reference = Queue.front();
		Lock.unlock();
		if (Released) {
			throw "Queue is empty";
		}
		return Reference;
	}
	void pop(void)
	{
		Lock.lock();
		Queue.pop();
		if (Queue.empty())
			IsEmpty.lock();
		Lock.unlock();
		return ;
	}
	void push(_T& __x)
	{
		Lock.lock();
		Released = false;
		Queue.push(__x);
		if (!Queue.empty())
			IsEmpty.unlock();
		Lock.unlock();
		return ;
	}
	bool empty(void)
	{
		return Released;
	}
	void unrelease(void)
	{
		Released = false;
		IsEmpty.lock();
		return ;
	}
	void release(void)
	{
		Released = true;
		IsEmpty.unlock();
		return ;
	}
	blocking_queue(void)
	{
		Lock.lock();
		IsEmpty.lock();
		while (!Queue.empty())
			Queue.pop();
		Released = false;
		Lock.unlock();
		return ;
	}
	~blocking_queue(void)
	{
		release();
		return ;
	}
};

#endif /* ALGO_BLOCKING_QUEUE */

