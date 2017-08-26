#pragma once

#include "cSingletonBase.h"
#include <string>
#include <map>


//T 리소스 매니져로 관리된는 Type 의 자료형
template<typename T, typename A>
class cResourceMgr : public cSingletonBase<A>
{
protected:
	typedef std::map<string, T>		MAP_RESOURCE;
	MAP_RESOURCE					resourceTable;	//리소스 table;

public:
	cResourceMgr(){}
	~cResourceMgr(){}

	//파일 경로로 리소스를 불러온다.
	T GetResource(std::string filePath, void* pParam = NULL);

	//특정 리소스 해제
	void RemoveResource(std::string filePath);

	//모든 리소스 해제
	void ClearResource();


protected:

	//로드 방식만 재정의 
	virtual T LoadResource(std::string filePath, void* pParam = NULL) = 0;

	//해재 방식만 재정의
	virtual void ReleaseResource(T data) = 0;


};



//파일 경로로 리소스를 불러온다.
template< typename T, typename A>
T cResourceMgr<T,A>::GetResource(std::string filePath, void* pParam)
{
	MAP_RESOURCE::iterator find;

	//얻어올 경로의 리소스가 resourceTable 에 있는 지확인
	find = resourceTable.find(filePath);

	//이미 존재한다.
	if (find != resourceTable.end()){

		//찾은 놈의 value 를 리턴
		return find->second;
	}

	//여기까지온다면 없다.

	//로딩 ( 재정의 된 LoadResource 가 실행됨 )
	T newResource = LoadResource(filePath, pParam);

	//로딩에 실패했다면..
	if (newResource == 0)
		return 0;

	//Table 에 추가
	resourceTable.insert(make_pair(filePath, newResource));


	return newResource;
}

//특정 리소스 해제
template< typename T, typename A>
void cResourceMgr<T, A>::RemoveResource(std::string filePath)
{
	MAP_RESOURCE::iterator find;

	//지울놈 찾는다.
	find = resourceTable.find(filePath);


	//찾았다면
	if (find != resourceTable.end()){

		//찾은 리소스를 해제하자.
		ReleaseResource(find->second);


		//Map 에서 삭제
		resourceTable.erase(find);

	}


}

//모든 리소스 해제
template< typename T, typename A>
void cResourceMgr<T, A>::ClearResource()
{
	MAP_RESOURCE::iterator iter;
	for (iter = resourceTable.begin();
		iter != resourceTable.end();
		++iter)
	{
		//찾은 리소스를 해제하자.
		ReleaseResource(iter->second);
	}

	//table 클리어
	resourceTable.clear();



}