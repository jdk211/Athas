#pragma once


template <class T>
class cPriorityQueue
{
	// ���ϰ��� 0 �̸� a �� b �� ����.
	//���ϰ��� 1 �̸� a �� b ���� ũ�� ( �켱�������� �и��� )
	//�������� -1 �̸� a �� b ���� �۴� ( �켱������ ���� )
	typedef int(*ComparePtr)(T a, T b);		//�ڽ��� �����Ϳ� ����Ҽ� �ִ� �Լ������� ��

	//��� ��� Action ó��
	typedef void(*Action)(T& a);


private:
	T*				pData;		//����� ������ �迭
	int				capacity;
	int				size;
	ComparePtr		compare;	//���� �Լ�


public:
	cPriorityQueue();
	cPriorityQueue(int capacity);
	~cPriorityQueue();

	//������ �߰�
	void Enqueue(T data);

	//������ ���°�
	T Dequeue();

	int GetSize()
	{
		return this->size;
	}

	bool IsEmpty()
	{
		return this->size == 0;
	}

	//��� �Լ� ����
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

	//���� �ε����� �θ��� �ε����� ��´�.
	int GetParentIndex(int curIndex);


	//���� �ε����� �����ڽ��� �ε����� ��´�.
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


//������ �߰�
template<class T>
void cPriorityQueue<T>::Enqueue(T data)
{
	//���� ���Ҵ�
	if (this->size == this->capacity){

		//���ο� �뷮�� ������ 2 �� ( ���� stl vector �� ������ 1.5 �� ���� �ȴ� )
		int newCapacity = this->capacity * 2;

		//���ο� ���� �Ҵ�
		T* pNewData = new T[newCapacity];

		//�̻�
		for (int i = 0; i < size; i++)
			pNewData[i] = pData[i];

		//������ ö��
		delete[] pData;

		//���� ���
		pData = pNewData;
		capacity = newCapacity;
	}


	//�ϴ��� �Ǹ������� �߰�
	pData[size] = data;


	//��ó��....

	//����ġ
	int curIndex = size;

	//���� �ö󰡸鼭 �θ�� Ȯ��
	while (curIndex > 0){

		//�θ��ε��� ��´�.
		int parentIndex = this->GetParentIndex(curIndex);

		//���� �θ𺸴� ũ�ٸ�..
		//if (pData[curIndex] >= pData[parentIndex]){
		if (compare(pData[curIndex], pData[parentIndex]) >= 0 ){

			//�ڸ��� ã�Ҵ�.
			break;
		}


		//�θ�� �ٲ�
		T temp = pData[curIndex];
		pData[curIndex] = pData[parentIndex];
		pData[parentIndex] = temp;

		//�θ���� �ٽ�
		curIndex = parentIndex;
	}

	//�߰� �Ǿ����� ����
	size++;

}

//������ ���°�
template<class T>
T cPriorityQueue<T>::Dequeue()
{
	//���°��� �ϴ� �Ǿտ���
	T result = this->pData[0];

	//�������� �ִ� ���� ó������.
	this->pData[0] = this->pData[--size];

	//�����ε���
	int curIndex = 0;

	//������ ���� ��������.
	while (true){

		//���ڽ��� �ε����� ��´�.
		int leftChildIndex = this->GetLeftChildIndex(curIndex);
		int rightChildIndex = leftChildIndex + 1;

		//�����ڽĿ��� �������ε����� ����
		int selectIndex = 0;

		//�ڽ��� ����
		if (leftChildIndex >= size){
			break;
		}

		//�����ڽĸ� �ִ�.
		else if (leftChildIndex == size - 1){
			selectIndex = leftChildIndex;
		}
		else{
			//�����ڽĿ��� �������ε����� ����
			//if (this->pData[leftChildIndex] < this->pData[rightChildIndex])
			if (this->compare(this->pData[leftChildIndex], this->pData[rightChildIndex]) < 0 )
				selectIndex = leftChildIndex;
			else
				selectIndex = rightChildIndex;

		}

		//���� �� �ε����� ��
		//if (this->pData[selectIndex] >= this->pData[curIndex]){
		if (this->compare(this->pData[selectIndex], this->pData[curIndex]) >= 0){
			//���ڽĺ��� ���� �������̱⶧���� �ڸ��� ã����...
			break;
		}

		//�ٲ�
		T temp = this->pData[selectIndex];
		this->pData[selectIndex] = this->pData[curIndex];
		this->pData[curIndex] = temp;

		//�ڽĺ���....
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


//���� �ε����� �θ��� �ε����� ��´�.
template<class T>
int cPriorityQueue<T>::GetParentIndex(int curIndex)
{
	return (curIndex - 1) / 2;
}


//���� �ε����� �����ڽ��� �ε����� ��´�.
template<class T>
int cPriorityQueue<T>::GetLeftChildIndex(int curIndex)
{
	return (curIndex * 2) + 1;
}
