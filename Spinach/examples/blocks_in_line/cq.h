#ifndef CQ_H
#define CQ_H

#define QCAP 3
template <typename T>
class CircularQueue
{
public:
	CircularQueue()
		:r(-1),f(0),sz(0) {
	}

	bool isEmpty() const {
		return (sz <= 0);
	}

	bool isFull() const {
		return (sz >= QCAP);
	}

	bool inQ(const T& n) {
		if (isFull()) return false;
		r = (r + 1) % QCAP;
		arr[r] = n;
		++sz;
		return true;
	}

	bool inQ(T&& n) {
		if (isFull()) return false;
		r = (r + 1) % QCAP;
		arr[r] = std::move(n);
		++sz;
		return true;
	}

	bool delQ(T& out){
		if(isEmpty()){
			return false;
		}
		out = arr[f];
		f = ( f + 1 )% QCAP;
		--sz;
		return true;
	}

	bool front(T& out) {
		if (isEmpty()) {
			return false;
		}
		out = arr[f];
		return true;
	}

	bool onedown(T& out) {
		if (isEmpty() || sz < 2) {
			return false;
		}
		out = arr[(f + 1) % QCAP];
		return true;
	}


private:
	T arr[QCAP];
	int r;
	int f;
	int sz;
};

#endif
