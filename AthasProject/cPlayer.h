#pragma once
class cInventory;

class Player 
{
private:

	

	RECT _rc, _rc2;
	RECT _index[3];
	

	cInventory* _inventory;

	RECT _EX;
	RECT m_WeaRc;
	RECT m_ArRc;
	RECT m_RingRc;

public:

	HRESULT init(void);
	void release(void);
	void update(void);
	void render(void);

	int _lv;		//lv

	int _hp;		//hp
	int _currentHP; //현재 hp

	int _mp;		//mp
	int _currentMP; //현재 mp

	int _att;		//공격력

	int _def;		//방어력

	int _exp;		//경험치

	int addAtt;

	int addDef;

	int addMP;
	void Explanation();

	/*int getLV() { return _lv; }
	void setLV(int lv) { _lv = lv; }

	int getHP() { return _hp; }
	void setHP(int hp) { _hp = hp; }

	int getCurrentHP() { return _currentHP; }
	void setCurrentHP(int currentHP) { _currentHP = currentHP; }

	int getMP() { return _mp; }
	void setMP(int mp) { _mp = mp; }

	int getCurrentMP() { return _currentMP; }
	void setCurrentMP(int currentMP) { _currentMP = currentMP; }

	int getAtt() { return _att; }
	void setAtt(int att) { _att = att; }

	int getDef() { return _def; }
	void setDef(int def) { _def = def; }

	int getExp() { return _exp; }
	void setExp(int exp) { _exp = exp; }*/



	void setinventoryclass(cInventory* inven) { _inventory = inven; }


	Player();
	~Player();
};

