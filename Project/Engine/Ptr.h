#pragma once


template<typename T>
class Ptr
{
private:
	T* m_pRes;

public:
	T* Get() const { return m_pRes; }
	T** GetAdressOf() const { return &m_pRes; }

	void operator = (T* _pRes)
	{
		if (nullptr != m_pRes)
		{
			m_pRes->Release();
		}

		m_pRes = _pRes;

		if (nullptr != m_pRes)
		{
			m_pRes->AddRef();
		}
	}


	void operator = (const Ptr<T>& _Ptr)
	{
		if (nullptr != m_pRes)
		{
			m_pRes->Release();
		}

		m_pRes = _Ptr.m_pRes;

		if (nullptr != m_pRes)
		{
			m_pRes->AddRef();
		}
	}

	T* operator -> () const
	{
		return m_pRes;
	}

	bool operator == (void* _pRes) const
	{
		if (m_pRes == _pRes)
			return true;
		return false;
	}

	bool operator == (const Ptr<T>& _Other) const
	{
		if (m_pRes == _Other.m_pRes)
			return true;
		else
			return false;
	}

public:
	Ptr()
		: m_pRes(nullptr)
	{}


	Ptr(T* _pRes)
		: m_pRes(_pRes)
	{
		if (nullptr != m_pRes)
			m_pRes->AddRef();
	}

	Ptr(const Ptr<T>& _Ptr)
		: m_pRes(_Ptr.m_pRes)
	{
		if (nullptr != m_pRes)
			m_pRes->AddRef();
	}

	~Ptr()
	{
		if (nullptr != m_pRes)
			m_pRes->Release();
	}

};

template<typename T>
bool operator == (void* _Res, const Ptr<T>& _Ptr)
{
	if (_Res == _Ptr.Get())
	{
		return true;
	}

	return false;
}

template<typename T>
bool operator != (void* _Res, const Ptr<T>& _Ptr)
{
	if (_Res != _Ptr.Get())
	{
		return true;
	}

	return false;
}