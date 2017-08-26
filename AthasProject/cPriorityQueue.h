#pragma once


template <class T>
class cPriorityQueue
{
	// 리턴값이 0 이면 a 와 b 는 같다.
	//리턴값이 1 이면 a 가 b 보다 크다 ( 우선순위에서 밀린다 )
	//리컨값이 -1 이며 a 가 b 보다 작다 ( 우선순위가 높다 )
	typedef int(*ComparePtr)(T a, T b);		//자신의 데이터와 비요할수 있는 함수포인터 형

	//모든 요소 Action 처리
	typedef void(*Action)(T& a);


private:
	T*				pData;		//저장될 데이터 배열
	int				capacity;
	int				size;
	ComparePtr		compare;	//비교자 함수


public:
	cPriorityQueue();
	cPriorityQueue(int capacity);
	~cPriorityQueue();

	//데이터 추가
	void Enqueue(T data);

	//데이터 빼는거
	T Dequeue();

	int GetSize()
	{
		return this->size;
	}

	bool IsEmpty()
	{
		return this->size == 0;
	}

	//비고 함수 셋팅
	void SetCompareFunc(ComparePtr pCompare)
	{
		this->compare = pCompare;
	}

	void Clear()
	{
		this->size = 0;
	}

	void DoAction(Action actionFunc);

private:

	//현재 인덱스로 부모의 인덱스를 얻는다.
	int GetParentIndex(int curIndex);


	//현재 인덱스로 왼쪽자식의 인덱스를 얻는다.
	int GetLeftChildIndex(int curIndex);


};


///////////////////////////////////////////

template <class T>
cPriorityQueue<T>::cPriorityQueue()
: capacity(10), size(0), compare( NULL )
{
	this->pData = new T[capacity];
}


template <class T>
cPriorityQueue<T>::cPriorityQueue(int capacity)
{
	if (capacity <= 0)
		capacity = 1;

	this->capacity = capacity;
	this->size = 0;

	this->pData = new T[this->capacity];

}

template<class T>
cPriorityQueue<T>::~cPriorityQueue()
{
	delete[] this->pData;
}


//데이터 추가
template<class T>
void cPriorityQueue<T>::Enqueue(T data)
{
	//동적 재할당
	if (this->size == this->capacity){

		//새로운 용량은 이전의 2 배 ( 참고 stl vector 는 이전의 1.5 배 증가 된다 )
		int newCapacity = this->capacity * 2;

		//새로운 공간 할당
		T* pNewData = new T[newCapacity];

		//이사
		for (int i = 0; i < size; i++)
			pNewData[i] = pData[i];

		//기존집 철거
		delete[] pData;

		//등기부 등록
		pData = pNewData;
		capacity = newCapacity;
	}


	//일단은 맨마지막에 추가
	pData[size] = data;


	//뒤처리....

	//현위치
	int curIndex = size;

	//위로 올라가면서 부모랑 확인
	while (curIndex > 0){

		//부모인덱스 얻는다.
		int parentIndex = this->GetParentIndex(curIndex);

		//내가 부모보다 크다면..
		//if (pData[curIndex] >= pData[parentIndex]){
		if (compare(pData[curIndex], pData[parentIndex]) >= 0 ){

			//자리를 찾았다.
			break;
		}


		//부모랑 바꿔
		T temp = pData[curIndex];
		pData[curIndex] = pData[parentIndex];
		pData[parentIndex] = temp;

		//부모부터 다시
		curIndex = parentIndex;
	}

	//추가 되었으니 증가
	size++;

}

//데이터 빼는거
template<class T>
T cPriorityQueue<T>::Dequeue()
{
	//빼는것은 일단 맨앞에것
	T result = this->pData[0];

	//마지막에 있는 것을 처음으로.
	this->pData[0] = this->pData[--size];

	//현재인덱스
	int curIndex = 0;

	//위에서 부터 내려간다.
	while (true){

		//양자식의 인덱스를 얻는다.
		int leftChildIndex = this->GetLeftChildIndex(curIndex);
		int rightChildIndex = leftChildIndex + 1;

		//양쪽자식에서 작은쪽인덱스를 선택
		int selectIndex = 0;

		//자식이 없다
		if (leftChildIndex >= size){
			break;
		}

		//왼쪽자식만 있다.
		else if (leftChildIndex == size - 1){
			selectIndex = leftChildIndex;
		}
		else{
			//양쪽자식에서 작은쪽인덱스를 선택
			//if (this->pData[leftChildIndex] < this->pData[rightChildIndex])
			if (this->compare(this->pData[leftChildIndex], this->pData[rightChildIndex]) < 0 )
				selectIndex = leftChildIndex;
			else
				selectIndex = rightChildIndex;

		}

		//작은 쪽 인덱스랑 비교
		//if (this->pData[selectIndex] >= this->pData[curIndex]){
		if (this->compare(this->pData[selectIndex], this->pData[curIndex]) >= 0){
			//두자식보다 내가 작은것이기때문에 자리를 찾앗음...
			break;
		}

		//바꿔
		T temp = this->pData[selectIndex];
		this->pData[selectIndex] = this->pData[curIndex];
		this->pData[curIndex] = temp;

		//자식부터....
		curIndex = selectIndex;

	}




	return result;
}

template<class T>
void cPriorityQueue<T>::DoAction(Action actionFunc)
{
	for (int i = 0; i < this->size; i++){
		actionFunc(this->pData[i]);
	}
}

///////////////////////////////////////////


//현재 인덱스로 부모의 인덱스를 얻는다.
template<class T>
int cPriorityQueue<T>::GetParentIndex(int curIndex)
{
	return (curIndex - 1) / 2;
}


//현재 인덱스로 왼쪽자식의 인덱스를 얻는다.
template<class T>
int cPriorityQueue<T>::GetLeftChildIndex(int curIndex)
{
	return (curIndex * 2) + 1;
}
