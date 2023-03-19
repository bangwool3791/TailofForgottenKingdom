#pragma once


struct tColor {
	UINT r, g, b;
	tColor();
	tColor(float r, float g, float b);
	~tColor() {}
};

class CImage
{
public:
	CImage(int widht, int height);
	~CImage();


	tColor GetColor(int x, int y) const;
	void SetColor(const tColor& color, int x, int y);
	
	void Export(const char* path) const;
private:
	int m_width;
	int m_height;
	vector<tColor> m_colors;

};