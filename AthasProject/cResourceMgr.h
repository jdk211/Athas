#pragma once

#include "cSingletonBase.h"
#include <string>
#include <map>


//T ���ҽ� �Ŵ����� �����ȴ� Type �� �ڷ���
template<typename T, typename A>
class cResourceMgr : public cSingletonBase<A>
{
protected:
	typedef std::map<string, T>		MAP_RESOURCE;
	MAP_RESOURCE					resourceTable;	//���ҽ� table;

public:
	cResourceMgr(){}
	~cResourceMgr(){}

	//���� ��η� ���ҽ��� �ҷ��´�.
	T GetResource(std::string filePath, void* pParam = NULL);

	//Ư�� ���ҽ� ����
	void RemoveResource(std::string filePath);

	//��� ���ҽ� ����
	void ClearResource();


protected:

	//�ε� ��ĸ� ������ 
	virtual T LoadResource(std::string filePath, void* pParam = NULL) = 0;

	//���� ��ĸ� ������
	virtual void ReleaseResource(T data) = 0;


};



//���� ��η� ���ҽ��� �ҷ��´�.
template< typename T, typename A>
T cResourceMgr<T,A>::GetResource(std::string filePath, void* pParam)
{
	MAP_RESOURCE::iterator find;

	//���� ����� ���ҽ��� resourceTable �� �ִ� ��Ȯ��
	find = resourceTable.find(filePath);

	//�̹� �����Ѵ�.
	if (find != resourceTable.end()){

		//ã�� ���� value �� ����
		return find->second;
	}

	//��������´ٸ� ����.

	//�ε� ( ������ �� LoadResource �� ����� )
	T newResource = LoadResource(filePath, pParam);

	//�ε��� �����ߴٸ�..
	if (newResource == 0)
		return 0;

	//Table �� �߰�
	resourceTable.insert(make_pair(filePath, newResource));


	return newResource;
}

//Ư�� ���ҽ� ����
template< typename T, typename A>
void cResourceMgr<T, A>::RemoveResource(std::string filePath)
{
	MAP_RESOURCE::iterator find;

	//����� ã�´�.
	find = resourceTable.find(filePath);


	//ã�Ҵٸ�
	if (find != resourceTable.end()){

		//ã�� ���ҽ��� ��������.
		ReleaseResource(find->second);


		//Map ���� ����
		resourceTable.erase(find);

	}


}

//��� ���ҽ� ����
template< typename T, typename A>
void cResourceMgr<T, A>::ClearResource()
{
	MAP_RESOURCE::iterator iter;
	for (iter = resourceTable.begin();
		iter != resourceTable.end();
		++iter)
	{
		//ã�� ���ҽ��� ��������.
		ReleaseResource(iter->second);
	}

	//table Ŭ����
	resourceTable.clear();



}