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

	void Clear() {
		r = -1;
		f = 0;
		sz = 0;
	}
	bool IsEmpty() const {
		return (sz <= 0);
	}

	bool IsFull() const {
		return (sz >= QCAP);
	}

	bool InQ(const T& n) {
		if (IsFull()) return false;
		r = (r + 1) % QCAP;
		arr[r] = n;
		++sz;
		return true;
	}

	bool InQ(T&& n) {
		if (IsFull()) return false;
		r = (r + 1) % QCAP;
		arr[r] = std::move(n);
		++sz;
		return true;
	}

	bool DelQ(T& out){
		if(IsEmpty()){
			return false;
		}
		out = arr[f];
		f = ( f + 1 )% QCAP;
		--sz;
		return true;
	}

	bool First(T& out) {
		if (IsEmpty()) {
			return false;
		}
		out = arr[f];
		return true;
	}

	bool Second(T& out) {
		if (IsEmpty() || sz < 2) {
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
